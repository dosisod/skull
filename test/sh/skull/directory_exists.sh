#!/bin/sh

case "$@" in
	*"directory not a file"*) ;;
	*) exit 1 ;;
esac
