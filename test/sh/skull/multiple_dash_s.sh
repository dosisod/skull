#!/bin/sh

case "$@" in
	*"-S cannot be used more then once"*) ;;
	*) exit 1 ;;
esac
