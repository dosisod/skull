#!/bin/sh

[ -f "./test/sh/skull/.dummy.sk.s" ]

case "$(file ./test/sh/skull/.dummy.sk.s)" in
	*assembler*) ;;
	*) exit 1 ;;
esac

rm ./test/sh/skull/.dummy.sk.s
