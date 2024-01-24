#!/bin/bash

gcc ./src/editor.c -lncurses -o ./bin/editor.cool

./bin/editor.cool inputs/special.txt
