.DEFAULT_GOAL := all

ROOT_DIR:=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))

# Environment being built in
UNAME := $(shell uname)
# silence mode for doc gen
QUITE_DOCS ?= NO

# Debug make option
ifeq ($(DEBUG),)
	DEBUG := FALSE
else
	DEBUG := TRUE
endif
ifeq ($(DEBUG),FALSE)
	BUILD_TYPE= Release
else
	BUILD_TYPE= Debug
endif

# The following variable is project-specific:
ifeq ($(DEBUG),FALSE)
	CMAKE_TOOLCHAIN_PATH="./projects/clang.cmake"
else
	ifeq ($(UNAME),Darwin)
		CMAKE_TOOLCHAIN_PATH="./projects/clang-lldb.cmake"
	else ifeq ($(UNAME),Linux)
		CMAKE_TOOLCHAIN_PATH="./projects/gcc-gdb.cmake"
	endif
endif
CMAKE_TOOLCHAIN_ABSOLUTE_FILE_PATH = $(shell realpath $(CMAKE_TOOLCHAIN_PATH))

ifeq ($(UNAME),Darwin)
	LLVM_PATH := $(shell brew --prefix llvm@16)/bin/
else ifeq ($(UNAME),Linux)
	LLVM_PATH := ""
endif

# The following variables are used by common.mk
export ROOT_DIR BUILD_TYPE CMAKE_TOOLCHAIN_ABSOLUTE_FILE_PATH TOOLCHAIN_PATH TEST

.PHONY: list
list:
	@LC_ALL=C $(MAKE) -pRrq -f $(lastword $(MAKEFILE_LIST)) : 2>/dev/null | awk -v RS= -F: '/^# File/,/^# Finished Make data base/ {if ($$1 !~ "^[#.]") {print $$1}}' | sort | grep -E -v -e '^[^[:alnum:]]' -e '^$@$$'

## Setup ##
.PHONY: grab-submodules
grab-submodules:
	git submodule add https://gitlab.com/libeigen/eigen.git libraries/eigen
	git submodule add https://github.com/google/googletest.git libraries/googletest
	git submodule add https://github.com/awdeorio/csvstream.git libraries/csvstream
	git submodule add https://github.com/aladshaw3/cmake-modules.git libraries/cmake-modules
	git submodule add https://github.com/ETLCPP/etl.git libraries/etl

.PHONY: environment
environment:
	@./build/setup.sh

## Static Code Analysis ##
.PHONY: static-code-analysis
static-code-analysis:
	@$(MAKE) -C build -f common.mk static-code-analysis TOOLCHAIN_PATH=${LLVM_PATH}

.PHONY: static-code-analysis-with-fixes
static-code-analysis-with-fixes:
	@$(MAKE) -C build -f common.mk static-code-analysis TOOLCHAIN_PATH=${LLVM_PATH} CLANG_TIDY_ERROR_FIXING=ON

.PHONY: lint-filename
lint-filename:
	@ls-lint -warn

## Tests ##
.PHONY: unit-tests
unit-tests:
	@$(MAKE) -C build -f common.mk tests TOOLCHAIN_PATH=${LLVM_PATH} BUILD_TYPE=${BUILD_TYPE}

.PHONY: unit-tests-clean
unit-tests-clean:
	@$(MAKE) -C build -f common.mk tests-clean TOOLCHAIN_PATH=${LLVM_PATH} BUILD_TYPE=${BUILD_TYPE}
	rm -rf projects/UnitTests/Build

.PHONY: unit-tests-asan-and-lsan
unit-tests-asan-and-lsan:
	@$(MAKE) -C build -f common.mk tests TOOLCHAIN_PATH=${LLVM_PATH} BUILD_TYPE=${BUILD_TYPE} ENABLE_SANITIZERS=ON ANALYZE_ADDRESS=ON LSAN_OPTIONS=suppressions=$(ROOT_DIR)/FW3-Algorithms/SanitizerSuppression.supp ASAN_OPTIONS=detect_leaks=1

.PHONY: unit-tests-ubsan
unit-tests-ubsan:
	@$(MAKE) -C build -f common.mk tests TOOLCHAIN_PATH=${LLVM_PATH} BUILD_TYPE=${BUILD_TYPE} ENABLE_SANITIZERS=ON ANALYZE_UNDEFINED=ON

.PHONY: unit-tests-rebuild
unit-tests-rebuild:
	@$(MAKE) -C build -f common.mk tests-rebuild TOOLCHAIN_PATH=${LLVM_PATH} BUILD_TYPE=${BUILD_TYPE}

.PHONY: gdb-unit-tests-rebuild
gdb-unit-tests-rebuild:
ifeq ($(UNAME),Darwin)
	@$(MAKE) -C build -f common.mk tests-rebuild TOOLCHAIN_PATH=${LLVM_PATH} BUILD_TYPE=Debug DEBUG=TRUE CMAKE_TOOLCHAIN_ABSOLUTE_FILE_PATH=$(shell realpath "./projects/clang-lldb.cmake")
else ifeq ($(UNAME),Linux)
	@$(MAKE) -C build -f common.mk tests-rebuild TOOLCHAIN_PATH=${LLVM_PATH} BUILD_TYPE=Debug DEBUG=TRUE CMAKE_TOOLCHAIN_ABSOLUTE_FILE_PATH=$(shell realpath "./projects/gcc-gdb.cmake")
endif

.PHONY: unit-test
unit-test:
	@$(MAKE) -C build -f common.mk test TEST=$(TEST) TOOLCHAIN_PATH=${LLVM_PATH} BUILD_TYPE=${BUILD_TYPE} ENABLE_SANITIZERS=ON ANALYZE_ADDRESS=ON ANALYZE_LEAK=ON LSAN_OPTIONS=suppressions=../../../SanitizerSuppression.supp

.PHONY: gdb-unit-test
gdb-unit-test: gdb-unit-tests-rebuild
	@$(MAKE) -C build -f common.mk gdb-test TEST=$(TEST) TOOLCHAIN_PATH=${LLVM_PATH} BUILD_TYPE=${BUILD_TYPE} ENABLE_SANITIZERS=ON ANALYZE_ADDRESS=ON ANALYZE_LEAK=ON LSAN_OPTIONS=suppressions=../../../SanitizerSuppression.supp

.PHONY: unit-test-coverage
unit-test-coverage:
	@echo "Generating unit test coverage report"
	@$(MAKE) -C build -f common.mk unit-test-coverage TOOLCHAIN_PATH=${LLVM_PATH} BUILD_TYPE=${BUILD_TYPE}

## Documentation ##
.PHONY: api-docs
api-docs:
	@echo "Generating API docs for project files"
	@./build/build_docs.sh ${QUITE_DOCS}

.PHONY: pre-commit
pre-commit:
	pre-commit run --all-files

.PHONY: all-checks
all-checks:
	@./build/run_all_checks.sh
