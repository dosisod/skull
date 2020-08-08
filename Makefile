include config.mk

all: skull skullc test docs

.PHONY: skull skullc test docs

options:
	@echo "\033[92mCC:\033[0m $(CC)"
	@echo "\033[92mCFLAGS:\033[0m $(CFLAGS)"
	@echo "\033[92mOBJS:\033[0m $(OBJS)"

setup:
	@mkdir $(DIRS) -p

skull: setup | $(ODIR)/skull/main.o $(OBJS)
	@echo "\033[92mLink\033[0m skull"
	@$(CC) $(ODIR)/skull/main.o $(OBJS) -o build/skull/skull $(CFLAGS)

skullc: skull | $(OBJS)
	@echo "\033[92mCompile\033[0m skullc"
	@$(CC) skullc/skullc.c $(OBJS) -o build/skullc/.skullc $(CFLAGS) $(LLVMFLAGS)

test: skull | $(ODIR)/test/main.o
	@echo "\033[92mLink\033[0m tests"
	@$(CC) $(ODIR)/test/main.o $(OBJS) -o build/test/test $(CFLAGS)

$(ODIR)/%.o: %.c
	@echo "\033[92mCompile\033[0m $<"
	@$(CC) $< -c -o $@ $(CFLAGS)

docs:
	@echo "\033[92mBuild\033[0m docs"
	@python3 make_docs.py

clean:
	@echo "\033[92mCleaning\033[0m"
	@rm -rf build/*

install: clean | skull
	@echo "\033[92mInstall\033[0m skull"
	@install build/skull/skull $(INSTALL)/bin/
	@install -m 644 docs/skull.1 $(INSTALL)/man/man1/
	@make clean