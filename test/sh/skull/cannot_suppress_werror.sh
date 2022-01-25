#!/bin/sh

[ "$2" = "1" ] || exit 1;

case $1 in
	*"warnings aren't suppressed with --werror, -q is ignored"*) ;;
	*) exit 1;;
esac
