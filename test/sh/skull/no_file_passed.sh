#!/bin/sh

case "$@" in
	*"expected filename"*) ;;
	*) exit 1 ;;
esac
