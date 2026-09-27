[![Checks](https://github.com/aladshaw3/embedded_projects/actions/workflows/build-and-test.yml/badge.svg)](https://github.com/aladshaw3/embedded_projects/actions/workflows/build-and-test.yml)

## Personal Embedded Projects
Mono Repo for various personal embedded projects with my husband

This mono repo is strictly just for code development with a lot of boilerplate for helping
to write unit tests for the code. Does not include build instructions for specific devices.

# Getting Started

(1) Initial Setup

```bash
make environment
make grab-submodules
```

NOTE: Not strictly required if you have an environment that is already setup. See build/setup.sh for recommendations

(2) Running unit tests

```bash
make unit-tests
```

or

```bash
make unit-test TEST={TEST_NAME}
```

(3) Checking formatting

```bash
make static-code-analysis
make lint-filename
make pre-commit
```

or

```bash
make all-checks
```

(3) Building API-Docs

```bash
make api-docs
```

# Contributing

## New code project under `projects` folder

(1) Start by creating new folder under `projects`

e.g.,
```
embedded_projects/
├── projects/
│   ├── ExampleProject/
│   └── NewProject/         # Add here
```

(2) Update the `projects/sources.cmake` and `include.cmake` files for new location

e.g., source.cmake
```
################# Add other project directories here #####################
include_sources(ExampleProject)
include_sources(NewProject)      # Add here
```

e.g., include.cmake
```
################# Add other project directories here #####################
include_directories(${SIL_DIR}/ExampleProject)
include_directories(${SIL_DIR}/NewProject)
```

(3) Adding subdirectories in your project

You can add any subdirectories to your new project folder. You will
then need to add another layer of a `sources.cmake` file to add the
new subdirectories to be discoverable by the unit testing and static
code analysis infrastructure

See `projects/ExampleProject` for an example.

(4) Adding source code

Each sub-folder should contain both the header file and source file together.
Each sub-folder that contains source files needs to also include a `sources.cmake`
file. This file should call an `add_sources` helper function to tell cmake what
source files to include in the builds.

e.g., `projects/ExampleProject/unit_tests/sources.cmake`
```
add_sources(test_builds.cpp)
add_sources(test_eigen.cpp)
```
