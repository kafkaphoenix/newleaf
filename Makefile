.DEFAULT_GOAL := help

# AutoDoc
# -------------------------------------------------------------------------
.PHONY: help
help: ## This help
	@awk 'BEGIN {FS = ":.*?## "} /^[a-zA-Z_-]+:.*?## / {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}' $(MAKEFILE_LIST)
.DEFAULT_GOAL := help

.PHONY: static
static: ## Generate static build
	cmake --preset static
	cmake --build build

.PHONY: shared
shared: ## Generate shared build
	cmake --preset shared
	cmake --build build

.PHONY: debug-msvc
debug-msvc: ## Generate debug build for MSVC
	cmake --preset debug-msvc
	cmake --build build

.PHONY: debug-clang
debug-clang: ## Generate debug build for Clang
	cmake --preset debug-clang
	cmake --build build

.PHONY: debug-gcc
debug-gcc: ## Generate debug build for GCC
	cmake --preset debug-gcc
	cmake --build build

.PHONY: clean
clean: ## Clean build directory
	rm -rf build

.PHONY: tests
tests: ## Run tests
	cmake --preset tests
	cmake --build build
	ctest --test-dir build