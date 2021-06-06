#!/bin/bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" /dev/null && pwd )"

python3 $SCRIPT_DIR/vec.py
node $SCRIPT_DIR/vec.js
./nucleus.exe $SCRIPT_DIR/vec.nuc