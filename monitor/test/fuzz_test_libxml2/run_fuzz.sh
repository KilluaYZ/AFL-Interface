#!/bin/bash
export AFLGO=~/CLionProjects/AFL-Interface/afl-2.57b
$AFLGO/afl-fuzz -m none -z exp -c 45m -i in -o out ./xmllint-fast --valid --recover @@
