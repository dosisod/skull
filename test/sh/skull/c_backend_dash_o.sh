#!/bin/sh

case "$@" in
	*"int main"*) ;;
	*) exit 1 ;;
esac
