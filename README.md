## embedded_projects
Repo for various embedded projects my husband and I are working on together

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
