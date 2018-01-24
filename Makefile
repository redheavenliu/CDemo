DIRS = lib apue demo x unpv13e stanet
OUT_DIR = out

all:
	test -d $(OUT_DIR) || mkdir -p $(OUT_DIR)
	@for i in $(DIRS); do \
		(cd $$i && echo "making $$i" && $(MAKE) ) || exit 1; \
	done
	@echo "Success"

clean:
	@for i in $(DIRS); do \
		(cd $$i && echo "cleaning $$i" && $(MAKE) clean) || exit 1; \
	done
	rm -rf ./out/*