#!/bin/bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" /dev/null && pwd )"

python3 $SCRIPT_DIR/ack.py
node $SCRIPT_DIR/ack.js
./nucleus.exe $SCRIPT_DIR/ack.nuc
