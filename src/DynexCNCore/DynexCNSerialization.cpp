// Copyright (c) 2021-2022, Dynex Developers
// 
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice, this list of
//    conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice, this list
//    of conditions and the following disclaimer in the documentation and/or other
//    materials provided with the distribution.
// 
// 3. Neither the name of the copyright holder nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 
// Parts of this project are originally copyright by:
// Copyright (c) 2012-2016, The DynexCN developers, The Bytecoin developers
// Copyright (c) 2014-2018, The Monero project
// Copyright (c) 2014-2018, The Forknote developers
// Copyright (c) 2018, The TurtleCoin developers
// Copyright (c) 2016-2018, The Karbowanec developers
// Copyright (c) 2017-2022, The CROAT.community developers


#include "DynexCNSerialization.h"

#include <algorithm>
#include <sstream>
#include <stdexcept>

#include <boost/variant/static_visitor.hpp>
#include <boost/variant/apply_visitor.hpp>

#include "Serialization/ISerializer.h"
#include "Serialization/SerializationOverloads.h"
#include "Serialization/BinaryInputStreamSerializer.h"
#include "Serialization/BinaryOutputStreamSerializer.h"

#include "Common/StringOutputStream.h"
#include "crypto/crypto.h"

#include "Account.h"
#include "DynexCNConfig.h"
#include "DynexCNFormatUtils.h"
#include "DynexCNTools.h"
#include "TransactionExtra.h"

using namespace Common;

namespace {

using namespace DynexCN;
using namespace Common;

size_t getSignaturesCount(const TransactionInput& input) {
  struct txin_signature_size_visitor : public boost::static_visitor < size_t > {
    size_t operator()(const BaseInput& txin) const { return 0; }
    size_t operator()(const KeyInput& txin) const { return txin.outputIndexes.size(); }
    size_t operator()(const MultisignatureInput& txin) const { return txin.signatureCount; }
  };

  return boost::apply_visitor(txin_signature_size_visitor(), input);
}

struct BinaryVariantTagGetter: boost::static_visitor<uint8_t> {
  uint8_t operator()(const DynexCN::BaseInput) { return  0xff; }
  uint8_t operator()(const DynexCN::KeyInput) { return  0x2; }
  uint8_t operator()(const DynexCN::MultisignatureInput) { return  0x3; }
  uint8_t operator()(const DynexCN::KeyOutput) { return  0x2; }
  uint8_t operator()(const DynexCN::MultisignatureOutput) { return  0x3; }
  uint8_t operator()(const DynexCN::Transaction) { return  0xcc; }
  uint8_t operator()(const DynexCN::Block) { return  0xbb; }
};

struct VariantSerializer : boost::static_visitor<> {
  VariantSerializer(DynexCN::ISerializer& serializer, const std::string& name) : s(serializer), name(name) {}

  template <typename T>
  void operator() (T& param) { s(param, name); }

  DynexCN::ISerializer& s;
  std::string name;
};

void getVariantValue(DynexCN::ISerializer& serializer, uint8_t tag, DynexCN::TransactionInput& in) {
  switch(tag) {
  case 0xff: {
    DynexCN::BaseInput v;
    serializer(v, "value");
    in = v;
    break;
  }
  case 0x2: {
    DynexCN::KeyInput v;
    serializer(v, "value");
    in = v;
    break;
  }
  case 0x3: {
    DynexCN::MultisignatureInput v;
    serializer(v, "value");
    in = v;
    break;
  }
  default:
    throw std::runtime_error("Unknown variant tag");
  }
}

void getVariantValue(DynexCN::ISerializer& serializer, uint8_t tag, DynexCN::TransactionOutputTarget& out) {
  switch(tag) {
  case 0x2: {
    DynexCN::KeyOutput v;
    serializer(v, "data");
    out = v;
    break;
  }
  case 0x3: {
    DynexCN::MultisignatureOutput v;
    serializer(v, "data");
    out = v;
    break;
  }
  default:
    throw std::runtime_error("Unknown variant tag");
  }
}

template <typename T>
bool serializePod(T& v, Common::StringView name, DynexCN::ISerializer& serializer) {
  return serializer.binary(&v, sizeof(v), name);
}

bool serializeVarintVector(std::vector<uint32_t>& vector, DynexCN::ISerializer& serializer, Common::StringView name) {
  size_t size = vector.size();
  
  if (!serializer.beginArray(size, name)) {
    vector.clear();
    return false;
  }

  vector.resize(size);

  for (size_t i = 0; i < size; ++i) {
    serializer(vector[i], "");
  }

  serializer.endArray();
  return true;
}

}

namespace Crypto {

bool serialize(PublicKey& pubKey, Common::StringView name, DynexCN::ISerializer& serializer) {
  return serializePod(pubKey, name, serializer);
}

bool serialize(SecretKey& secKey, Common::StringView name, DynexCN::ISerializer& serializer) {
  return serializePod(secKey, name, serializer);
}

bool serialize(Hash& h, Common::StringView name, DynexCN::ISerializer& serializer) {
  return serializePod(h, name, serializer);
}

bool serialize(KeyImage& keyImage, Common::StringView name, DynexCN::ISerializer& serializer) {
  return serializePod(keyImage, name, serializer);
}

bool serialize(chacha8_iv& chacha, Common::StringView name, DynexCN::ISerializer& serializer) {
  return serializePod(chacha, name, serializer);
}

bool serialize(Signature& sig, Common::StringView name, DynexCN::ISerializer& serializer) {
  return serializePod(sig, name, serializer);
}

bool serialize(EllipticCurveScalar& ecScalar, Common::StringView name, DynexCN::ISerializer& serializer) {
  return serializePod(ecScalar, name, serializer);
}

bool serialize(EllipticCurvePoint& ecPoint, Common::StringView name, DynexCN::ISerializer& serializer) {
  return serializePod(ecPoint, name, serializer);
}

}

namespace DynexCN {

void serialize(TransactionPrefix& txP, ISerializer& serializer) {
  serializer(txP.version, "version");

  if (CURRENT_TRANSACTION_VERSION < txP.version) {
    throw std::runtime_error("Wrong transaction version");
  }

  serializer(txP.unlockTime, "unlock_time");
  serializer(txP.inputs, "vin");
  serializer(txP.outputs, "vout");
  serializeAsBinary(txP.extra, "extra", serializer);
}

void serialize(Transaction& tx, ISerializer& serializer) {
  serialize(static_cast<TransactionPrefix&>(tx), serializer);

  size_t sigSize = tx.inputs.size();
  //TODO: make arrays without sizes
//  serializer.beginArray(sigSize, "signatures");
  
  //if (serializer.type() == ISerializer::INPUT) {
  // ignore base transaction
  if (serializer.type() == ISerializer::INPUT && !(sigSize == 1 && tx.inputs[0].type() == typeid(BaseInput))) {
    tx.signatures.resize(sigSize);
  }

  bool signaturesNotExpected = tx.signatures.empty();
  if (!signaturesNotExpected && tx.inputs.size() != tx.signatures.size()) {
    throw std::runtime_error("Serialization error: unexpected signatures size");
  }

  for (size_t i = 0; i < tx.inputs.size(); ++i) {
    size_t signatureSize = getSignaturesCount(tx.inputs[i]);
    if (signaturesNotExpected) {
      if (signatureSize == 0) {
        continue;
      } else {
        throw std::runtime_error("Serialization error: signatures are not expected");
      }
    }

    if (serializer.type() == ISerializer::OUTPUT) {
      if (signatureSize != tx.signatures[i].size()) {
        throw std::runtime_error("Serialization error: unexpected signatures size");
      }

      for (Crypto::Signature& sig : tx.signatures[i]) {
        serializePod(sig, "", serializer);
      }

    } else {
      std::vector<Crypto::Signature> signatures(signatureSize);
      for (Crypto::Signature& sig : signatures) {
        serializePod(sig, "", serializer);
      }

      tx.signatures[i] = std::move(signatures);
    }
  }
//  serializer.endArray();
}

void serialize(TransactionInput& in, ISerializer& serializer) {
  if (serializer.type() == ISerializer::OUTPUT) {
    BinaryVariantTagGetter tagGetter;
    uint8_t tag = boost::apply_visitor(tagGetter, in);
    serializer.binary(&tag, sizeof(tag), "type");

    VariantSerializer visitor(serializer, "value");
    boost::apply_visitor(visitor, in);
  } else {
    uint8_t tag;
    serializer.binary(&tag, sizeof(tag), "type");

    getVariantValue(serializer, tag, in);
  }
}

void serialize(BaseInput& gen, ISerializer& serializer) {
  serializer(gen.blockIndex, "height");
}

void serialize(KeyInput& key, ISerializer& serializer) {
  serializer(key.amount, "amount");
  serializeVarintVector(key.outputIndexes, serializer, "key_offsets");
  serializer(key.keyImage, "k_image");
}

void serialize(MultisignatureInput& multisignature, ISerializer& serializer) {
  serializer(multisignature.amount, "amount");
  serializer(multisignature.signatureCount, "signatures");
  serializer(multisignature.outputIndex, "outputIndex");
}

void serialize(TransactionOutput& output, ISerializer& serializer) {
  serializer(output.amount, "amount");
  serializer(output.target, "target");
}

void serialize(TransactionOutputTarget& output, ISerializer& serializer) {
  if (serializer.type() == ISerializer::OUTPUT) {
    BinaryVariantTagGetter tagGetter;
    uint8_t tag = boost::apply_visitor(tagGetter, output);
    serializer.binary(&tag, sizeof(tag), "type");

    VariantSerializer visitor(serializer, "data");
    boost::apply_visitor(visitor, output);
  } else {
    uint8_t tag;
    serializer.binary(&tag, sizeof(tag), "type");

    getVariantValue(serializer, tag, output);
  }
}

void serialize(KeyOutput& key, ISerializer& serializer) {
  serializer(key.key, "key");
}

void serialize(MultisignatureOutput& multisignature, ISerializer& serializer) {
  serializer(multisignature.keys, "keys");
  serializer(multisignature.requiredSignatureCount, "required_signatures");
}

void serialize(ParentBlockSerializer& pbs, ISerializer& serializer) {
  serializer(pbs.m_parentBlock.majorVersion, "majorVersion");

  serializer(pbs.m_parentBlock.minorVersion, "minorVersion");
  serializer(pbs.m_timestamp, "timestamp");
  serializer(pbs.m_parentBlock.previousBlockHash, "prevId");
  serializer.binary(&pbs.m_nonce, sizeof(pbs.m_nonce), "nonce");

  if (pbs.m_hashingSerialization) {
    Crypto::Hash minerTxHash;
    if (!getObjectHash(pbs.m_parentBlock.baseTransaction, minerTxHash)) {
      throw std::runtime_error("Get transaction hash error");
    }

    Crypto::Hash merkleRoot;
    Crypto::tree_hash_from_branch(pbs.m_parentBlock.baseTransactionBranch.data(), pbs.m_parentBlock.baseTransactionBranch.size(), minerTxHash, 0, merkleRoot);

    serializer(merkleRoot, "merkleRoot");
  }

  uint64_t txNum = static_cast<uint64_t>(pbs.m_parentBlock.transactionCount);
  serializer(txNum, "numberOfTransactions");
  pbs.m_parentBlock.transactionCount = static_cast<uint16_t>(txNum);
  if (pbs.m_parentBlock.transactionCount < 1) {
    throw std::runtime_error("Wrong transactions number");
  }

  if (pbs.m_headerOnly) {
    return;
  }

  size_t branchSize = Crypto::tree_depth(pbs.m_parentBlock.transactionCount);
  if (serializer.type() == ISerializer::OUTPUT) {
    if (pbs.m_parentBlock.baseTransactionBranch.size() != branchSize) {
      throw std::runtime_error("Wrong miner transaction branch size");
    }
  } else {
    pbs.m_parentBlock.baseTransactionBranch.resize(branchSize);
  }

//  serializer(m_parentBlock.baseTransactionBranch, "baseTransactionBranch");
  //TODO: Make arrays with computable size! This code won't work with json serialization!
  for (Crypto::Hash& hash: pbs.m_parentBlock.baseTransactionBranch) {
    serializer(hash, "");
  }

  serializer(pbs.m_parentBlock.baseTransaction, "minerTx");

  TransactionExtraMergeMiningTag mmTag;
  if (!getMergeMiningTagFromExtra(pbs.m_parentBlock.baseTransaction.extra, mmTag)) {
    throw std::runtime_error("Can't get extra merge mining tag");
  }

  if (mmTag.depth > 8 * sizeof(Crypto::Hash)) {
    throw std::runtime_error("Wrong merge mining tag depth");
  }

  if (serializer.type() == ISerializer::OUTPUT) {
    if (mmTag.depth != pbs.m_parentBlock.blockchainBranch.size()) {
      throw std::runtime_error("Blockchain branch size must be equal to merge mining tag depth");
    }
  } else {
    pbs.m_parentBlock.blockchainBranch.resize(mmTag.depth);
  }

//  serializer(m_parentBlock.blockchainBranch, "blockchainBranch");
  //TODO: Make arrays with computable size! This code won't work with json serialization!
  for (Crypto::Hash& hash: pbs.m_parentBlock.blockchainBranch) {
    serializer(hash, "");
  }
}

void serializeBlockHeader(BlockHeader& header, ISerializer& serializer) {
  serializer(header.majorVersion, "major_version");
  if (header.majorVersion > BLOCK_MAJOR_VERSION_4) {
    throw std::runtime_error("Wrong major version");
  }

  serializer(header.minorVersion, "minor_version");
  
  if (header.majorVersion == BLOCK_MAJOR_VERSION_2 || header.majorVersion == BLOCK_MAJOR_VERSION_3) {
    serializer(header.previousBlockHash, "prev_id");
  } else if (header.majorVersion == BLOCK_MAJOR_VERSION_1 || header.majorVersion >= BLOCK_MAJOR_VERSION_4) {
    serializer(header.timestamp, "timestamp");
    serializer(header.previousBlockHash, "prev_id");
    serializer.binary(&header.nonce, sizeof(header.nonce), "nonce");
  }
  else {
    throw std::runtime_error("Wrong major version");
  }
}

void serialize(BlockHeader& header, ISerializer& serializer) {
  serializeBlockHeader(header, serializer);
}

void serialize(Block& block, ISerializer& serializer) {
  serializeBlockHeader(block, serializer);

  if (block.majorVersion == BLOCK_MAJOR_VERSION_2 || block.majorVersion == BLOCK_MAJOR_VERSION_3) {
    auto parentBlockSerializer = makeParentBlockSerializer(block, false, false);
    serializer(parentBlockSerializer, "parent_block");
  }

  serializer(block.baseTransaction, "miner_tx");
  serializer(block.transactionHashes, "tx_hashes");
}

void serialize(AccountPublicAddress& address, ISerializer& serializer) {
  serializer(address.spendPublicKey, "m_spend_public_key");
  serializer(address.viewPublicKey, "m_view_public_key");
}

void serialize(AccountKeys& keys, ISerializer& s) {
  s(keys.address, "m_account_address");
  s(keys.spendSecretKey, "m_spend_secret_key");
  s(keys.viewSecretKey, "m_view_secret_key");
}

void doSerialize(TransactionExtraMergeMiningTag& tag, ISerializer& serializer) {
  uint64_t depth = static_cast<uint64_t>(tag.depth);
  serializer(depth, "depth");
  tag.depth = static_cast<size_t>(depth);
  serializer(tag.merkleRoot, "merkle_root");
}

void serialize(TransactionExtraMergeMiningTag& tag, ISerializer& serializer) {
  if (serializer.type() == ISerializer::OUTPUT) {
    std::string field;
    StringOutputStream os(field);
    BinaryOutputStreamSerializer output(os);
    doSerialize(tag, output);
    serializer(field, "");
  } else {
    std::string field;
    serializer(field, "");
    MemoryInputStream stream(field.data(), field.size());
    BinaryInputStreamSerializer input(stream);
    doSerialize(tag, input);
  }
}

void serialize(KeyPair& keyPair, ISerializer& serializer) {
  serializer(keyPair.secretKey, "secret_key");
  serializer(keyPair.publicKey, "public_key");
}


} //namespace DynexCN
