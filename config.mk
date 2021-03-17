BIN := /usr/local/bin/
HEADER := /usr/include
MANPATH := $(shell manpath | cut -d : -f1)/man1/

CC = $(shell readlink -f `which cc`)
CXX ?= g++

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
	-Wundef \
	-Wunused \
	-Wunused-macros \
	-Wconversion \
	-Wfloat-equal \
	-Wwrite-strings \
	-Wdouble-promotion \
	-I.

LLVM_CONFIG = $(shell command -v llvm-config-10 || command -v llvm-config)

LLVM_CFLAGS = $(shell $(LLVM_CONFIG) --cflags)

RELEASE ?= 0
ifeq ($(RELEASE), 1)
	LLVM_LDFLAGS = $(shell $(LLVM_CONFIG) --libfiles --link-static) \
		$(shell $(CXX) -print-file-name=libstdc++.a) \
		-lm -lpthread -lncurses
else
	LLVM_LDFLAGS = $(shell $(LLVM_CONFIG) --libs)
endif


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
	-Wunsafe-loop-optimizations
else
	CFLAGS += -Wno-newline-eof
endif

_OBJS := $(patsubst %.c,%.o,$(shell find skull -name "*.c" | grep -v "main\|llvm"))
_OBJS_TEST := $(patsubst %.c,%.o,$(shell find test/skull -name "*.c") test/testing.c)
_OBJS_LLVM := $(patsubst %.c,%.o,$(shell find skull/llvm -name "*.c") skull/main.c skull/setup_main.c)

ODIR := build/objs
OBJS := $(addprefix $(ODIR)/,$(_OBJS))
OBJS_TEST := $(addprefix $(ODIR)/,$(_OBJS_TEST))
OBJS_LLVM := $(addprefix $(ODIR)/,$(_OBJS_LLVM))

DIRS := build/skull \
	build/test \
	$(ODIR)/skull/eval/types \
	$(ODIR)/skull/parse/ast \
	$(ODIR)/skull/common \
	$(ODIR)/skull/llvm \
	$(ODIR)/test/skull/eval/types \
	$(ODIR)/test/skull/parse/ast \
	$(ODIR)/test/skull/common
