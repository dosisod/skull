#!/bin/sh

[ "$2" = "1" ] || exit 1;

case $1 in
	*"skull: --color overrides COLOR environment variable"*) ;;
	*) exit 1;;
esac
