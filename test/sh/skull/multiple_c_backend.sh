#!/bin/sh

case "$@" in
	*"--c-backend cannot be used more then once"*) ;;
	*) exit 1 ;;
esac
