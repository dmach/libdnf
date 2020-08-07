#!/bin/sh

set -e

#export CC=clang
#export CXX=clang++

export CC=gcc
export CXX=g++

rm -rf build
mkdir build || :
pushd build

# configure
BINDINGS_OFF="-DWITH_GO=OFF -DWITH_PYTHON3=ON -DWITH_PERL5=OFF -DWITH_RUBY=OFF"
SANITIZERS="-DWITH_SANITIZERS=ON"
COMPONENTS="-DWITH_DNFDAEMON_SERVER=OFF -DWITH_DNFDAEMON_CLIENT=OFF -DWITH_MICRODNF=ON"
cmake .. $BINDINGS_OFF $SANITIZERS $COMPONENTS

# compile
make -j4

./microdnf/microdnf

