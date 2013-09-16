# Overview

LLVM-based demos, samples and libraries for ZeroVM.

# Requisitions

## Environment

Make sure you have ZVM_PREFIX set.

    export ZVM_PREFIX=/opt/zerovm

## 3rd-party Libraries

1. *Google test*

        git clone https://github.com/zerovm/zerovm
        cd lib/gtest
        cp libgtest.a $ZVM_PREFIX/x86_64-nacl/lib

2. *BFD* (Binary File Descriptor)

        git clone https://github.com/zerovm/binutils
        cd binutils
        cd bfd
        ./configure --host=x86_64-nacl --disable-werror --prefix=$ZVM_PREFIX --exec-prefix=$ZVM_PREFIX/x86_64-nacl
        make
        cp libbfd.a $ZVM_PREFIX/x86_64-nacl/lib/
        cp bfd.h ../include/bfdlink.h $ZVM_PREFIX/x86_64-nacl/include/ 

3. *LLVM + Clang*

        git clone https://github.com/zerovm/llvm-port llvm
        cd llvm
        cd tools
        git clone https://github.com/zerovm/clang-port clang
        cd ..
        ./build.sh
        cd build
        make install

# libjit

    cd libjit
    make
    make install

# libzdl

    cd libzdl
    make
    make install

# MapReduce samples

Make sure _BFD_, _LLVM_, _libjit_, _libzdl_ are installed and could be found
in $ZVM\_PREFIX/x86\_64-nacl dir.

## terasort

*Known issue*: works well when SINGLE_NODE_INPUT_RECORDS_COUNT<1000000, otherwise map node could crash somewhere.

    cd terasort
    make
    ./start.sh

## wordcount

    cd wordcount
    make
    ./mr_start.sh




