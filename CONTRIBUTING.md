# Contributing to Mercurial Core Detector

Thank you for your interest in contributing to the Mercurial Core Detector (MCD)!

This project aims to provide a clean, hardware‑agnostic way to detect silent data corruption in distributed training clusters.
These are still baby steps so we wholeheartedly welcome contributions of all kinds: bug reports, feature suggestions, documentation improvements, and code patches.

## How to Contribute

### 1. Reporting Bugs & Suggesting Features

- **Bugs**: If you find a bug, please open a [GitHub Issue](https://github.com/howler-dev/mercurial-core-detector/issues/new) with a clear description, steps to reproduce (if applicable), and your environment.
- **Features**: For feature requests, open an issue and label it `enhancement`. Describe the problem you’re trying to solve and your proposed solution.

### 2. Submitting Changes (Pull Requests)

Use the standard GitHub fork + pull request workflow:
1. **Fork** the repository on GitHub.
2. **Clone** your fork locally:
   ```bash
   git clone https://github.com/howler-dev/mercurial-core-detector.git
   ```
3. Create a new branch for your changes:
  ```bash
  git checkout -b my-feature-branch
  ```
4. Make your changes, following the coding guidelines below.
5. Test your changes (see “Development Setup”).
6. Commit your changes with a clear, descriptive commit message.
7. Push to your fork:
  ```bash
  git push origin my-feature-branch
  ```
8. Open a Pull Request against the `main` branch of this repository. In the PR description, explain what your changes do and why they’re needed.
   
### 3. Development Setup
- Build dependencies: A C99 compiler (gcc/clang) and `make`.
- Clone the repository and build:
  ```bash
  git clone https://github.com/howler-dev/mercurial-core-detector.git
  cd mercurial-core-detector
  make
  ```
- Run the tests:
  ```bash
  ./test_checkpoint
  ```
All tests must pass before submitting a PR.

### Coding Guidelines
To maintain the reliability and auditability of the code, we must enforce a strict set of rules. All contributions must adhere to these:
- No recursion. All iteration must be explicit loops.
- Every loop must have a provable upper bound. There should be no unbounded loops (e.g., `while(1)` without a break condition that is clearly bounded).
- No dynamic memory allocation after initialization. All memory must be static or stack‑allocated. (The library does no allocation at all after startup.)
- Function length ≤ 60 lines. This keeps functions short and focused.
- Minimum 2 assertions per function. Use `assert()` to check preconditions, postconditions, and invariants.
- Every return value must be checked, especially for functions that can fail (like the consensus checks).
- Zero compiler warnings. Compile with `-Wall -Wextra -pedantic` and ensure no warnings.
- Static Analysis: We strongly recommend running a static analyser (e.g., `clang-tidy`, `cppcheck`) on your code.
- No function pointers. All calls must be direct (this avoids indirect control flow that could be harder to verify).
- Restricted Pointer Dereferencing: Avoid double indirection; use simple array indexing where possible.

These rules help ensure the code remains simple, verifiable, and less vulnerable to the very hardware errors it is designed to detect.

### Testing
- The test suite (`tests/test_checkpoint.c`) covers basic functionality: saving/verifying checkpoints, detecting corruption, and handling the “no checkpoint” case.
- If you add new features, please add corresponding tests.
- Run `make clean && make && ./test_checkpoint` to verify everything works.

### Documentation
- Update the `README.md` if your change affects usage or build instructions.
- For new functions or features, add comments explaining their purpose and any invariants.

### Code of Conduct
We expect all contributors to be respectful and constructive.
Please review `CODE_OF_CONDUCT.md`.

### License
By contributing, you agree that your contributions will be licensed under the MIT License (see `LICENSE`).
