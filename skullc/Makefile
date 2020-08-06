.PHONY: skullc

CC = cc

CFLAGS := $(shell llvm-config-9 --cflags --libs) -I..

ODIR = ../build/objs

_OBJS = $(patsubst ../%.c,%.o,$(shell find ../skull -name "*.c" | grep -v "main"))

OBJS := $(addprefix $(ODIR)/,$(_OBJS))

skullc:
	@echo "\033[92mCompile\033[0m skullc"
	@$(CC) skullc.c $(OBJS) -o .skullc $(CFLAGS)
