#!/bin/bash

gcc ./src/editor.c -lncurses -o ./bin/jotter

./bin/jotter inputs/shitt.txt
