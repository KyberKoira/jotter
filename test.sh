#!/bin/bash

gcc ./src/testi2.c -lncurses -o ./bin/haloo.cool

./bin/haloo.cool inputs/special.txt
