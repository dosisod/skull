.PHONY: skullc

CC = cc

CFLAGS = $(shell llvm-config-9 --cflags --libs)

skullc:
	@echo "\033[92mCompile\033[0m skullc"
	@$(CC) skullc.c -o .skullc $(CFLAGS)
