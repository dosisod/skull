case "$@" in
	*"["*"[0m"*) ;;
	*) exit 1;
esac

rm -f ./test/sh/skull/werror
