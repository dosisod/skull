#!/bin/sh

case "$@" in
	*Skull*v*.*.*) ;;
	*Skull*[0-9a-f][0-9a-f][0-9a-f][0-9a-f]*) ;;
	*) exit 1 ;;
esac
