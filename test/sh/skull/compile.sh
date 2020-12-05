[ -f "./test/sh/skull/dummy" ] || exit 1

case "$(file ./test/sh/skull/dummy)" in
	*ELF*shared\ object*) ;;
	*) exit 1 ;;
esac

rm ./test/sh/skull/dummy
