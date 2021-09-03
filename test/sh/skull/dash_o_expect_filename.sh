#!/bin/sh

case "$@" in
	*"expected filename after -o"*) ;;
	*) exit 1 ;;
esac
