#!/bin/sh

[ "$2" = "0" ] || exit 1;

case $1 in
	*"Warning: file is empty"*) ;;
	*) exit 1;;
esac
