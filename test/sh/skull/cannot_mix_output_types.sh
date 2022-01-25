#!/bin/sh

[ "$2" = "1" ] || exit 1;

case $1 in
	*"-S, -E, and -c cannot be used together"*) ;;
	*) exit 1;;
esac
