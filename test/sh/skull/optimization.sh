#!/bin/sh

expected=$(cat ./test/sh/skull/o1.sk.ll)

[ "$1" = "$expected" ] || exit 1;
