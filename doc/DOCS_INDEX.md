# Documentation Index

Complete guide to all documentation in this project.

## Quick Start

1. **New to the project?** Start with [README.md](../README.md) or [README.zh-CN.md](../README.zh-CN.md)
2. **Installing?** See [INSTALL.md](INSTALL.md)
3. **Contributing?** Read [CONTRIBUTING.md](CONTRIBUTING.md)

## Core Documentation

### 📘 [README.md](../README.md)
**Main project documentation (English)**
- Project overview and features
- Directory structure
- Building and running
- Module documentation
- Development guidelines

### 📘 [README.zh-CN.md](../README.zh-CN.md)
**主要项目文档（简体中文）**
- 项目概述和特性
- 目录结构
- 构建和运行
- 模块文档
- 开发指南

### 🏗️ [ARCHITECTURE.md](ARCHITECTURE.md)
**System architecture and design**
- Architecture diagrams
- Module descriptions
- Data flow patterns
- Design patterns
- Extensibility guidelines

### 📦 [INSTALL.md](INSTALL.md)
**Installation and build instructions**
- Prerequisites for all platforms
- Installing Cosmopolitan toolchain
- Building the project
- Platform-specific notes
- Troubleshooting

### 🤝 [CONTRIBUTING.md](CONTRIBUTING.md)
**Contribution guidelines**
- Code of conduct
- Development workflow
- Code style rules
- Testing guidelines
- Pull request process

## Feature Documentation

### 🌍 [LOCALIZATION.md](LOCALIZATION.md)
**Internationalization/Localization**
- Supported languages (English, 简体中文)
- Language switching methods
- Adding translations
- Technical implementation
- Font support

### 🎨 [UI_FEATURES.md](UI_FEATURES.md)
**Material Design UI and Kaomoji**
- Material Design implementation
- Color scheme and components
- Responsive design breakpoints
- Kaomoji emoticon picker (12 categories, 100+ emoticons)
- Language switcher
- Accessibility features

### 🗄️ [DATABASE_SCHEMA.md](DATABASE_SCHEMA.md)
**Database structure and queries**
- SQLite configuration
- Table schemas (boards, threads, posts, sessions)
- Entity relationships
- Common queries
- Performance optimization
- Backup and maintenance

### 🔌 [API.md](API.md)
**HTTP endpoints and API reference**
- Public endpoints (boards, threads, posts)
- Admin endpoints (dashboard, management)
- Request/response formats
- Authentication and sessions
- Error responses
- Future JSON API plans

## Development History

### 📝 [CHANGELOG.md](CHANGELOG.md)
**Version history and changes**
- Latest features (i18n, Material Design, Kaomoji)
- Bug fixes (SIGSEGV, database stability)
- CI/CD enhancements
- Release notes

### ♻️ [REFACTORING_SUMMARY.md](REFACTORING_SUMMARY.md)
**Code refactoring details**
- New modules created (auth, utils, kaomoji, html_template)
- Modified modules (admin, board)
- Benefits and improvements
- File structure after refactoring
- Future refactoring plans

### 🐛 [BUGFIX_SUMMARY.md](BUGFIX_SUMMARY.md)
**Database stability fixes**
- ENOENT/Segfault issue resolution
- SQLite PRAGMA configuration
- Directory creation fixes
- CI/CD release improvements

### 💥 [BUGFIX_THREAD_CRASH.md](BUGFIX_THREAD_CRASH.md)
**Thread view SIGSEGV crash fix**
- Problem description and root cause
- Format string parameter mismatch
- Testing and verification
- Prevention guidelines

## Documentation by Role

### For New Users
1. [README.md](../README.md) or [README.zh-CN.md](../README.zh-CN.md) - Understand what the project does
2. [INSTALL.md](INSTALL.md) - Get it running
3. [LOCALIZATION.md](LOCALIZATION.md) - Switch to your language
4. [UI_FEATURES.md](UI_FEATURES.md) - Learn about the interface

### For Developers
1. [CONTRIBUTING.md](CONTRIBUTING.md) - How to contribute
2. [ARCHITECTURE.md](ARCHITECTURE.md) - Understand the system
3. [DATABASE_SCHEMA.md](DATABASE_SCHEMA.md) - Work with data
4. [API.md](API.md) - Understand the endpoints
5. [REFACTORING_SUMMARY.md](REFACTORING_SUMMARY.md) - Code organization

### For Administrators
1. [INSTALL.md](INSTALL.md) - Deploy the application
2. [DATABASE_SCHEMA.md](DATABASE_SCHEMA.md) - Manage the database
3. [API.md](API.md) - Admin endpoints
4. [BUGFIX_SUMMARY.md](BUGFIX_SUMMARY.md) - Troubleshooting

### For Translators
1. [LOCALIZATION.md](LOCALIZATION.md) - Complete i18n guide
2. [UI_FEATURES.md](UI_FEATURES.md) - UI elements to translate
3. [API.md](API.md) - Endpoint parameters

## Documentation by Topic

### Installation & Setup
- [INSTALL.md](INSTALL.md) - Complete installation guide
- [README.md](../README.md#prerequisites) - Prerequisites
- [README.md](../README.md#building) - Building instructions

### Architecture & Design
- [ARCHITECTURE.md](ARCHITECTURE.md) - Full architecture documentation
- [DATABASE_SCHEMA.md](DATABASE_SCHEMA.md) - Database design
- [REFACTORING_SUMMARY.md](REFACTORING_SUMMARY.md) - Code organization

### Features
- [LOCALIZATION.md](LOCALIZATION.md) - Multi-language support
- [UI_FEATURES.md](UI_FEATURES.md) - UI components and Kaomoji
- [API.md](API.md) - All endpoints

### Development
- [CONTRIBUTING.md](CONTRIBUTING.md) - Contribution process
- [README.md](../README.md#development) - Development guidelines
- [CHANGELOG.md](CHANGELOG.md) - What's changed

### Troubleshooting
- [BUGFIX_SUMMARY.md](BUGFIX_SUMMARY.md) - Database issues
- [BUGFIX_THREAD_CRASH.md](BUGFIX_THREAD_CRASH.md) - SIGSEGV crash
- [INSTALL.md](INSTALL.md#troubleshooting) - Installation problems
- [DATABASE_SCHEMA.md](DATABASE_SCHEMA.md#troubleshooting) - Database problems

## File Statistics

### Documentation Size
```
Total: 12 markdown files
Core:    5 files (README x2, ARCHITECTURE, INSTALL, CONTRIBUTING)
Feature: 4 files (LOCALIZATION, UI_FEATURES, DATABASE_SCHEMA, API)
History: 3 files (CHANGELOG, REFACTORING_SUMMARY, BUGFIX_*)
```

### Coverage
- ✅ Installation and setup
- ✅ Architecture and design
- ✅ All features documented
- ✅ API reference complete
- ✅ Database schema documented
- ✅ Internationalization guide
- ✅ UI/UX documentation
- ✅ Contribution guidelines
- ✅ Troubleshooting guides
- ✅ Version history

## Maintenance

### Updating Documentation

When adding features:
1. Update [CHANGELOG.md](CHANGELOG.md) with the change
2. Update [README.md](../README.md) and [README.zh-CN.md](../README.zh-CN.md) if it affects core functionality
3. Update [ARCHITECTURE.md](ARCHITECTURE.md) if it affects design
4. Update feature-specific docs ([LOCALIZATION.md](LOCALIZATION.md), [UI_FEATURES.md](UI_FEATURES.md), etc.)
5. Update [API.md](API.md) if it adds/changes endpoints

When fixing bugs:
1. Document major fixes in BUGFIX_*.md files
2. Update [CHANGELOG.md](CHANGELOG.md)
3. Update [DATABASE_SCHEMA.md](DATABASE_SCHEMA.md#troubleshooting) if database-related

When refactoring:
1. Document in [REFACTORING_SUMMARY.md](REFACTORING_SUMMARY.md)
2. Update [ARCHITECTURE.md](ARCHITECTURE.md)
3. Update [README.md](../README.md) and [README.zh-CN.md](../README.zh-CN.md) module documentation

## Documentation Standards

### File Naming
- Use UPPERCASE for important docs (README, INSTALL, etc.)
- Use underscores for multi-word names (BUGFIX_SUMMARY.md)
- Use .md extension for markdown files

### Content Structure
- Start with # heading (file title)
- Include table of contents for long documents
- Use ## for major sections
- Use ### for subsections
- Include examples where appropriate
- Link to related documentation

### Code Examples
- Use proper syntax highlighting (```bash, ```c, ```sql)
- Include complete, runnable examples
- Comment complex code

### Maintenance
- Keep documentation up-to-date with code
- Review and update during releases
- Add to CHANGELOG.md when docs change

## Contributing to Documentation

See [CONTRIBUTING.md](CONTRIBUTING.md) for general contribution guidelines.

**Documentation-specific tips:**
- Use clear, concise language
- Include examples and screenshots where helpful
- Link to related documentation
- Update DOCS_INDEX.md when adding new docs
- Test all code examples
- Use proper markdown formatting

## Questions?

If you can't find what you're looking for:
1. Check this index again
2. Use GitHub's search feature
3. Create an issue with the `documentation` label
4. Check related documentation links at the bottom of each doc

---

**Last Updated:** 2024-11-04  
**Documentation Version:** 1.0  
**Project Version:** See [CHANGELOG.md](CHANGELOG.md)
