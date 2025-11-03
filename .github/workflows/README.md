# GitHub Actions CI/CD

This directory contains GitHub Actions workflow files for continuous integration and deployment.

## Workflows

### CI Build and Test (`ci.yml`)

Automated build and testing pipeline that runs on every push and pull request.

#### Jobs

1. **build-gcc** - Build with GCC compiler
   - Uses standard GCC for Linux
   - Creates `app` binary for development testing
   - Uploads artifact for use in subsequent jobs

2. **build-cosmo** - Build with Cosmopolitan
   - Uses Cosmopolitan Libc toolchain
   - Creates Actually Portable Executable (`app.com`)
   - Caches toolchain for faster subsequent builds
   - Produces cross-platform binary

3. **test-gcc** - Run tests with GCC build
   - Executes all test suites
   - Validates functionality on Linux

4. **test-cosmo** - Run tests with Cosmopolitan build
   - Executes all test suites with APE
   - Validates cross-platform compatibility

5. **lint** - Code quality checks
   - Compiles with strict warnings (`-Wall -Wextra -Werror`)
   - Ensures code meets quality standards

6. **integration** - Integration testing
   - Starts the application
   - Tests HTTP endpoints
   - Validates server functionality

7. **release** - Create release artifacts
   - Runs only on main branch or tags
   - Bundles both GCC and Cosmopolitan builds
   - Generates checksums
   - Creates release-ready artifacts

## Triggers

The CI pipeline runs on:
- Push to `main`, `develop`, or `github-actions-ci-build-test` branches
- Pull requests to `main` or `develop` branches
- Manual workflow dispatch

## Artifacts

The workflow produces several artifacts:

- **app-gcc-build** - Linux binary built with GCC (7 days retention)
- **app-cosmopolitan-ape** - Actually Portable Executable (30 days retention)
- **integration-test-logs** - Application logs from integration tests (7 days retention)
- **release-artifacts** - Release bundle with checksums (90 days retention)

## Caching

The workflow caches the Cosmopolitan toolchain to speed up builds:
- Cache key: `cosmo-toolchain-v1`
- Location: `/opt/cosmo`

## Local Testing

To test the workflow locally, you can run the same commands:

### GCC Build and Test
```bash
make BUILD_MODE=gcc
make BUILD_MODE=gcc test
```

### Cosmopolitan Build and Test
```bash
make BUILD_MODE=cosmo
make BUILD_MODE=cosmo test
```

### Lint Check
```bash
make BUILD_MODE=gcc CFLAGS="-Wall -Wextra -Werror -std=c11 -Isrc -Ithird_party/sqlite3"
```

## Customization

### Changing Build Configuration

Edit the workflow file to modify:
- Compiler flags
- Test timeouts
- Artifact retention periods
- Branch triggers

### Adding New Tests

Tests are automatically discovered from the `tests/` directory by the Makefile.

### Modifying Release Process

The release job can be customized to:
- Create GitHub releases automatically
- Deploy to servers
- Publish to package registries
- Generate release notes

## Troubleshooting

### Cosmopolitan Installation Fails

If the Cosmopolitan toolchain download fails:
1. Check https://cosmo.zip/pub/cosmos/bin/ for available files
2. Update the download URLs in the workflow
3. Invalidate the cache by changing the cache key

### Tests Fail

Check the test logs in the Actions tab:
1. Click on the failed job
2. Expand the "Build and run tests" step
3. Review error messages

### Integration Test Timeout

If the application doesn't start in time:
1. Increase the sleep duration in the "Wait for application to start" step
2. Check application logs artifact
3. Verify port 8080 is not in use

## Best Practices

1. **Always run tests locally** before pushing
2. **Review artifacts** to ensure builds are correct
3. **Monitor cache usage** to optimize build times
4. **Update dependencies** in the workflow regularly
5. **Keep workflows simple** and maintainable

## References

- [GitHub Actions Documentation](https://docs.github.com/en/actions)
- [Cosmopolitan Libc](https://github.com/jart/cosmopolitan)
- [Project Build Documentation](../../README.md)
