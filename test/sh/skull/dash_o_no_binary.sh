#!/bin/sh

case "$@" in
	*"cannot print binary file to stdout"*) ;;
	*) exit 1 ;;
esac

rm test/sh/skull/.dummy.sk.ll
