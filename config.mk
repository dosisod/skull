BIN := /usr/local/bin
LIB := /usr/lib/
HEADER := /usr/include
MANPATH := $(shell manpath | cut -d : -f1)/man1/

CC := $(shell readlink -f `which cc`)
CXX ?= g++

SKULL_VERSION := $(shell git describe --always)

ECHO=printf

DEBUG ?= 0
ifeq ($(DEBUG), 1)
	CFLAGS += -g3 -O0
endif

TRACE ?= 0
ifeq ($(TRACE), 1)
	CFLAGS += -fsanitize=address -fsanitize=leak -fsanitize=undefined
endif

CFLAGS += -std=c18 \
	-Wno-cpp \
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
	-fno-common \
	-Wnull-dereference \
	-Wstrict-overflow=5 \
	-Wstrict-prototypes \
	-Wcast-qual \
	-Wstack-protector \
	-I. \
	-DSKULL_VERSION="\"$(SKULL_VERSION)\"" \
	-fPIC

LLVM_CONFIG := $(shell command -v llvm-config-13 || command -v llvm-config)
LLVM_CFLAGS := $(shell $(LLVM_CONFIG) --cflags)

RELEASE ?= 0
ifeq ($(RELEASE), 1)
	LLVM_LDFLAGS := $(shell $(LLVM_CONFIG) --libfiles --link-static) \
		$(shell $(CXX) -print-file-name=libstdc++.a) \
		-lm -lpthread -lncurses -lz
else
	LLVM_LDFLAGS := $(shell $(LLVM_CONFIG) --libs)
endif


#dont add gnu specific flags if compiling in clang
ifeq ($(findstring clang,$(CC)),)
	CFLAGS += -Waggressive-loop-optimizations \
	-Wformat-signedness \
	-Wformat-overflow=2 \
	-Wformat-truncation=2 \
	-Wstringop-overflow=4 \
	-Wduplicated-cond \
	-Wduplicated-branches \
	-Wjump-misses-init \
	-Warray-bounds=2 \
	-Wshift-overflow=2 \
	-Wcast-align=strict \
	-D_FORTIFY_SOURCE=2 \
	-Wlogical-op \
	-Wsuggest-attribute=const \
	-Wsuggest-attribute=format \
	-Wsuggest-attribute=noreturn \
	-Wsuggest-attribute=pure \
	-Wsync-nand \
	-Wtrampolines \
	-Wunsafe-loop-optimizations
else
	CFLAGS += -Wno-newline-eof \
		-Warray-bounds \
		-Wshift-overflow \
		-Wshift-sign-overflow \
		-Wcast-align \
		-Warray-bounds-pointer-arithmetic \
		-Wimplicit-fallthrough \
		-Wconditional-uninitialized \
		-Wloop-analysis \
		-Wtautological-constant-in-range-compare \
		-Wcomma \
		-Wassign-enum \
		-Wformat-type-confusion \
		-Wpointer-arith \
		-Widiomatic-parentheses \
		-Wunreachable-code-aggressive
endif

_OBJS := $(patsubst %.c,%.o,$(shell find skull -name "*.c" | grep -v "main\|codegen\|cli"))
_OBJS_TEST := $(patsubst %.c,%.o,$(shell find test/skull -name "*.c" | grep -v ".sk.c\|e2e") \
	$(shell find skull -name "*.c" | grep -v "main\|errors\|ast_node\|hashtable\|classify\|codegen\|cli") test/testing.c)
_OBJS_LLVM := $(patsubst %.c,%.o,$(shell find skull/codegen -name "*.c"))

E2E := $(patsubst %.sk,%.c,$(shell find test/sh -name "*.sk" | grep -v "skull\|dummy"))

ODIR := build/objs
OBJS := $(addprefix $(ODIR)/,$(_OBJS))
OBJS_TEST := $(addprefix $(ODIR)/,$(_OBJS_TEST))
OBJS_LLVM := $(addprefix $(ODIR)/,$(_OBJS_LLVM))

DIRS := build/skull \
	build/test \
	$(ODIR)/skull/parse/ast \
	$(ODIR)/skull/common \
	$(ODIR)/skull/codegen/llvm \
	$(ODIR)/skull/codegen/c \
	$(ODIR)/skull/semantic \
	$(ODIR)/test/skull/semantic \
	$(ODIR)/test/skull/parse/ast \
	$(ODIR)/test/skull/codegen/c \
	$(ODIR)/test/skull/common
