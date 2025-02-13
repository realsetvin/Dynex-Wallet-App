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


#pragma once

#include <array>
#include <string>
#include <vector>

#include "CryptoTypes.h"
#include "DynexCN.h"
#include "BlockchainExplorerData.h"
#include <boost/variant.hpp>

namespace DynexCN {

struct transaction_output_details {
	TransactionOutput output;
	uint64_t globalIndex;
};

struct BaseInputDetails {
  BaseInput input;
  uint64_t amount;
};

struct KeyInputDetails {
  KeyInput input;
  uint64_t mixin;
  std::vector<TransactionOutputReferenceDetails> outputs;
};

struct MultisignatureInputDetails {
  MultisignatureInput input;
  TransactionOutputReferenceDetails output;
};

typedef boost::variant<BaseInputDetails, KeyInputDetails, MultisignatureInputDetails> transaction_input_details;

struct TransactionExtraDetails2 {
  Crypto::PublicKey publicKey;
  BinaryArray nonce;
  BinaryArray raw;
};

struct TransactionDetails2 {
  Crypto::Hash hash;
  uint64_t size = 0;
  uint64_t fee = 0;
  uint64_t totalInputsAmount = 0;
  uint64_t totalOutputsAmount = 0;
  uint64_t mixin = 0;
  uint64_t unlockTime = 0;
  uint64_t timestamp = 0;
  Crypto::Hash paymentId;
  bool hasPaymentId = false;
  bool inBlockchain = false;
  Crypto::Hash blockHash;
  uint32_t blockHeight = 0;
  TransactionExtraDetails2 extra;
  std::vector<std::vector<Crypto::Signature>> signatures;
  std::vector<transaction_input_details> inputs;
  std::vector<transaction_output_details> outputs;
};

struct BlockDetails2 {
  uint8_t majorVersion = 0;
  uint8_t minorVersion = 0;
  uint64_t timestamp = 0;
  Crypto::Hash prevBlockHash;
  uint32_t nonce = 0;
  bool isOrphaned = false;
  uint32_t height = 0;
  Crypto::Hash hash;
  uint64_t difficulty = 0;
  uint64_t reward = 0;
  uint64_t baseReward = 0;
  uint64_t blockSize = 0;
  uint64_t transactionsCumulativeSize = 0;
  uint64_t alreadyGeneratedCoins = 0;
  uint64_t alreadyGeneratedTransactions = 0;
  uint64_t sizeMedian = 0;
  double penalty = 0.0;
  uint64_t totalFeeAmount = 0;
  std::vector<TransactionDetails2> transactions;
};

}
