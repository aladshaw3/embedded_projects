# Environment being built in
UNAME := $(shell uname)

# Default toolchain path to empty
TOOLCHAIN_PATH ?=
# Default to NOT building TrustZone secure app
SECURE_APP ?= OFF

# Clang Sanitizers
ENABLE_SANITIZERS ?= OFF
ANALYZE_ADDRESS ?= OFF
ANALYZE_DATAFLOW ?= OFF
ANALYZE_LEAK ?= OFF
ANALYZE_MEMORY ?= OFF
ANALYZE_THREAD ?= OFF
ANALYZE_UNDEFINED ?= OFF

# Clang-Tidy
CLANG_TIDY_ERROR_FIXING ?= OFF

BUILD_DIR ?= $(ROOT_DIR)/Build/$(APPLICATION_NAME)/${BUILD_TYPE}
UT_DIR ?= $(ROOT_DIR)/projects/UnitTests/Build/UnitTests
SCA_DIR ?= $(ROOT_DIR)/projects/StaticCodeAnalysis/Build/StaticCodeAnalysis

OS := $(shell uname)
ifeq ($(OS),Linux)
CORES ?= $(shell nproc 2>/dev/null)
# LCOV Test Coverage
ON_UBUNTU ?= TRUE
else ifeq ($(OS),Darwin)
CORES ?= $(shell sysctl -n hw.ncpu)
ON_UBUNTU ?= FALSE
else
CORES ?=
ON_UBUNTU ?=
endif

## Test

.PHONY: tests
tests: tests-rebuild
	@echo "Note: To see more info, go to $(UT_DIR) and run the following command"
	@echo "ctest --test-dir $(UT_DIR) --output-on-failure"
	@echo "Running project unit tests"
	@ctest --test-dir $(UT_DIR) --output-on-failure

.PHONY: tests-rebuild
tests-rebuild:
	@echo "Generating project files for building SIL++ unit tests"
	ENABLE_SANITIZERS=$(ENABLE_SANITIZERS) \
	ANALYZE_ADDRESS=$(ANALYZE_ADDRESS) \
	ANALYZE_DATAFLOW=$(ANALYZE_DATAFLOW) \
	ANALYZE_LEAK=$(ANALYZE_LEAK) \
	ANALYZE_MEMORY=$(ANALYZE_MEMORY) \
	ANALYZE_THREAD=$(ANALYZE_THREAD) \
	ANALYZE_UNDEFINED=$(ANALYZE_UNDEFINED) \
	LSAN_OPTIONS=$(LSAN_OPTIONS) \
	ASAN_OPTIONS=$(ASAN_OPTIONS) \
	./generate_ut_build.sh ${CMAKE_TOOLCHAIN_ABSOLUTE_FILE_PATH} ${TOOLCHAIN_PATH}
	@echo "Building project unit test"
	@echo "$(MAKE) -C $(UT_DIR) -j$(CORES)"
	@$(MAKE) -C $(UT_DIR) -j$(CORES)

.PHONY: tests-clean
tests-clean:
	@echo "Cleaning project Unit Tests"
	@[ ! -d "$(UT_DIR)" ] || { $(MAKE) -C $(UT_DIR) clean; }

.PHONY: test
test:
	@echo "Building project unit test"
	@echo "$(MAKE) -C $(UT_DIR) -j$(CORES)"
	@$(MAKE) -C $(UT_DIR) -j$(CORES)
ifeq ($(TEST),)
	@echo "Please specify unit test to run with parameter 'TEST=test-suite-name'"
else
	@echo "Note: To see more info, go to $(UT_DIR) and run the following command"
	@echo "ctest --test-dir $(UT_DIR) --output-on-failure -R $(TEST)"
	@echo "Running project unit test " $(TEST)
	@echo "ctest --test-dir $(UT_DIR) --output-on-failure -R $(TEST)"
	@ctest --test-dir $(UT_DIR) --output-on-failure -R $(TEST)
endif

.PHONY: gdb-test
gdb-test:
	@echo "Building project unit test"
	@echo "$(MAKE) -C $(UT_DIR) -j$(CORES)"
	@$(MAKE) -C $(UT_DIR) -j$(CORES)
ifeq ($(TEST),)
	@echo "Please specify unit test to run with parameter 'TEST=test-suite-name'"
else
ifeq ($(UNAME),Darwin)
	@echo "Note: To see more info, go to $(UT_DIR) and run the following command"
	@echo "lldb -- $(UT_DIR)/UnitTests --gtest_filter=$(TEST)"
	@echo "Running LLDB session on project unit test " $(TEST)
	@echo "lldb -- $(UT_DIR)/UnitTests --gtest_filter=$(TEST)"
	@lldb -- $(UT_DIR)/UnitTests --gtest_filter=$(TEST)
else ifeq ($(UNAME),Linux)
	@echo "Note: To see more info, go to $(UT_DIR) and run the following command"
	@echo "gdb --args $(UT_DIR)/UnitTests --gtest_filter=$(TEST)"
	@echo "Running GDB session on project unit test " $(TEST)
	@echo "gdb --args $(UT_DIR)/UnitTests --gtest_filter=$(TEST)"
	@gdb --args $(UT_DIR)/UnitTests --gtest_filter=$(TEST)
endif
endif

.PHONY: unit-test-coverage
unit-test-coverage:
	@echo "Generating project files for project unit test coverage"
	ENABLE_TEST_COVERAGE=ON \
	ON_UBUNTU=$(ON_UBUNTU) \
	./generate_ut_build.sh ${CMAKE_TOOLCHAIN_ABSOLUTE_FILE_PATH} ${TOOLCHAIN_PATH}
	@echo "Generating project unit test coverage report"
	@$(MAKE) -C $(UT_DIR) -j$(CORES) UnitTests_coverage

## Static code analysis - Clang-Tidy

.PHONY: static-code-analysis
static-code-analysis:
	@echo "Generating project files for running Clang-Tidy"
	CLANG_TIDY_ERROR_FIXING=$(CLANG_TIDY_ERROR_FIXING) \
	./generate_sca_build.sh ${CMAKE_TOOLCHAIN_ABSOLUTE_FILE_PATH} ${TOOLCHAIN_PATH}
	@echo "Run Clang-Tidy"
	@$(MAKE) -C $(SCA_DIR) -j$(CORES)

.PHONY: clean ${BUILD_DIR}
clean:
	@echo "Cleaning Application"
	@[ ! -d "$(BUILD_DIR)" ] || { $(MAKE) -C $(BUILD_DIR) clean; }

.PHONY: remove
remove:
	@echo "Removing Application"
	@[ ! -d "$(BUILD_DIR)" ] || { rm -rf $(BUILD_DIR); }
