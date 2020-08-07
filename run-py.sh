#!/bin/sh

export PYTHONPATH="$(pwd)/build/bindings/python3/"
export LD_LIBRARY_PATH="$(pwd)/build/libdnf/:$(pwd)/build/libdnf-cli/:"
./run-py.py
