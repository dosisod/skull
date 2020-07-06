include config.mk

all: src test docs

.PHONY: src test docs

setup:
	@mkdir $(DIRS) -p

src: setup | $(OBJS)
	@echo "\033[92mCompile\033[0m skull"
	@$(CC) src/main.c $(OBJS) -o build/src/skull $(CFLAGS)

test: src
	@echo "\033[92mCompile\033[0m tests"
	@$(CC) test/main.c $(OBJS) -o build/test/test $(CFLAGS)

$(ODIR)/%.o: %.c
	@echo "\033[92mCompile\033[0m $<"
	@$(CC) $< -c -o $@ $(CFLAGS)

docs:
	@echo "\033[92mCompile\033[0m docs"
	@python3 make_docs.py

clean:
	@rm -rf build/*

install: clean | src
	@echo "\033[92mInstall\033[0m skull"
	@install build/src/skull $(INSTALL)/bin/
	@install -m 644 docs/skull.1 $(INSTALL)/man/man1/
	@make clean