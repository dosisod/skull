#!/bin/sh

[ ! -f "./test/sh/skull/.dummy.sk.s" ]

output_type="$(echo "$1" | file -)"

case "$output_type" in
	*assembler*) ;;
	*) exit 1 ;;
esac
