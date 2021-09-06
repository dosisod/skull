#!/bin/sh

case "$@" in
	*"long options not supported"*) ;;
	*) exit 1 ;;
esac
