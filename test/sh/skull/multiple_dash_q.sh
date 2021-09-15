#!/bin/sh

case "$@" in
	*"-q cannot be used more then once"*) ;;
	*) exit 1 ;;
esac
