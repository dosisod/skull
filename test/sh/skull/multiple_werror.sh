#!/bin/sh

case "$@" in
	*"--werror cannot be used more then once"*) ;;
	*) exit 1 ;;
esac
