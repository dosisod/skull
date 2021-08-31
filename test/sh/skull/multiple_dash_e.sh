#!/bin/sh

case "$@" in
	*"-E cannot be used more then once"*) ;;
	*) exit 1 ;;
esac
