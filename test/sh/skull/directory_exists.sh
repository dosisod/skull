#!/bin/sh

case "$@" in
	*"directory not a file"*) ;;
	*) exit 1 ;;
esac

rm -f test/sh/skull/.dir.sk.o
