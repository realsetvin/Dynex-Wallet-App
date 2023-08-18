# Dynex GUI Wallet

Send, receive, and store your Dynex coins in an instant. 

The Dynex GUI Wallet Application is a Bitcoin-based QT solution for the management of user funds on the Dynex platform. This open-source wallet software makes it easy for anyone to quickly create and manage their connection to the Dynex blockchain. 


## Release Notes Version 2.2.2(rev.B)

- added mnemonic phrase import, export, new wallet creation;
- added private keys for export and import;
- added connection to remote node;
- added connection configuration menu with ip binding and ports selection;
- added wallet rescan option;
- added transaction fee option;
- added payment id support to address book;
- added address book edit options;
- added address book labels to wallets in transactions;
- added hash column to transactions;
- added payment id to transactions info;
- added right-click menus to address book and transactions;
- added autobackup;
- added open recent wallets option;
- added open log option;
- added node info msgs on splash screen (builtin node mode)
- improved startup time;
- improved help and about windows;
- updated copyright info;

![Dynex Wallet](https://github.com/dynexcoin/Dynex-Wallet-App/raw/main/dynexwallet.png)

With the Dynex Wallet App users can use the Dynex wallet functionality to create wallets, send and receive DNX and manage transactions and recipients. It is not necessary to run a separate Dynex node, everything is built into the GUI wallet.


## Security

It is important for users of the Dynex blockchain to secure their own funds. Central services, like exchanges, are not reliable enough to manage or store large quantities of valuable cryptocurrencies. When it comes to your own funds, you should always maintain responsible ownership by making sure to choose self-custody. 

This includes writing down your seed phrase and making sure it is backed up in a safe location. You should always store your seed phrase offline, never in the form of a screenshot or text document stored on a phone or computer. Always send a test transaction first before sending large transactions.

Be aware of friendly strangers offering technical support or friendship asking you to join communities, websites or share your personal information. Never reveal or share your seed words with anyone. 


## Build Dynex Wallet from source (Linux):

### Requirements

make sure that you have cmake, boost, Qt5.15 and libcurl installed on your operating system. 

```
sudo apt-get install libboost-all-dev &&
sudo apt-get install libcurl-dev &&
sudo apt install qtcreator qtbase5-dev qt5-qmake cmake
```

### Build


```
mkdir build &&
cd build &&
cmake .. &&
make
```

The wallet can be started with

```
cd /build/src/WalletGui/
./dynexwallet
```
