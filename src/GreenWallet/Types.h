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

#include <cstddef>
#include <stdint.h>
#include "DynexCNConfig.h"
#include <Serialization/ISerializer.h>
#include <Wallet/WalletGreen.h>

struct CLICommand
{
    CLICommand() {}

    CLICommand(std::string name, std::string description,
               std::string shortName, bool hasShortName, bool hasArgument) :
               name(name), description(description), shortName(shortName),
               hasShortName(hasShortName), hasArgument(hasArgument) {}

    /* The command name */
    std::string name;

    /* The command description */
    std::string description;

    /* The command shortname, e.g. --help == -h */
    std::string shortName;

    /* Does the command have a shortname */
    bool hasShortName;

    /* Does the command take an argument, e.g. --wallet-file yourwalletname */
    bool hasArgument;
};

struct WalletInfo
{
    WalletInfo(std::string walletFileName, 
               std::string walletPass, 
               std::string walletAddress,
               bool viewWallet,
               DynexCN::WalletGreen &wallet) : 
               walletFileName(walletFileName), 
               walletPass(walletPass), 
               walletAddress(walletAddress),
               viewWallet(viewWallet),
               wallet(wallet) {}

    /* How many transactions do we know about */
    size_t knownTransactionCount = 0;

    /* The wallet file name */
    std::string walletFileName;

    /* The wallet password */
    std::string walletPass;

    /* The wallets primary address */
    std::string walletAddress;

    /* Is the wallet a view only wallet */
    bool viewWallet;

    /* The walletgreen wallet container */
    DynexCN::WalletGreen &wallet;
};

struct Config
{
    /* Should we exit after parsing arguments */
    bool exit = false;

    /* Was the wallet file specified on CLI */
    bool walletGiven = false;

    /* Was the wallet pass specified on CLI */
    bool passGiven = false;

    /* The daemon host */
    std::string host = "127.0.0.1";
    
    /* The daemon port */
    uint16_t port = DynexCN::RPC_DEFAULT_PORT;

    /* The url path */
    std::string path = "/";

    /* Enable SSL mode */
    bool ssl = false;

    /* The wallet file path */
    std::string walletFile = "";

    /* The wallet password */
    std::string walletPass = "";
};

struct AddressBookEntry
{
    AddressBookEntry() {}

    /* Used for quick comparison with strings */
    AddressBookEntry(std::string friendlyName) : friendlyName(friendlyName) {}

    AddressBookEntry(std::string friendlyName, std::string address,
                std::string paymentID) : friendlyName(friendlyName),
                                         address(address),
                                         paymentID(paymentID) {}

    /* Friendly name for this address book entry */
    std::string friendlyName;
    /* The wallet address of this entry */
    std::string address;
    /* The payment ID associated with this address */
    std::string paymentID;

    void serialize(DynexCN::ISerializer &s)
    {
        KV_MEMBER(friendlyName)
        KV_MEMBER(address)
        KV_MEMBER(paymentID)
    }

    /* Only compare via name as we don't really care about the contents */
    bool operator==(const AddressBookEntry &rhs) const
    {
        return rhs.friendlyName == friendlyName;
    }
};

/* An address book is a vector of address book entries */
typedef std::vector<AddressBookEntry> AddressBook;

/* This borrows from haskell, and is a nicer boost::optional class. We either
   have Just a value, or Nothing.

   Example usage follows.
   The below code will print:

   ```
   100
   Nothing
   ```

   Maybe<int> parseAmount(std::string input)
   {
        if (input.length() == 0)
        {
            return Nothing<int>();
        }

        try
        {
            return Just<int>(std::stoi(input)
        }
        catch (const std::invalid_argument &)
        {
            return Nothing<int>();
        }
   }

   int main()
   {
       auto foo = parseAmount("100");

       if (foo.isJust)
       {
           std::cout << foo.x << std::endl;
       }
       else
       {
           std::cout << "Nothing" << std::endl;
       }

       auto bar = parseAmount("garbage");

       if (bar.isJust)
       {
           std::cout << bar.x << std::endl;
       }
       else
       {
           std::cout << "Nothing" << std::endl;
       }
   }

*/

template <class X> struct Maybe
{
    X x;
    bool isJust;

    Maybe(const X &x) : x (x), isJust(true) {}
    Maybe() : isJust(false) {}
};

template <class X> Maybe<X> Just(const X&x)
{
    return Maybe<X>(x);
}

template <class X> Maybe<X> Nothing()
{
    return Maybe<X>();
}
