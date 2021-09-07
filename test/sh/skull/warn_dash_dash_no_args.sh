#!/bin/sh

case "$@" in
	*"expected arguments after --"*) ;;
	*) exit 1 ;;
esac
