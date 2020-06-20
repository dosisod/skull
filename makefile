SKULL_VERSION=0.0.1

CC = $(shell readlink -f $(shell which cc)) -std=c11
CFLAGS = -Wall \
	-Wextra \
	-pedantic \
	-Werror \
	-Waggregate-return \
	-Wbad-function-cast \
	-Wbuiltin-macro-redefined \
	-Wcast-align \
	-Wdate-time \
	-Wdisabled-optimization \
	-Wformat-nonliteral \
	-Wformat-security \
	-Wformat-y2k \
	-Winvalid-pch \
	-Wmissing-include-dirs \
	-Wnested-externs \
	-Wpacked \
	-Wredundant-decls \
	-Wshadow \
	-Wswitch-default \
	-Wswitch-enum \
	-Wundef \
	-Wunused \
	-Wunused-macros \
	-Wconversion \
	-Wfloat-equal \
	-Wwrite-strings \
	-DSKULL_VERSION="\"$(SKULL_VERSION)\""

#dont add gnu specific flags if compiling in clang
ifeq ($(findstring clang,$(CC)),)
	CFLAGS += -Waggressive-loop-optimizations \
	-Wc99-c11-compat \
	-Wformat-signedness \
	-Wlogical-op \
	-Wsuggest-attribute=const \
	-Wsuggest-attribute=format \
	-Wsuggest-attribute=noreturn \
	-Wsuggest-attribute=pure \
	-Wsync-nand \
	-Wtrampolines \
	-Wunsafe-loop-optimizations \
	-Wchkp
else
	CFLAGS += -Wno-newline-eof
endif

all:
	make src
	make test
	make docs

.PHONY: src test docs

src:
	mkdir build/src -p
	$(CC) src/main.c -o build/src/skull -ldl $(CFLAGS)

test:
	mkdir build/test -p
	$(CC) test/main.c -o build/test/test $(CFLAGS)

docs:
	python3 make_docs.py

clean:
	rm -rf build/