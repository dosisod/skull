[ -f "./test/sh/skull/alt_name" ] || exit 1

case "$(file ./test/sh/skull/alt_name)" in
	*assembler*) ;;
	*) exit 1 ;;
esac

rm ./test/sh/skull/alt_name
