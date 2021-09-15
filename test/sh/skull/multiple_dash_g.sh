#!/bin/sh

case "$@" in
	*"-g cannot be used more then once"*) ;;
	*) exit 1 ;;
esac
