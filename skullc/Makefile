.PHONY: skullc

CC = g++

CXXFLAGS = $(shell llvm-config-9 --cxxflags --libs) -std=c++14

skullc:
	@echo "\033[92mCompile\033[0m skullc"
	@$(CC) skullc.cpp -o .skullc $(CXXFLAGS)
