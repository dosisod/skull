#!/bin/sh

[ "$2" = "1" ] || exit 1;

case $1 in
	*"unrecognized optimization level \"x\""*) ;;
	*) exit 1;;
esac
