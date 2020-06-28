SKULL_VERSION = 0.1.0

CC = $(shell readlink -f $(shell which cc))
CFLAGS = -std=c11 \
	-Wall \
	-Wextra \
	-pedantic \
	-Werror \
	-Waggregate-return \
	-Wbad-function-cast \
	-Wbuiltin-macro-redefined \
	-Wcast-align \
	-Wdate-time \
	-Wdisabled-optimization \
	-Wformat=2 \
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
	-Wdouble-promotion \
	-DSKULL_VERSION="\"$(SKULL_VERSION)\""

#dont add gnu specific flags if compiling in clang
ifeq ($(findstring clang,$(CC)),)
	CFLAGS += -Waggressive-loop-optimizations \
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

ODIR = build/objs

_OBJS = src/eval/context.o \
	src/eval/repl.o \
	src/errors.o \
	src/common/str.o \
	src/parse/tokenize.o \
	src/eval/file.o \
	src/eval/variable.o \
	src/eval/eval_assign.o \
	src/parse/classify.o \
	src/parse/ast/node.o \
	src/eval/eval_add.o \
	src/eval/eval_integer.o \
	src/eval/eval_float.o \
	src/common/wegex.o \
	src/parse/types.o \
	src/parse/constants.o \
	src/eval/eval_bool.o

OBJS = $(addprefix $(ODIR)/,$(_OBJS))

DIRS = build/src \
	build/test \
	$(ODIR)/src/eval \
	$(ODIR)/src/parse/ast \
	$(ODIR)/src/common

setup:
	@mkdir $(DIRS) -p

src: setup | $(OBJS)
	@echo "\033[92mCompile\033[0m skull"
	@$(CC) src/main.c $(OBJS) -o build/src/skull $(CFLAGS)

test: setup | src
	@echo "\033[92mCompile\033[0m tests"
	@$(CC) test/main.c $(OBJS) -o build/test/test $(CFLAGS)

$(ODIR)/%.o: %.c
	@echo "\033[92mCompile\033[0m $<"
	@$(CC) $< -c -o $@ $(CFLAGS)

docs:
	@echo "\033[92mCompile\033[0m docs"
	@python3 make_docs.py

clean:
	rm -rf build/*