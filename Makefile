include config.mk

all: skull skullc test docs

.PHONY: skull skullc test docs

options:
	@echo "\033[92mCC:\033[0m $(CC)"
	@echo "\033[92mCFLAGS:\033[0m $(CFLAGS)"
	@echo "\033[92mOBJS_SKULL:\033[0m $(OBJS_SKULL)"
	@echo "\033[92mOBJS_SKULLC:\033[0m $(OBJS_SKULLC)"

setup:
	@mkdir $(DIRS) -p

skull: setup | $(ODIR)/skull/main.o $(OBJS_SKULL)
	@echo "\033[92mLink\033[0m skull"
	@$(CC) $(ODIR)/skull/main.o $(OBJS_SKULL) -o build/skull/skull $(CFLAGS)

skullc: skull | $(ODIR)/skullc/main.o $(OBJS_SKULLC)
	@echo "\033[92mLink\033[0m skullc"
	@$(CC) $(ODIR)/skullc/main.o $(OBJS_SKULL) $(OBJS_SKULLC) -o build/skullc/_skullc $(CFLAGS) $(LLVM_LDFLAGS)

test: skull | $(ODIR)/test/skull/main.o
	@echo "\033[92mLink\033[0m test"
	@$(CC) $(ODIR)/test/skull/main.o $(OBJS_SKULL) -o build/test/test $(CFLAGS)

$(ODIR)/%.o: %.c
	@echo "\033[92mCompile\033[0m $<"
	@$(CC) $< -c -o $@ $(CFLAGS)

$(ODIR)/skullc/%.o: skullc/%.c
	@echo "\033[92mCompile\033[0m $<"
	@$(CC) $< -c -o $@ $(CFLAGS) $(LLVM_CFLAGS)

docs:
	@echo "\033[92mBuild\033[0m docs"
	@python3 make_docs.py

clean:
	@echo "\033[92mCleaning\033[0m"
	@rm -rf build/*

install: clean | skullc
	@echo "\033[92mInstall\033[0m skull"
	@install build/skull/skull $(INSTALL)/bin/
	@install -m 644 docs/skull/skull.1 $(INSTALL)/man/man1/
	@echo "\033[92mInstall\033[0m skullc"
	@install skullc/skullc $(INSTALL)/bin/
	@install build/skullc/_skullc $(INSTALL)/bin/
	@make clean