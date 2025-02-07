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

#include <string>
#include <system_error>

namespace DynexCN {
namespace error {

// custom error conditions enum type:
enum WalletErrorCodes {
  NOT_INITIALIZED = 1,
  ALREADY_INITIALIZED,
  WRONG_STATE,
  WRONG_PASSWORD,
  INTERNAL_WALLET_ERROR,
  MIXIN_COUNT_TOO_BIG,
  BAD_ADDRESS,
  TRANSACTION_SIZE_TOO_BIG,
  WRONG_AMOUNT,
  SUM_OVERFLOW,
  ZERO_DESTINATION,
  TX_CANCEL_IMPOSSIBLE,
  TX_CANCELLED,
  OPERATION_CANCELLED,
  TX_TRANSFER_IMPOSSIBLE,
  WRONG_VERSION,
  FEE_TOO_SMALL,
  KEY_GENERATION_ERROR,
  INDEX_OUT_OF_RANGE,
  ADDRESS_ALREADY_EXISTS,
  TRACKING_MODE,
  WRONG_PARAMETERS,
  OBJECT_NOT_FOUND,
  WALLET_NOT_FOUND,
  CHANGE_ADDRESS_REQUIRED,
  CHANGE_ADDRESS_NOT_FOUND,
  DESTINATION_ADDRESS_REQUIRED,
  DESTINATION_ADDRESS_NOT_FOUND,
  BAD_PAYMENT_ID,
  BAD_TRANSACTION_EXTRA,
  MIXIN_COUNT_TOO_SMALL,
  MIXIN_COUNT_TOO_LARGE,
  WRONG_TX_SECRET_KEY
};

// custom category:
class WalletErrorCategory : public std::error_category {
public:
  static WalletErrorCategory INSTANCE;

  virtual const char* name() const throw() override {
    return "WalletErrorCategory";
  }

  virtual std::error_condition default_error_condition(int ev) const throw() override {
    return std::error_condition(ev, *this);
  }

  virtual std::string message(int ev) const override {
    switch (ev) {
    case NOT_INITIALIZED:               return "Object was not initialized";
    case WRONG_PASSWORD:                return "The password is wrong";
    case ALREADY_INITIALIZED:           return "The object is already initialized";
    case INTERNAL_WALLET_ERROR:         return "Internal error occurred";
    case MIXIN_COUNT_TOO_BIG:           return "MixIn count is too big";
    case BAD_ADDRESS:                   return "Bad address";
    case TRANSACTION_SIZE_TOO_BIG:      return "Transaction size is too big";
    case WRONG_AMOUNT:                  return "Wrong amount";
    case SUM_OVERFLOW:                  return "Sum overflow";
    case ZERO_DESTINATION:              return "The destination is empty";
    case TX_CANCEL_IMPOSSIBLE:          return "Impossible to cancel transaction";
    case WRONG_STATE:                   return "The wallet is in wrong state (maybe loading or saving), try again later";
    case OPERATION_CANCELLED:           return "The operation you've requested has been cancelled";
    case TX_TRANSFER_IMPOSSIBLE:        return "Transaction transfer impossible";
    case WRONG_VERSION:                 return "Wrong version";
    case FEE_TOO_SMALL:                 return "Transaction fee is too small";
    case KEY_GENERATION_ERROR:          return "Cannot generate new key";
    case INDEX_OUT_OF_RANGE:            return "Index is out of range";
    case ADDRESS_ALREADY_EXISTS:        return "Address already exists";
    case TRACKING_MODE:                 return "The wallet is in tracking mode";
    case WRONG_PARAMETERS:              return "Wrong parameters passed";
    case OBJECT_NOT_FOUND:              return "Object not found";
    case WALLET_NOT_FOUND:              return "Requested wallet not found";
    case CHANGE_ADDRESS_REQUIRED:       return "Change address required";
    case CHANGE_ADDRESS_NOT_FOUND:      return "Change address not found";
    case DESTINATION_ADDRESS_REQUIRED:  return  "Destination address required";
    case DESTINATION_ADDRESS_NOT_FOUND: return "Destination address not found";
    case BAD_PAYMENT_ID:                return "Wrong payment id format";
    case BAD_TRANSACTION_EXTRA:         return "Wrong transaction extra format";
    case MIXIN_COUNT_TOO_SMALL:         return "MixIn count is below the required minimum";
    case MIXIN_COUNT_TOO_LARGE:         return "MixIn count is over the maximum allowed";
    case WRONG_TX_SECRET_KEY:           return "Wrong transaction secret key";
    default:                            return "Unknown error";
    }
  }

private:
  WalletErrorCategory() {
  }
};

}
}

inline std::error_code make_error_code(DynexCN::error::WalletErrorCodes e) {
  return std::error_code(static_cast<int>(e), DynexCN::error::WalletErrorCategory::INSTANCE);
}
