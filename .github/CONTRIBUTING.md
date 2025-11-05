# Contributing to ROPsmith

First off, thank you for considering contributing to **ROPsmith**! Your input helps improve and refine this project for everyone in the security and reverse engineering community.

## 🧩 How to Contribute

### 1. Fork & Clone

```bash
git clone https://github.com/<your-username>/ROPsmith.git
cd ROPsmith
```

### 2. Create a Feature Branch

```bash
git checkout -b feature/your-feature-name
```

### 3. Code Standards

* Write **clean, readable C/C++** code.
* Follow the existing **project structure** and naming conventions.
* Ensure code is **portable** across major Linux distributions, Windows, and macOS.
* Use comments for any complex logic, especially in assembly or exploit-related parts.

### 4. Testing

* Run all existing tests before submitting.
* Add new tests for new functionalities where possible.
* Verify that your code does **not break existing gadget detection logic**.

### 5. Commit Guidelines

Use clear, concise commit messages:

```text
feat: added new ROP chain generator logic
fix: corrected buffer size validation in parser
refactor: improved disassembly performance
```

### 6. Pull Request (PR)

* Open a PR to the `main` branch.
* Include a **detailed description** of what your changes do.
* Reference any related issues using GitHub keywords (e.g., `Fixes #12`).

Once reviewed and approved, your PR will be merged.

## Dev Requirements

* **CMake** >= 3.10
* **A C/C++ compiler** (GCC, Clang, MSVC)
* **Git** for version control
* **Familiarity** with *binary exploitation* and *ROP* concepts is a plus

## 🔒 Security Contributions

If you find a **vulnerability** in ROPsmith or in the way it handles binary data:

1. **Do not open a public issue.**
2. Report it privately via email to the maintainers (see below).
3. Include steps to reproduce, affected versions, and a minimal PoC if possible.

We take responsible disclosure seriously and will respond as soon as possible.

---

### 📧 Contact

For contribution-related questions or vulnerability reports:

* **Maintainer:** [[wh0crypt@protonmail.com](mailto:wh0crypt@protonmail.com)]

Thank you for helping make **ROPsmith** better and safer for everyone!
