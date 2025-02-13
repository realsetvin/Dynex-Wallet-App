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


/////////////////////////////
#include <GreenWallet/Fusion.h>
/////////////////////////////

#include "DynexCNConfig.h"

#include <iostream>
#include <thread>

#include <Wallet/WalletGreen.h>

#include <GreenWallet/ColouredMsg.h>
#include <GreenWallet/Tools.h>
#include <GreenWallet/WalletConfig.h>

size_t makeFusionTransaction(DynexCN::WalletGreen &wallet, 
                             uint64_t threshold)
{
    uint64_t bestThreshold = threshold;
    size_t optimizable = 0;

    /* Find the best threshold by starting at threshold and decreasing by
       half till we get to the minimum amount, storing the threshold that
       gave us the most amount of optimizable amounts */
    while (threshold > WalletConfig::minimumSend)
    {
        const auto fusionReadyCount
            = wallet.estimate(threshold).fusionReadyCount;

        if (fusionReadyCount > optimizable)
        {
            optimizable = fusionReadyCount;
            bestThreshold = threshold;
        }

        threshold /= 2;
    }

    /* Can't optimize */
    if (optimizable == 0)
    {
        return DynexCN::WALLET_INVALID_TRANSACTION_ID;
    }

    try
    {
        return wallet.createFusionTransaction(bestThreshold, 
                                              WalletConfig::defaultMixin);
    }
    catch (const std::runtime_error &e)
    {
        std::cout << WarningMsg("Failed to send fusion transaction: ")
                  << WarningMsg(e.what()) << std::endl;

        return DynexCN::WALLET_INVALID_TRANSACTION_ID;
    }
}

void fullOptimize(DynexCN::WalletGreen &wallet)
{
    std::cout << "Attempting to optimize your wallet to allow you to "
              << "send large amounts at once. " << std::endl
              << WarningMsg("This may take a very long time!") << std::endl;

    if (!confirm("Do you want to proceed?"))
    {
        std::cout << WarningMsg("Cancelling optimization.") << std::endl;
        return;
    }

    for (int i = 1;;i++)
    {
        std::cout << InformationMsg("Running optimization round "
                                  + std::to_string(i) + "...")
                  << std::endl;

        /* Optimize as many times as possible until optimization is no longer
           possible. */
        if (!optimize(wallet, wallet.getActualBalance()))
        {
            break;
        }
    }

    std::cout << SuccessMsg("Full optimization completed!") << std::endl;
}

bool optimize(DynexCN::WalletGreen &wallet, uint64_t threshold)
{
    std::vector<Crypto::Hash> fusionTransactionHashes;

    while (true)
    {
        /* Create as many fusion transactions until we can't send anymore,
           either because balance is locked too much or we can no longer
           optimize anymore transactions */
        const size_t tmpFusionTxID = makeFusionTransaction(wallet, threshold);

        if (tmpFusionTxID == DynexCN::WALLET_INVALID_TRANSACTION_ID)
        {
            break;
        }
        else
        {
            const DynexCN::WalletTransaction w
                = wallet.getTransaction(tmpFusionTxID);

            fusionTransactionHashes.push_back(w.hash);

            if (fusionTransactionHashes.size() == 1)
            {
                std::cout << SuccessMsg("Created 1 fusion transaction!")
                          << std::endl;
            }
            else
            {
                std::cout << SuccessMsg("Created " 
                            + std::to_string(fusionTransactionHashes.size())
                                    + " fusion transactions!") << std::endl;
            }
        }
    }

    if (fusionTransactionHashes.empty())
    {
        return false;
    }

    /* Hurr durr grammar */
    if (fusionTransactionHashes.size() == 1)
    {
        std::cout << SuccessMsg("1 fusion transaction has been sent, waiting "
                                "for balance to return and unlock")
                  << std::endl << std::endl;
    }
    else
    {
        std::cout << SuccessMsg(std::to_string(fusionTransactionHashes.size()) +
                                " fusion transactions have been sent, waiting "
                                "for balance to return and unlock")
              << std::endl << std::endl;
    }

    wallet.updateInternalCache();

    /* Short sleep to ensure it's in the transaction pool when we poll it */
    std::this_thread::sleep_for(std::chrono::seconds(1));

    while (true)
    {
        const std::vector<DynexCN::WalletTransactionWithTransfers> 
            unconfirmedTransactions = wallet.getUnconfirmedTransactions();

        std::vector<Crypto::Hash> unconfirmedTxHashes;

        for (const auto &t : unconfirmedTransactions)
        {
            unconfirmedTxHashes.push_back(t.transaction.hash);
        }

        bool fusionCompleted = true;

        /* Is our fusion transaction still unconfirmed? We can't gain the
           benefits of fusioning if the balance hasn't unlocked, so we can
           send this new optimized balance */
        for (const auto &tx : fusionTransactionHashes)
        {
            /* If the fusion transaction hash is present in the unconfirmed
               transactions pool, we need to wait for it to complete. */
            if (std::find(unconfirmedTxHashes.begin(),
                          unconfirmedTxHashes.end(), tx) 
                       != unconfirmedTxHashes.end())
            {
                fusionCompleted = false; 
            }
            else
            {
                /* We can't find this transaction in the unconfirmed
                   transaction pool anymore, so it has been confirmed. Remove
                   it so we both have to check less transactions each time,
                   and we can easily update the transactions left to confirm
                   output message */
                fusionTransactionHashes.erase(std::remove
                    (fusionTransactionHashes.begin(),
                     fusionTransactionHashes.end(), tx), 
                     fusionTransactionHashes.end());
            }
        }

        if (!fusionCompleted)
        {
            std::cout << WarningMsg("Balance is still locked, "
                  + std::to_string(fusionTransactionHashes.size()));

            /* More grammar... */
            if (fusionTransactionHashes.size() == 1)
            {
                std::cout << WarningMsg(" fusion transaction still to be "
                                        "confirmed.");
            }
            else
            {
                std::cout << WarningMsg(" fusion transactions still to be "
                                        "confirmed.");
            }
            
            std::cout << std::endl
                      << SuccessMsg("Will try again in 1 minute...")
                      << std::endl;

            std::this_thread::sleep_for(std::chrono::minutes(1));

            wallet.updateInternalCache();
        }
        else
        {
            std::cout << SuccessMsg("All fusion transactions confirmed!")
                      << std::endl;
            break;
        }
    }

    return true;
}

bool fusionTX(DynexCN::WalletGreen &wallet, 
              DynexCN::TransactionParameters p)
{
    std::cout << WarningMsg("Your transaction is too large to be accepted by "
                            "the network!")
              << std::endl << "We're attempting to optimize your "
              << "wallet, which hopefully will make the transaction small "
              << "enough to fit in a block." << std::endl 
              << "Please wait, this will take some time..." << std::endl 
              << std::endl;

    /* We could check if optimization succeeded, but it's not really needed
       because we then check if the transaction is too large... it could have
       potentially become valid because another payment came in. */
    optimize(wallet, p.destinations[0].amount + p.fee);

    const auto startTime = std::chrono::system_clock::now();

    while (wallet.getActualBalance() < p.destinations[0].amount + p.fee)
    {
        /* Break after a minute just in case something has gone wrong */
        if ((std::chrono::system_clock::now() - startTime) > 
             std::chrono::minutes(5))
        {
            std::cout << WarningMsg("Fusion transactions have "
                                    "completed, however available "
                                    "balance is less than transfer "
                                    "amount specified.") << std::endl
                      << WarningMsg("Transfer aborted, please review "
                                    "and start a new transfer.")
                      << std::endl;

            return false;
        }

        std::cout << WarningMsg("Optimization completed, but balance "
                                "is not fully unlocked yet!")
                  << std::endl
                  << SuccessMsg("Will try again in 5 seconds...")
                  << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    return true;
}
