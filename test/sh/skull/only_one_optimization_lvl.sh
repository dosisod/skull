#!/bin/sh

[ "$2" = "1" ] || exit 1;

case $1 in
	*"cannot specify multiple optimization levels"*) ;;
	*) exit 1;;
esac
