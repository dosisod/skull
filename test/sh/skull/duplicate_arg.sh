#!/bin/sh

option="$1"
shift

case "$@" in
	*"$option cannot be used more then once"*) ;;
	*) exit 1 ;;
esac
