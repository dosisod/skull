SKULL_VERSION := 0.1.1

INSTALL := /usr/local

CC := $(shell which cc)
CFLAGS = -std=c18 \
	-Wall \
	-Wextra \
	-pedantic \
	-Werror \
	-Werror=vla \
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
	-DSKULL_VERSION="\"$(SKULL_VERSION)\"" \
	-I.

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

ODIR = build/objs

_OBJS = skull/eval/context.o \
	skull/eval/repl.o \
	skull/errors.o \
	skull/common/str.o \
	skull/parse/tokenize.o \
	skull/eval/file.o \
	skull/eval/variable.o \
	skull/eval/eval_assign.o \
	skull/parse/classify.o \
	skull/parse/ast/node.o \
	skull/eval/eval_add.o \
	skull/eval/eval_integer.o \
	skull/eval/eval_float.o \
	skull/eval/eval_sub.o \
	skull/eval/eval_mult.o \
	skull/eval/eval_div.o \
	skull/eval/function.o \
	skull/common/wegex.o \
	skull/eval/types/types.o \
	skull/parse/constants.o \
	skull/eval/types/bool.o \
	skull/eval/types/char.o \
	skull/eval/types/str.o \
	skull/eval/types/type.o \
	skull/eval/types/float.o \
	skull/eval/types/int.o

OBJS := $(addprefix $(ODIR)/,$(_OBJS))

DIRS := build/skull \
	build/test \
	$(ODIR)/skull/eval/types \
	$(ODIR)/skull/parse/ast \
	$(ODIR)/skull/common