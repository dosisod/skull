SKULL_VERSION := 0.1.1

INSTALL := /usr/local

CC := cc
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

ODIR = build/objs

_OBJS = $(patsubst %.c,%.o,$(shell find skull* -name "*.c" | grep -v "main"))

OBJS := $(addprefix $(ODIR)/,$(_OBJS))

DIRS := build/skull \
	build/skullc \
	build/test \
	$(ODIR)/skull/eval/types \
	$(ODIR)/skull/parse/ast \
	$(ODIR)/skull/common \
	$(ODIR)/skullc \
	$(ODIR)/test