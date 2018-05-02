#!/bin/bash

make clean
make
make clean -C ./sample-client-server-ex
make -C ./sample-client-server-ex
x-terminal-emulator -e ./proxy
x-terminal-emulator -e ./sample-client-server-ex/client