#!/bin/sh

# shellcheck disable=SC2046,SC2215,SC2171

include config.mk

all: skull test docs e2e

.PHONY: skull test docs

options:
	@$(ECHO) "\033[92mCC:\033[0m $(CC)\n"
	@$(ECHO) "\033[92mCFLAGS:\033[0m $(CFLAGS)\n"
	@$(ECHO) "\033[92mOBJS:\033[0m $(OBJS)\n"
	@$(ECHO) "\033[92mOBJS_LLVM:\033[0m $(OBJS_LLVM)\n"
	@$(ECHO) "\033[92mOBJS_TEST:\033[0m $(OBJS_TEST)\n"
	@$(ECHO) "\033[92mOBJS_MAIN:\033[0m $(OBJS_MAIN)\n"

setup:
	@mkdir $(DIRS) -p

$(ODIR)/%.o: %.c %.h
	@$(ECHO) "\033[92mCompile\033[0m $<\n"
	@$(CC) $< -c -o "$@" $(CFLAGS) $(LLVM_CFLAGS)

test: setup | $(OBJS_LLVM) $(OBJS_TEST) $(ODIR)/skull/real_main.o
	@$(ECHO) "\033[92mLink\033[0m test\n"
	@$(CC) $| -o build/test/test $(CFLAGS) $(LLVM_LDFLAGS)

skull: setup | $(OBJS) $(OBJS_LLVM) $(ODIR)/skull/real_main.o
	@$(ECHO) "\033[92mLink\033[0m skull\n"
	@$(CC) $| -DRELEASE=$(RELEASE) -DSKULL_VERSION="\"$(SKULL_VERSION)\"" \
		skull/cli.c -o build/skull/skull \
		$(CFLAGS) $(LLVM_LDFLAGS) $(LLVM_CFLAGS)

e2e: setup | $(OBJS) $(OBJS_LLVM) $(ODIR)/skull/real_main.o
	@$(ECHO) "\033[92mLink\033[0m e2e tests\n"
	@$(CC) $| test/skull/e2e.c $(ODIR)/test/testing.o -o build/test/e2e \
		$(CFLAGS) $(LLVM_LDFLAGS) $(LLVM_CFLAGS)

docs:
	@$(ECHO) "\033[92mBuild\033[0m docs\n"
	@python3 docs/make_docs.py

clean:
	@$(ECHO) "\033[92mCleaning\033[0m\n"
	@rm -rf build/*

scaffold:
	@[ -n "$(NAME)" ] || { echo "NAME is not set, exiting"; exit 1; }
	@echo "\033[92mScaffold\033[0m $(NAME)\n"
	@cp ./test/skull/template.c.bak ./test/skull/$(NAME).c
	@cp ./test/skull/template.h.bak ./test/skull/$(NAME).h
	@cp ./skull/template.c.bak ./skull/$(NAME).c
	@cp ./skull/template.h.bak ./skull/$(NAME).h
	@sed -i "s/NAME/$(subst /,\\/,$(NAME))/" ./test/skull/$(NAME).c
	@sed -i "s/NAME/$(subst /,\\/,$(NAME))/" ./skull/$(NAME).c

install: clean | skull
	@mkdir -p $(MANPATH)
	@install -m 644 docs/skull/skull.1 $(MANPATH)
	@$(ECHO) "\033[92mInstall\033[0m Skull\n"
	@install build/skull/skull $(BIN)
	@make clean

install-dev:
	@$(ECHO) "\033[92mInstall\033[0m Skull headers\n"
	@mkdir -p $(HEADER)/skull
	@cp skull/Skull.h $(HEADER)/skull/Skull.h

uninstall:
	@$(ECHO) "\033[92mUninstall\033[0m Skull\n"
	@rm -f $(BIN)/skull
	@$(ECHO) "\033[92mUninstall\033[0m Skull docs\n"
	@rm -f $(MANPATH)skull.1
	@$(ECHO) "\033[92mUninstall\033[0m Skull headers\n"
	@rm -rf $(HEADER)/skull
