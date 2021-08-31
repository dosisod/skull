#!/bin/sh

case "$@" in
	*"-c cannot be used more then once"*) ;;
	*) exit 1 ;;
esac
