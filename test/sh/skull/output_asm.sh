[ -f "./test/sh/skull/alt_name" ]

case "$(file ./test/sh/skull/alt_name)" in
	*assembler*) ;;
	*) exit 1 ;;
esac

rm ./test/sh/skull/alt_name
