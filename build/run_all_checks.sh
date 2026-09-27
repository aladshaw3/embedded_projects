#!/bin/bash
echo "********** unit-tests ***********"
make unit-tests | grep Failed

echo "********** unit-tests (DEBUG) ***********"
make unit-tests DEBUG=TRUE| grep Failed

echo "********** unit-tests-on-gcc ***********"
make unit-tests CMAKE_TOOLCHAIN_PATH=./projects/gcc.cmake | grep Failed

echo "********** unit-tests-asan-and-lsan ***********"
make unit-tests-asan-and-lsan | grep Failed

echo "********** static-code-analysis ***********"
ERR_OUTPUT=$(make static-code-analysis 2>&1 1>/dev/null)
echo "$ERR_OUTPUT"

echo "********** lint-filename ***********"
make lint-filename

echo "********** pre-commit ***********"
pre-commit run --all-files

echo "********** api-docs ***********"
make api-docs QUITE_DOCS=YES
