#!/bin/sh

[ "$2" = "1" ] || exit 1;

case $1 in
	*"cannot output assembly (-S) using C backend"*) ;;
	*) exit 1;;
esac
