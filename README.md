# Dynex GUI Wallet

## Release Notes Version 2.2.2.(rev.B)

- added mnemonic phrase import, export, new wallet creation;
- added private keys export and import;
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

## Build Dynex Wallet from source:

### Requirements

make sure that you have boost, Qt5.15 and libcurl installed on your operating system. 

```
sudo apt-get install libboost-all-dev 
sudo apt-get install libcurl-dev
sudo apt install qtcreator qtbase5-dev qt5-qmake cmake
```

### Build

```
mkdir build && cd build && cmake .. && make
```

The wallet can be started with

```
cd /build/src/WalletGui/
./dynexwallet
```
