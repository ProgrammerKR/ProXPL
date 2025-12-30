# ProXPL Versioning and Release Guide

**Current Version: 0.3.0**  
**Release Date: December 2024**

This document describes the versioning strategy, semantic versioning policy, and release procedures for ProXPL.

---

## Semantic Versioning (SemVer)

ProXPL follows [Semantic Versioning 2.0.0](https://semver.org/).

**Format:** `MAJOR.MINOR.PATCH[-PRERELEASE][+BUILD]`

### Version Components

- **MAJOR**: Incompatible API changes (0.x = unstable)
- **MINOR**: New features, backward compatible
- **PATCH**: Bug fixes, backward compatible
- **PRERELEASE**: Pre-release versions (alpha, beta, rc)
- **BUILD**: Build metadata (not part of version precedence)

### Examples

```
0.1.0          Final release v0.1.0
0.1.0-alpha    Alpha pre-release
0.1.0-beta.1   Beta 1 pre-release
0.1.0-rc.1     Release candidate 1
1.0.0          Major release (stable API)
1.0.0+build.42 Build 42 of v1.0.0
```

---

## Version History

### v0.1.0 (December 2024) - Initial Release

**Status**: Alpha - Educational and experimental use

**Features**:
- ✅ Core language implementation
- ✅ 75+ standard library functions
- ✅ Bytecode compiler and VM
- ✅ Type system with compile-time checking
- ✅ Comprehensive documentation
- ✅ Examples and tutorials
- ✅ CI/CD pipeline

**Known Limitations**:
- No OOP (classes/inheritance)
- No module system
- No async/await
- Limited error recovery
- Single-threaded execution

**Stability**: Not guaranteed for production use

---

## Release Procedure

### Pre-Release Checklist

Before releasing a new version:

```bash
# 1. Update version numbers
# Edit: main.c, CMakeLists.txt, package.json (if applicable)
# Search for version string and update all occurrences

# 2. Update CHANGELOG.md
# - Add new version section
# - Summarize all changes since last release
# - Note breaking changes, deprecations
# - Link to GitHub compare

# 3. Run full test suite
make clean
make test
make lint

# 4. Build release artifacts
mkdir release
cmake -DCMAKE_BUILD_TYPE=Release -B release
cd release
make

# 5. Test artifacts
./prox ../examples/hello.prox
./prox ../examples/fibonacci.prox

# 6. Create release notes
# Draft release notes in GitHub

# 7. Update documentation
# - Verify all docs are current
# - Check tutorial examples
# - Update README version numbers
```

### Git Release Procedure

```bash
# 1. Create release branch
git checkout -b release/v0.2.0

# 2. Make version updates (as above)
git add .
git commit -m "chore: bump version to 0.2.0"

# 3. Create annotated tag
git tag -a v0.2.0 -m "Release version 0.2.0

- Feature 1: Description
- Feature 2: Description
- Bug fix 1: Description

See CHANGELOG.md for full details."

# 4. Merge to main
git checkout main
git pull origin main
git merge release/v0.2.0

# 5. Push changes and tags
git push origin main
git push origin v0.2.0

# 6. Create GitHub Release
# Automated via CI/CD or manual creation
```

### Changelog Format

```markdown
## [0.2.0] - 2025-01-15

### Added
- New feature X
- New feature Y
- New module Z

### Changed
- Improved performance of component A
- Updated documentation

### Fixed
- Fixed bug in parser
- Fixed memory leak in VM

### Removed
- Deprecated function (use X instead)

### Breaking Changes
- Function `old_func()` removed (use `new_func()`)

[0.2.0]: https://github.com/ProgrammerKR/ProXPL/compare/v0.1.0...v0.2.0
```

---

## Release Types

### Alpha Release (v0.x.0-alpha)

- Early stage, major features incomplete
- API likely to change significantly
- Not recommended for production
- Used for community feedback

### Beta Release (v0.x.0-beta.N)

- Feature-complete for version
- API stabilizing but may still change
- Most major bugs fixed
- Good for testing and feedback

### Release Candidate (v0.x.0-rc.N)

- All features completed and tested
- Only critical bug fixes allowed
- API is frozen
- Close to final release

### Stable Release (v1.0.0+)

- Stable API guaranteed
- Safe for production use
- Breaking changes only in major versions
- Maintenance support provided

### Hotfix Release (v1.0.1)

- Patch version for urgent bug fixes
- No new features
- Minimal changes
- Released quickly after issue discovery

---

## Backward Compatibility

### Compatibility Guarantees

**v0.x.x** (Pre-1.0):
- No backward compatibility guaranteed
- API may change between minor versions
- Breaking changes documented in changelog

**v1.0.0+** (Stable):
- API stable within major version
- Breaking changes only in major versions (v2.0.0)
- Deprecation warnings provided for one minor version

### Deprecation Policy

When deprecating functionality:

1. **Announce**: Add deprecation notice in release notes
2. **Warn**: Emit warning when deprecated function called
3. **Document**: Update documentation with migration path
4. **Support**: Provide at least 2 minor versions support
5. **Remove**: Remove only in next major version

```c
// Deprecated function example
void old_function() __attribute__((deprecated("use new_function() instead"))) {
    // ...
}
```

---

## Build Artifact Naming

Release artifacts follow pattern:

```
prox-v0.1.0-linux-x86_64.tar.gz
prox-v0.1.0-macos-arm64.zip
prox-v0.1.0-windows-x86_64.exe
```

---

## Version Management in Code

### C Code Version

File: `include/common.h`

```c
#define PROXPL_VERSION_MAJOR 0
#define PROXPL_VERSION_MINOR 1
#define PROXPL_VERSION_PATCH 0
#define PROXPL_VERSION_STRING "0.1.0"
#define PROXPL_VERSION_PRERELEASE "alpha"
```

### CMake Version

File: `CMakeLists.txt`

```cmake
project(ProXPL
    VERSION 0.1.0
    DESCRIPTION "A Modern Programming Language"
    LANGUAGES C
)
```

### Querying Version at Runtime

```c
func main() {
    print("ProXPL " + version());  // Outputs: "ProXPL 0.1.0"
}
```

---

## Roadmap

### v0.1.x (Current - Alpha)
- Bug fixes and stability improvements
- Documentation enhancements
- Performance optimization
- Additional examples

### v0.2.0 (Q1 2025 - Beta)
- Class-based OOP support
- Module/import system
- Exception handling improvements
- More standard library functions

### v0.3.0 (Q2 2025 - Beta)
- Async/await support
- Generators
- Decorators
- Pattern matching

### v1.0.0 (Q3 2025 - Stable)
- API freeze
- Production-ready
- Comprehensive tooling
- Official language specification

### v1.x.x (Ongoing)
- Maintenance releases
- Performance improvements
- Ecosystem development
- Community contributions

---

## Release Calendar

Target release schedule:

- **v0.1.0**: December 2024 (✅ Released)
- **v0.1.1**: January 2025 (Patch)
- **v0.2.0-beta**: February 2025 (Beta)
- **v0.2.0**: March 2025 (Stable beta)
- **v0.3.0**: June 2025
- **v1.0.0**: September 2025 (Stable production)

---

## Maintenance Policy

### Active Support
- Current and previous minor version
- Security and critical bug fixes

### Long-Term Support (LTS)
- v1.0.0 and subsequent major versions
- Minimum 2 years of support
- Security fixes for all versions in LTS period

### End of Life
- v0.x versions: EOL when v1.0.0 released
- Announced 6 months in advance
- Community can fork and maintain if needed

---

## Communication

### Announcements
- GitHub Releases page
- CHANGELOG.md
- Project website
- Email notifications (via watch feature)

### Feedback
- GitHub Issues for bug reports
- GitHub Discussions for feature requests
- Email: contact@proxpl.dev

---

## Summary

ProXPL uses semantic versioning to communicate changes clearly to users. Before v1.0.0, API stability is not guaranteed. Once v1.0.0 is released, breaking changes only occur in major versions, providing stability for production use.

For questions about versioning, see [GitHub Issues](https://github.com/ProgrammerKR/ProXPL/issues) or [Discussions](https://github.com/ProgrammerKR/ProXPL/discussions).
