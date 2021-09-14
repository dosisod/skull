#!/bin/sh

[ "$2" = "1" ]

[ "$1" = "./test/sh/skull/werror.sk: Warning: line 1 column 1: variable \"x\" is unused" ]

# TODO: fix files with no compilation errors (just warnings) returning still
# compiling .ll file
rm ./test/sh/skull/.werror.sk.ll
