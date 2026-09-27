#!/bin/bash
set -euo pipefail
# This script generates project files using cmake and builds the project.
#
# Usage:
#
#   ./generate_ut_build
#   $1 - CMake toolchain file
#   $2 - Toolchain path

main()
{
    # Get the path to the unit test directory
    # Note: Assume projects is a submodule in the top-level directory
    #       of the application repository
    WORKDIR=$( cd "$(dirname "$0")"/../projects/UnitTests && pwd )
    CMAKE_TOOLCHAIN_FILE="$1"

    if [ -z "${2:-}" ]; then
        TOOLCHAIN_PATH=""
    else
        TOOLCHAIN_PATH="$2"
    fi

    BUILD_ROOT_PATH=${WORKDIR}/Build
    # Create builds
    create_build UnitTests ${CMAKE_TOOLCHAIN_FILE} ${TOOLCHAIN_PATH} || exit 1
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

    # Enable a debuggable build
    cmake_args="${cmake_args} -DCMAKE_BUILD_TYPE=${BUILD_TYPE}"

    # Enable unit test build
    cmake_args="${cmake_args} -DBUILD_UT=ON"

    # Check if we want to enable sanitizers
    enable_sanitizers="${ENABLE_SANITIZERS:-OFF}"
    cmake_args="${cmake_args} -DENABLE_SANITIZERS=${enable_sanitizers}"

    # Check if we want to analyze address
    analyze_address="${ANALYZE_ADDRESS:-OFF}"
    cmake_args="${cmake_args} -DANALYZE_ADDRESS=${analyze_address}"

    # Check if we want to analyze dataflow
    analyze_dataflow="${ANALYZE_DATAFLOW:-OFF}"
    cmake_args="${cmake_args} -DANALYZE_DATAFLOW=${analyze_dataflow}"

    # Check if we want to analyze leak
    analyze_leak="${ANALYZE_LEAK:-OFF}"
    cmake_args="${cmake_args} -DANALYZE_LEAK=${analyze_leak}"

    # Check if we want to analyze memory
    analyze_memory="${ANALYZE_MEMORY:-OFF}"
    cmake_args="${cmake_args} -DANALYZE_MEMORY=${analyze_memory}"

    # Check if we want to analyze thread
    analyze_thread="${ANALYZE_THREAD:-OFF}"
    cmake_args="${cmake_args} -DANALYZE_THREAD=${analyze_thread}"

    # Check if we want to analyze undefined
    analyze_undefined="${ANALYZE_UNDEFINED:-OFF}"
    cmake_args="${cmake_args} -DANALYZE_UNDEFINED=${analyze_undefined}"

    # Check if we want to enable test coverage
    enable_test_coverage="${ENABLE_TEST_COVERAGE:-OFF}"
    cmake_args="${cmake_args} -DENABLE_TEST_COVERAGE=${enable_test_coverage}"

    if [ "${enable_test_coverage}" = "ON" ]; then
        # The following is used to override the GCOV_PATH in bilke-cmake-modules/CodeCoverage.cmake
        cmake_args="${cmake_args} -DGCOV_PATH=${WORKDIR}/../bin/llvm-gcov"
    fi

    # Check if we want to run the build on ubuntu
    on_ubuntu="${ON_UBUNTU:-FALSE}"
    cmake_args="${cmake_args} -DON_UBUNTU=${on_ubuntu}"

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
