# GitHub Actions CI/CD Setup

## Overview

This document describes the GitHub Actions CI/CD pipeline that has been added to the Cosmopolitan Web Application project.

## What Was Added

### 1. GitHub Actions Workflow (`.github/workflows/ci.yml`)

A comprehensive CI/CD pipeline that includes:

#### Build Jobs
- **build-gcc**: Builds the application using standard GCC compiler for Linux
- **build-cosmo**: Builds the application using Cosmopolitan Libc to create an Actually Portable Executable (APE)

#### Test Jobs
- **test-gcc**: Runs the full test suite with the GCC build
- **test-cosmo**: Runs the full test suite with the Cosmopolitan build

#### Quality Jobs
- **lint**: Performs code quality checks with strict compiler warnings (-Wall -Wextra -Werror)
- **integration**: Integration testing that starts the application and tests HTTP endpoints

#### Release Job
- **release**: Creates release artifacts with checksums (only on main branch or tags)

### 2. Documentation
- `.github/workflows/README.md`: Comprehensive documentation about the CI/CD pipeline
- `CI_SETUP.md` (this file): Overview and setup information

### 3. Updated .gitignore
- Added `app` binary to the ignored files list

## Features

### Smart Caching
- Cosmopolitan toolchain is cached to speed up subsequent builds
- Cache key: `cosmo-toolchain-v1`

### Artifact Management
- **app-gcc-build**: Linux binary (7 days retention)
- **app-cosmopolitan-ape**: APE binary (30 days retention)
- **integration-test-logs**: Application logs (7 days retention)
- **release-artifacts**: Release bundle with checksums (90 days retention)

### Trigger Conditions
The CI pipeline runs on:
- Push to `main`, `develop`, or `github-actions-ci-build-test` branches
- Pull requests to `main` or `develop` branches
- Manual workflow dispatch (can be triggered manually from GitHub UI)

### Cross-Platform Support
The Cosmopolitan build creates an Actually Portable Executable that runs on:
- Linux (x86_64)
- Windows
- macOS
- FreeBSD
- OpenBSD
- NetBSD

## How to Use

### Viewing CI Results
1. Go to the "Actions" tab in your GitHub repository
2. Click on a workflow run to see detailed results
3. Click on individual jobs to see logs and artifacts

### Downloading Artifacts
1. Navigate to a completed workflow run
2. Scroll to the "Artifacts" section at the bottom
3. Click on the artifact name to download

### Manual Trigger
1. Go to the "Actions" tab
2. Select "CI Build and Test" workflow
3. Click "Run workflow" button
4. Select the branch and click "Run workflow"

## Local Testing

Before pushing, you can test the same commands locally:

```bash
# GCC build and test
make BUILD_MODE=gcc
make BUILD_MODE=gcc test

# Cosmopolitan build and test
make BUILD_MODE=cosmo
make BUILD_MODE=cosmo test

# Lint check
make BUILD_MODE=gcc CFLAGS="-Wall -Wextra -Werror -std=c11 -Isrc -Ithird_party/sqlite3"

# Clean build artifacts
make clean
```

## CI/CD Pipeline Flow

```
Push/PR
  ↓
┌─────────────────────────────────────┐
│  Build Jobs (Parallel)              │
│  - build-gcc                        │
│  - build-cosmo                      │
└─────────────────────────────────────┘
  ↓
┌─────────────────────────────────────┐
│  Test Jobs (Parallel)               │
│  - test-gcc (needs: build-gcc)      │
│  - test-cosmo (needs: build-cosmo)  │
│  - lint                             │
└─────────────────────────────────────┘
  ↓
┌─────────────────────────────────────┐
│  Integration Test                   │
│  (needs: build-gcc, test-gcc)       │
└─────────────────────────────────────┘
  ↓
┌─────────────────────────────────────┐
│  Release (if main/tag)              │
│  (needs: test-gcc, test-cosmo)      │
└─────────────────────────────────────┘
```

## Customization

### Changing Retention Periods
Edit the `retention-days` values in the workflow file:
```yaml
retention-days: 7  # Change to desired number of days
```

### Adding New Tests
Simply add new test files to the `tests/` directory. The Makefile will automatically discover and build them.

### Modifying Build Flags
Update the `CFLAGS` in the Makefile or pass them via command line:
```yaml
make BUILD_MODE=gcc CFLAGS="-O3 -march=native"
```

### Adding More Build Platforms
Add new jobs for different platforms:
```yaml
build-macos:
  runs-on: macos-latest
  steps:
    # ... build steps
```

## Troubleshooting

### Cosmopolitan Download Fails
If the Cosmopolitan toolchain fails to download:
1. Check if https://cosmo.zip is accessible
2. Update the download URLs in the workflow
3. Clear the cache by changing the cache key

### Tests Timeout or Fail
1. Review the test logs in the GitHub Actions UI
2. Run the same test locally to reproduce
3. Check if there are platform-specific issues

### Integration Test Fails
1. Download the `integration-test-logs` artifact
2. Review the application logs for errors
3. Ensure port 8080 is not blocked or in use

## Benefits

1. **Automated Quality Assurance**: Every push is automatically built and tested
2. **Cross-Platform Validation**: Both GCC and Cosmopolitan builds are tested
3. **Early Bug Detection**: Integration tests catch runtime issues
4. **Release Automation**: Release artifacts are automatically generated
5. **Code Quality**: Strict compiler warnings enforce code quality
6. **Documentation**: Comprehensive logs and artifacts for debugging

## Next Steps

Consider adding:
- Automated deployment to staging/production
- Security scanning (SAST/DAST)
- Code coverage reporting
- Performance benchmarks
- Automated release notes generation
- Container image building and publishing
- Multi-architecture builds (ARM, etc.)

## References

- [GitHub Actions Documentation](https://docs.github.com/en/actions)
- [Cosmopolitan Libc](https://github.com/jart/cosmopolitan)
- [Project README](README.md)
- [Workflow Documentation](.github/workflows/README.md)
