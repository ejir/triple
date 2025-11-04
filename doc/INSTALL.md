# Installation Guide

This guide will help you set up the Cosmopolitan toolchain and build the project.

## Table of Contents

- [Prerequisites](#prerequisites)
- [Installing Cosmopolitan](#installing-cosmopolitan)
- [Building the Project](#building-the-project)
- [Troubleshooting](#troubleshooting)

## Prerequisites

### Linux

- A modern Linux distribution (Ubuntu, Debian, Fedora, Arch, etc.)
- Basic build tools (typically already installed)
- wget or curl for downloading files

### Windows

- WSL2 (Windows Subsystem for Linux) recommended
- Or use the binary directly (APE files run natively on Windows)

### macOS

- Xcode Command Line Tools
- Homebrew (optional, for easier installation)

## Installing Cosmopolitan

### Method 1: Quick Install (Recommended)

Download the pre-built Cosmopolitan toolchain:

```bash
# Create installation directory
sudo mkdir -p /opt/cosmo
cd /opt/cosmo

# Download the latest release
sudo wget https://cosmo.zip/pub/cosmocc/cosmocc.zip
sudo unzip cosmocc.zip
sudo rm cosmocc.zip

# Make them executable
sudo chmod +x bin/cosmocc bin/cosmoar

# Add to PATH (optional)
echo 'export PATH="/opt/cosmo/bin:$PATH"' >> ~/.bashrc
source ~/.bashrc
```

### Method 2: Using a specific version

```bash
sudo mkdir -p /opt/cosmo
cd /opt/cosmo

# Download a specific version (e.g., 4.0.2)
sudo wget https://cosmo.zip/pub/cosmocc/cosmocc-4.0.2.zip
sudo unzip cosmocc-4.0.2.zip
sudo rm cosmocc-4.0.2.zip
sudo chmod +x bin/cosmocc bin/cosmoar

# Verify installation
/opt/cosmo/bin/cosmocc --version
```

### Method 3: Build from Source (Advanced)

If you want the absolute latest version or need to customize:

```bash
# Clone the repository
git clone https://github.com/jart/cosmopolitan.git
cd cosmopolitan

# Build (this will take a while)
make -j$(nproc)

# Install to /opt/cosmo
sudo make install PREFIX=/opt/cosmo
```

### Method 4: Custom Installation Location

If you prefer to install elsewhere:

```bash
# Install to your home directory
mkdir -p ~/local/cosmo
cd ~/local/cosmo

# Download and extract binaries
wget https://cosmo.zip/pub/cosmocc/cosmocc.zip
unzip cosmocc.zip
rm cosmocc.zip

# Make them executable
chmod +x bin/cosmocc bin/cosmoar

# Set COSMO_DIR when building
export COSMO_DIR=~/local/cosmo
```

## Building the Project

### Step 1: Clone the Repository

```bash
git clone <repository-url>
cd <repository-name>
```

### Step 2: Build with Cosmopolitan

```bash
# If Cosmopolitan is in /opt/cosmo (default)
make

# If Cosmopolitan is elsewhere
make COSMO_DIR=/path/to/cosmo

# Or set environment variable
export COSMO_DIR=/path/to/cosmo
make
```

### Step 3: Run the Application

```bash
./app.com
```

The `.com` extension is part of the Actually Portable Executable format. Despite the extension, this works on:
- Linux (x86_64, ARM64)
- Windows (x86_64)
- macOS (x86_64, ARM64)
- FreeBSD
- OpenBSD
- NetBSD

### Alternative: Development Build with GCC

For quick development iterations without Cosmopolitan:

```bash
make dev
./app
```

This builds a standard Linux executable for faster compilation during development.

## Verifying the Installation

### Check Cosmopolitan Installation

```bash
# Verify cosmocc is installed
/opt/cosmo/bin/cosmocc --version

# Or if in PATH
cosmocc --version
```

### Test Build

```bash
# Clean build
make clean
make

# Should produce app.com
ls -lh app.com

# Test run
./app.com
```

Expected output:
```
=== Cosmopolitan Web Application ===
Build: Nov  3 2025 07:10:02
Starting initialization...
Database initialized: app.db (stub)
Running database migrations (stub)
Router initialized
Board module initialized
...
Server ready!
Listening on: http://localhost:8080
Press Ctrl+C to stop
```

## Troubleshooting

### Problem: "cosmocc: command not found"

**Solution 1**: Ensure Cosmopolitan is installed:
```bash
ls -l /opt/cosmo/bin/cosmocc
```

**Solution 2**: Set COSMO_DIR:
```bash
export COSMO_DIR=/path/to/your/cosmopolitan
make
```

**Solution 3**: Use development build:
```bash
make dev  # Uses GCC instead
```

### Problem: Permission denied when running app.com

**Solution**: Make the binary executable:
```bash
chmod +x app.com
./app.com
```

### Problem: Download failures

**Solution 1**: Try using curl instead of wget:
```bash
# Use curl to download
curl -O https://cosmo.zip/pub/cosmocc/cosmocc.zip
unzip cosmocc.zip
```

**Solution 2**: Build from source (see Method 3 above)

### Problem: Build fails with linker errors

**Solution**: Ensure you have a complete Cosmopolitan installation:
```bash
# Reinstall from the official zip
cd /opt/cosmo
sudo rm -rf *
sudo wget https://cosmo.zip/pub/cosmocc/cosmocc.zip
sudo unzip cosmocc.zip
sudo rm cosmocc.zip
sudo chmod +x bin/cosmocc bin/cosmoar
```

### Problem: "No such file or directory" on Windows

**Solution**: Windows users should use WSL2 for building. The resulting `.com` file will run natively on Windows without WSL.

### Problem: macOS security warning

**Solution**: On first run, macOS may block the executable:
```bash
# Allow the binary
xattr -d com.apple.quarantine app.com
./app.com
```

## Platform-Specific Notes

### Linux

Everything should work out of the box. Most distributions include all necessary dependencies.

### Windows

**Option 1**: Build in WSL2, then run the `.com` file in Windows:
```bash
# In WSL2
make
cp app.com /mnt/c/Users/YourName/Desktop/

# In Windows (Command Prompt or PowerShell)
cd C:\Users\YourName\Desktop
app.com
```

**Option 2**: Use pre-built binaries (if available)

### macOS

On Apple Silicon (M1/M2/M3):
```bash
# Should work with Rosetta 2 or native ARM64 build
make
./app.com
```

On Intel Macs:
```bash
# Standard build process
make
./app.com
```

### FreeBSD/OpenBSD/NetBSD

```bash
# Install GNU make if needed
pkg install gmake  # FreeBSD
pkg_add gmake      # OpenBSD
pkgin install gmake # NetBSD

# Build with gmake
gmake
./app.com
```

## Advanced Configuration

### Cross-Compilation

Cosmopolitan binaries are naturally cross-platform, but if you need specific optimization:

```bash
# Build optimized for size
make CFLAGS="-Os -Wall -Wextra -std=c11 -Isrc"

# Build with debug symbols
make CFLAGS="-g -O0 -Wall -Wextra -std=c11 -Isrc"
```

### Custom Build Options

```bash
# Build with specific Cosmopolitan version
make COSMO_DIR=/opt/cosmo-v2.0

# Build with additional flags
make CFLAGS="-O3 -march=native -Wall -Wextra -std=c11 -Isrc"

# Build with custom target name
make TARGET=myapp.com
```

## Next Steps

After successful installation:

1. Read [README.md](../README.md) for project documentation
2. Review [CONTRIBUTING.md](CONTRIBUTING.md) for development guidelines
3. Start the server: `./app.com`
4. Access the application at `http://localhost:8080`

## Additional Resources

- [Cosmopolitan Libc Documentation](https://justine.lol/cosmopolitan/)
- [Cosmopolitan GitHub Repository](https://github.com/jart/cosmopolitan)
- [Actually Portable Executable Format](https://justine.lol/ape.html)
- [Project README](../README.md)
- [Contributing Guidelines](CONTRIBUTING.md)

## Getting Help

If you encounter issues:

1. Check this troubleshooting section
2. Review Cosmopolitan documentation
3. Search existing issues in the project repository
4. Create a new issue with:
   - Your OS and version
   - Cosmopolitan version (`cosmocc --version`)
   - Full error message
   - Steps to reproduce
