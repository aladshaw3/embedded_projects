#!/bin/bash
sudo apt-get install gcc-arm-none-eabi build-essential libssl-dev cmake ccache doxygen graphviz cppcheck clang-tidy pre-commit clang-format lcov llvm-15 gdb-multiarch libusb-1.0
sudo ln -s "/usr/lib/llvm-15/bin/llvm-cov" "/usr/bin/llvm-cov"
sudo curl -sL -o /usr/local/bin/ls-lint https://github.com/loeffel-io/ls-lint/releases/download/v1.11.2/ls-lint-linux && sudo chmod +x /usr/local/bin/ls-lint
