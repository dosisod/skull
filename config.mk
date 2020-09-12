SKULL_VERSION := 0.2.1

BIN := /usr/local/bin/
MANPATH := $(shell manpath | cut -d : -f1)/man1/

CC := $(shell readlink -f `which cc`)

DEBUG ?= 0
ifeq ($(DEBUG), 1)
	CFLAGS = -g3 -O0
endif

CFLAGS += -std=c18 \
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

LLVM_CFLAGS = $(shell llvm-config-9 --cflags)
LLVM_LDFLAGS = $(shell llvm-config-9 --libs)

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

_OBJS := $(patsubst %.c,%.o,$(shell find skull -name "*.c" | grep -v "main\|llvm"))
_OBJS_LLVM := $(patsubst %.c,%.o,$(shell find skull/llvm -name "*.c") skull/main.c)

ODIR := build/objs
OBJS := $(addprefix $(ODIR)/,$(_OBJS))
OBJS_LLVM := $(addprefix $(ODIR)/,$(_OBJS_LLVM))

DIRS := build/skull \
	build/test \
	$(ODIR)/skull/eval/types \
	$(ODIR)/skull/parse/ast \
	$(ODIR)/skull/common \
	$(ODIR)/skull/llvm \
	$(ODIR)/test/skull