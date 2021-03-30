#!/bin/sh

[ -f "./test/sh/skull/.dummy.sk.o" ]

case "$(file ./test/sh/skull/.dummy.sk.o)" in
	*ELF*relocatable*) ;;
	*) exit 1 ;;
esac

rm ./test/sh/skull/.dummy.sk.o
