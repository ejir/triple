# Quick Start Guide

Get up and running in 5 minutes!

## For Developers (Quick Test Build)

If you just want to test the application without installing Cosmopolitan:

```bash
# Clone the repository
git clone <repository-url>
cd <repository-name>

# Build with GCC (development mode)
make dev

# Run the application
./app
```

You should see:
```
=== Cosmopolitan Web Application ===
Build: Nov  3 2025 07:12:57
Starting initialization...
...
Server ready!
Listening on: http://localhost:8080
Press Ctrl+C to stop
```

Press `Ctrl+C` to stop the server.

## For Production (Portable Binary)

To build an Actually Portable Executable that runs on Linux, Windows, macOS, and BSD:

### 1. Install Cosmopolitan

```bash
# Quick install
sudo mkdir -p /opt/cosmo
cd /opt/cosmo
sudo wget https://cosmo.zip/pub/cosmocc/cosmocc.zip
sudo unzip cosmocc.zip
sudo rm cosmocc.zip
sudo chmod +x bin/cosmocc bin/cosmoar
```

For detailed installation instructions, see [INSTALL.md](INSTALL.md).

### 2. Build the Application

```bash
# Clone the repository
git clone <repository-url>
cd <repository-name>

# Build with Cosmopolitan
make

# Run the application
./app.com
```

### 3. Deploy

The resulting `app.com` file is a single, self-contained binary that runs on:
- Linux (x86_64, ARM64)
- Windows (x86_64)
- macOS (x86_64, ARM64)
- FreeBSD, OpenBSD, NetBSD

Just copy `app.com` to your target system and run it!

## Project Overview

This is a modular web application framework with:

- **HTTP Server**: Custom implementation (stub, ready to expand)
- **Router**: URL-based request routing
- **Database**: SQLite integration (stub)
- **Templating**: HTML rendering system (stub)
- **Modules**:
  - **Board**: Message board/forum functionality
  - **Admin**: Administration panel
  - **Upload**: File upload handling

All modules are implemented as stubs with placeholder functions, ready for full implementation.

## Key Files

- `src/main.c` - Application entry point
- `src/http.c/h` - HTTP server
- `src/router.c/h` - URL routing
- `src/db.c/h` - Database layer
- `src/board.c/h` - Message board
- `src/admin.c/h` - Admin panel
- `src/upload.c/h` - File uploads
- `Makefile` - Build system
- `README.md` - Full documentation

## Common Commands

```bash
# Build with Cosmopolitan
make

# Build with GCC (fast development)
make dev

# Clean build artifacts
make clean

# Build and run
make run

# Show help
make help
```

## Configuration

The application can be configured by modifying constants in `src/main.c`:

```c
#define DEFAULT_PORT 8080        // HTTP server port
#define DEFAULT_DB_PATH "app.db" // Database file path
```

## Routes

Current routes (all return stub HTML):

- `GET /` - Board list
- `GET /board?id=<id>` - View board
- `GET /thread?id=<id>` - View thread
- `POST /thread` - Create thread
- `POST /post` - Create post
- `GET /admin` - Admin dashboard
- `GET /admin/login` - Login page
- `POST /admin/login` - Login submission
- `GET /admin/logout` - Logout
- `POST /upload` - File upload

## Next Steps

1. **Read the Documentation**:
   - [README.md](README.md) - Complete project documentation
   - [ARCHITECTURE.md](ARCHITECTURE.md) - Architecture and design
   - [CONTRIBUTING.md](CONTRIBUTING.md) - Contribution guidelines
   - [INSTALL.md](INSTALL.md) - Detailed installation instructions

2. **Explore the Code**:
   - Start with `src/main.c` to see initialization
   - Check `src/router.c` to understand routing
   - Look at module files to see the structure

3. **Start Developing**:
   - Replace stub implementations with real functionality
   - Add new routes and handlers
   - Implement database schema and queries
   - Create HTML templates

4. **Test Your Changes**:
   ```bash
   make clean
   make dev
   ./app
   ```

## Troubleshooting

### Build Fails

**Error**: `cosmocc: command not found`

**Solution**: Either install Cosmopolitan (see [INSTALL.md](INSTALL.md)) or use development mode:
```bash
make dev
```

### Permission Denied

**Error**: `./app: Permission denied`

**Solution**: Make the binary executable:
```bash
chmod +x app
./app
```

### Port Already in Use

**Error**: `Failed to initialize HTTP server`

**Solution**: The current stub doesn't actually bind to a port, but when implementing the full server, either change the port in `src/main.c` or kill the conflicting process.

## Getting Help

- Check [README.md](README.md) for detailed documentation
- Review [ARCHITECTURE.md](ARCHITECTURE.md) for design details
- See [INSTALL.md](INSTALL.md) for installation help
- Read [CONTRIBUTING.md](CONTRIBUTING.md) for development guidelines

## Example Development Session

```bash
# Clone and build
git clone <repository-url>
cd <repository-name>
make dev

# Run the application
./app
# Output: Server ready! Listening on: http://localhost:8080

# In another terminal, make changes to source files
vim src/board.c

# Rebuild and test
make clean && make dev && ./app

# When satisfied, build portable binary
make clean
make

# Deploy
scp app.com user@server:/opt/myapp/
ssh user@server '/opt/myapp/app.com'
```

## Contributing

We welcome contributions! See [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

Quick contribution workflow:
1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test with `make dev`
5. Submit a pull request

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Support

For questions or issues:
1. Check the documentation
2. Search existing issues
3. Create a new issue with:
   - Your OS and version
   - Build mode (cosmo or gcc)
   - Error messages
   - Steps to reproduce

Happy coding! 🚀
