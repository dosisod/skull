#!/bin/sh

# shellcheck disable=SC2046

include config.mk

all: skull test docs

.PHONY: skull test docs

options:
	@$(ECHO) "\033[92mCC:\033[0m $(CC)\n"
	@$(ECHO) "\033[92mCFLAGS:\033[0m $(CFLAGS)\n"
	@$(ECHO) "\033[92mOBJS:\033[0m $(OBJS)\n"
	@$(ECHO) "\033[92mOBJS_LLVM:\033[0m $(OBJS_LLVM)\n"
	@$(ECHO) "\033[92mOBJS_TEST:\033[0m $(OBJS_TEST)\n"

setup:
	@mkdir $(DIRS) -p

skull: setup | $(OBJS) $(OBJS_LLVM) $(OBJS_MAIN)
	@$(ECHO) "\033[92mLink\033[0m Skull\n"
	@$(CC) $(OBJS) $(OBJS_LLVM) $(OBJS_MAIN) -o build/skull/_skull $(CFLAGS) $(LLVM_LDFLAGS)

$(ODIR)/%.o: %.c %.h
	@$(ECHO) "\033[92mCompile\033[0m $<\n"
	@$(CC) $< -c -o "$@" $(CFLAGS) $(LLVM_CFLAGS)

test: setup | $(OBJS) $(OBJS_LLVM) $(OBJS_TEST)
	@$(ECHO) "\033[92mLink\033[0m test\n"
	@$(CC) $(OBJS) $(OBJS_LLVM) $(OBJS_TEST) -o build/test/test $(CFLAGS) $(LLVM_LDFLAGS)

docs:
	@$(ECHO) "\033[92mBuild\033[0m docs\n"
	@python3 docs/make_docs.py

clean:
	@$(ECHO) "\033[92mCleaning\033[0m\n"
	@rm -rf build/*

install: clean | skull
	@mkdir -p $(MANPATH)
	@install -m 644 docs/skull/skull.1 $(MANPATH)
	@$(ECHO) "\033[92mInstall\033[0m Skull\n"
	@install skull/skull $(BIN)
	@install build/skull/_skull $(BIN)
	@make clean

install-dev:
	@$(ECHO) "\033[92mInstall\033[0m Skull headers\n"
	@mkdir -p $(HEADER)/skull
	@cp skull/Skull.h $(HEADER)/skull/Skull.h

uninstall:
	@$(ECHO) "\033[92mUninstall\033[0m Skull\n"
	@rm -f $(BIN)skull
	@rm -f $(BIN)_skull
	@$(ECHO) "\033[92mUninstall\033[0m Skull docs\n"
	@rm -f $(MANPATH)skull.1
	@$(ECHO) "\033[92mUninstall\033[0m Skull headers\n"
	@rm -rf $(HEADER)/skull
