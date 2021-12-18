#!/bin/sh

[ "$2" = "1" ]

[ "$1" = "./test/sh/skull/werror.sk: Warning: line 1 column 1: variable \"x\" is unused" ]
