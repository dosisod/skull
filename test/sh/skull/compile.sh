#!/bin/sh

[ -f "./test/sh/skull/dummy" ]

case "$(file ./test/sh/skull/dummy)" in
	*ELF*shared\ object*) ;;
	*) exit 1 ;;
esac

rm ./test/sh/skull/dummy
