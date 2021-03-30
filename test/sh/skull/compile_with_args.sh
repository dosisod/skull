#!/bin/sh

[ -f "./test/sh/skull/alt_name" ]

case "$(file ./test/sh/skull/alt_name)" in
	*ELF*shared\ object*) ;;
	*) exit 1 ;;
esac

rm ./test/sh/skull/alt_name
