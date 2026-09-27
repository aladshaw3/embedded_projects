#!/bin/bash
set -euo pipefail
# This script generates project files for running static code analysis
# such as Clang-Tidy using cmake.
#
# Usage:
#
#   ./generate_sca_build
#   $1 - CMake toolchain file
#   $2 - Toolchain path

main()
{
    # Get the path to the application repository directory
    # Note: Assume projects is a submodule in the top-level directory
    #       of the application repository
    WORKDIR=$( cd "$(dirname "$0")"/../projects/StaticCodeAnalysis && pwd )
    CMAKE_TOOLCHAIN_FILE="$1"

    if [ -z "${2:-}" ]; then
        TOOLCHAIN_PATH=""
    else
        TOOLCHAIN_PATH="$2"
    fi
    BUILD_ROOT_PATH=${WORKDIR}/Build

    # Create builds
    create_build StaticCodeAnalysis ${CMAKE_TOOLCHAIN_FILE} ${TOOLCHAIN_PATH} || exit 1
}

# Function - Creates the Cmake files for the build and makes
# $1 - Build output folder
# $2 - Optional toolchain file
# $3 - Optional toolchain path
function create_build()
{
    build_path="${BUILD_ROOT_PATH}/$1"
    cmake_args=""

    if [ -z "$1" ]; then
        printf "\n[ERROR - ${LINENO}] Bad arguments\n"
        #exit 1
    fi

    # Clear and create the build path
    printf "Clearing ${build_path}\n"
    mkdir -p ${build_path}
    rm -rf ${build_path}/*

    # Optionally select the toolchain file
    if [ ! -z "${2:-}" ]; then
        cmake_args="${cmake_args} -DCMAKE_TOOLCHAIN_FILE=${2}"
    fi

    if [ ! -z "${3:-}" ]; then
        cmake_args="${cmake_args} -DTOOLCHAIN_PATH=${3}"
    fi

    # Run Clang-Tidy
    cmake_args="${cmake_args} -DRUN_STATIC_CODE_ANALYSIS=ON"

    # Check if we want to add the --fix-error flag when running Clang-Tidy. Adding this flag will apply suggested fixes
    # even if compilation errors were found. If compiler errors have attached fix-its,
    # clang-tidy will apply them as well.
    clang_tidy_error_fixing="${CLANG_TIDY_ERROR_FIXING:-OFF}"
    cmake_args="${cmake_args} -DCLANG_TIDY_ERROR_FIXING=${clang_tidy_error_fixing}"

    # Call CMake to generate the build
    (
        set -x
        cd ${build_path} && cmake ${cmake_args} ${WORKDIR}
    )

    # Make sure cmake ran without issue
    if ! [ "$?" = "0" ]; then
        printf "\n[ERROR - ${LINENO}] Build step failed\n"
        exit 1
    fi
}

# calls main function - basically, just keeps the main up top for readability.
main "$@"
