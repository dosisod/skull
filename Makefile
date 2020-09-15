include config.mk

all: skull test docs

.PHONY: skull test docs

options:
	@echo "\033[92mCC:\033[0m $(CC)"
	@echo "\033[92mCFLAGS:\033[0m $(CFLAGS)"
	@echo "\033[92mOBJS:\033[0m $(OBJS)"
	@echo "\033[92mOBJS_LLVM:\033[0m $(OBJS_LLVM)"
	@echo "\033[92mOBJS_TEST:\033[0m $(OBJS_TEST)"

setup:
	@mkdir $(DIRS) -p

skull: setup | $(OBJS) $(OBJS_LLVM)
	@echo "\033[92mLink\033[0m skull"
	@$(CC) $(OBJS) $(OBJS_LLVM) -o build/skull/_skull $(CFLAGS) $(LLVM_LDFLAGS)

$(ODIR)/%.o: %.c
	@echo "\033[92mCompile\033[0m $<"
	@$(CC) $< -c -o $@ $(CFLAGS)

$(ODIR)/skull/main.o: skull/main.c
	@echo "\033[92mCompile\033[0m $<"
	@$(CC) $< -c -o $@ $(CFLAGS) $(LLVM_CFLAGS)

$(ODIR)/skull/llvm/%.o: skull/llvm/%.c
	@echo "\033[92mCompile\033[0m $<"
	@$(CC) $< -c -o $@ $(CFLAGS) $(LLVM_CFLAGS)

test: skull | $(OBJS_TEST)
	@echo "\033[92mLink\033[0m test"
	@$(CC) $(OBJS) $(OBJS_TEST) -o build/test/test $(CFLAGS)

docs:
	@echo "\033[92mBuild\033[0m docs"
	@python3 make_docs.py

clean:
	@echo "\033[92mCleaning\033[0m"
	@rm -rf build/*

install: clean | skull
	@mkdir -p $(MANPATH)
	@install -m 644 docs/skull/skull.1 $(MANPATH)
	@echo "\033[92mInstall\033[0m skull"
	@install skull/skull $(BIN)
	@install build/skull/_skull $(BIN)
	@make clean