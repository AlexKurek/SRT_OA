#!/bin/bash

#Warning_FLAGS="-Wall -Wextra -Wpedantic -Wformat=2 -Wno-unused-parameter -Wshadow -Wwrite-strings -Wstrict-prototypes -Wold-style-definition -Wredundant-decls -Wnested-externs -Wmissing-include-dirs -Wjump-misses-init -Wlogical-op"
Warning_FLAGS="-Wall -Wextra"

gcc -O3 -m64 -march=native ${Warning_FLAGS} pswriter.c -lm
# gcc -O3 -m32 -march=native ${Warning_FLAGS} pswriter.c -lm # to było w oryginale

if [ -e a.out ]
then
    cp a.out pswriter
    rm a.out
else
    echo "Something went wrong, 'a.out' not produced"
fi