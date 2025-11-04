# Documentation Reorganization

**Date:** 2024-11-04  
**Branch:** `feat/docs/add-cn-readme-move-docs-to-doc-dir`

## Overview

This document describes the reorganization of project documentation to improve structure and add Chinese language support.

## Changes Made

### 1. Added Chinese README

Created `README.zh-CN.md` - a complete Chinese translation of the main README file.

**Features:**
- Full translation of all sections
- Language switcher links between English and Chinese versions
- Updated documentation references to point to `doc/` directory
- Maintained consistent formatting and structure with English version

**Content includes:**
- 项目概述和特性 (Project overview and features)
- 目录结构 (Directory structure)
- 安装说明 (Installation instructions)
- 构建和运行 (Building and running)
- 模块文档 (Module documentation)
- 开发指南 (Development guidelines)
- 故障排除 (Troubleshooting)

### 2. Moved Documentation to doc/ Directory

All documentation files (except README files) have been moved from the project root to the `doc/` directory:

**Files moved:**
- `API.md` → `doc/API.md`
- `ARCHITECTURE.md` → `doc/ARCHITECTURE.md`
- `BUGFIX_SUMMARY.md` → `doc/BUGFIX_SUMMARY.md`
- `BUGFIX_THREAD_CRASH.md` → `doc/BUGFIX_THREAD_CRASH.md`
- `CHANGELOG.md` → `doc/CHANGELOG.md`
- `CONTRIBUTING.md` → `doc/CONTRIBUTING.md`
- `DATABASE_SCHEMA.md` → `doc/DATABASE_SCHEMA.md`
- `DOCS_INDEX.md` → `doc/DOCS_INDEX.md`
- `DOCS_UPDATE_SUMMARY.md` → `doc/DOCS_UPDATE_SUMMARY.md`
- `INSTALL.md` → `doc/INSTALL.md`
- `LOCALIZATION.md` → `doc/LOCALIZATION.md`
- `REFACTORING_SUMMARY.md` → `doc/REFACTORING_SUMMARY.md`
- `UI_FEATURES.md` → `doc/UI_FEATURES.md`

**Files kept in root:**
- `README.md` (English)
- `README.zh-CN.md` (Chinese) - NEW
- `LICENSE`
- `.gitignore`

### 3. Updated All Internal Documentation Links

All markdown files have been updated to reflect the new structure:

**Changes made to documentation files in `doc/`:**
- Updated references to `README.md` → `../README.md`
- Added references to `README.zh-CN.md` → `../README.zh-CN.md`
- Updated `DOCS_INDEX.md` to include Chinese README
- Updated file statistics in `DOCS_INDEX.md` (11 → 12 files)

**Files updated:**
- `doc/API.md` - Related Documentation section
- `doc/DATABASE_SCHEMA.md` - Related Documentation section
- `doc/DOCS_INDEX.md` - All sections with links to README
- `doc/DOCS_UPDATE_SUMMARY.md` - Usage Guide and Maintenance sections
- `doc/INSTALL.md` - Next Steps and Additional Resources sections
- `doc/UI_FEATURES.md` - Related Documentation section

**Main README updates (`README.md`):**
- Added language switcher: `English | [简体中文](README.zh-CN.md)`
- Updated directory structure to show `doc/` folder
- Updated all documentation links to use `doc/` prefix
- Added Chinese README to documentation list

## New Directory Structure

```
.
├── README.md                # English project overview
├── README.zh-CN.md          # Chinese project overview (NEW)
├── LICENSE                  # MIT License
├── .gitignore              # Git ignore rules
├── Makefile                # Build system
├── build.sh                # Alternative build script
├── doc/                    # Documentation directory (NEW)
│   ├── API.md
│   ├── ARCHITECTURE.md
│   ├── BUGFIX_SUMMARY.md
│   ├── BUGFIX_THREAD_CRASH.md
│   ├── CHANGELOG.md
│   ├── CONTRIBUTING.md
│   ├── DATABASE_SCHEMA.md
│   ├── DOCS_INDEX.md
│   ├── DOCS_UPDATE_SUMMARY.md
│   ├── INSTALL.md
│   ├── LOCALIZATION.md
│   ├── REFACTORING_SUMMARY.md
│   └── UI_FEATURES.md
├── src/                    # Source code
├── tests/                  # Test suite
└── third_party/           # Third-party libraries
```

## Benefits

### Improved Organization
- Cleaner project root directory
- All documentation in a dedicated folder
- Easier to navigate and maintain
- Follows common open-source project conventions

### Enhanced Accessibility
- Chinese-speaking users can read documentation in their native language
- Language switcher makes it easy to switch between versions
- Both language versions maintained at the same quality level

### Better Maintainability
- Centralized documentation location
- Consistent link structure
- Easier to find and update documentation
- Clear separation between code and documentation

## Usage

### For Users

**English speakers:**
Start with [README.md](../README.md) in the project root.

**Chinese speakers (中文用户):**
从项目根目录的 [README.zh-CN.md](../README.zh-CN.md) 开始。

**Additional documentation:**
All other documentation is in the `doc/` directory. See [doc/DOCS_INDEX.md](DOCS_INDEX.md) for a complete guide.

### For Developers

When adding new documentation:
1. Place it in the `doc/` directory (unless it's a README variant)
2. Update [doc/DOCS_INDEX.md](DOCS_INDEX.md) with the new file
3. Add links from related documentation files
4. If updating core functionality, update both `README.md` and `README.zh-CN.md`

When updating existing documentation:
1. Update the file in the `doc/` directory
2. If it affects README content, update both language versions
3. Check for cross-references that may need updating
4. Update [doc/DOCS_INDEX.md](DOCS_INDEX.md) if the changes are significant

## Migration Checklist

- ✅ Created Chinese README (`README.zh-CN.md`)
- ✅ Created `doc/` directory
- ✅ Moved all documentation files to `doc/`
- ✅ Updated `README.md` with language switcher
- ✅ Updated `README.md` documentation links
- ✅ Updated `README.zh-CN.md` documentation links
- ✅ Updated `doc/DOCS_INDEX.md`
- ✅ Updated `doc/API.md`
- ✅ Updated `doc/DATABASE_SCHEMA.md`
- ✅ Updated `doc/DOCS_UPDATE_SUMMARY.md`
- ✅ Updated `doc/INSTALL.md`
- ✅ Updated `doc/UI_FEATURES.md`
- ✅ Verified all internal links work correctly

## Statistics

### Documentation Files
- **Total:** 12 markdown files
- **Root:** 2 files (README.md, README.zh-CN.md)
- **doc/:** 13 files (including this file)
- **tests/:** 1 file (tests/README.md)

### Changes
- **Files created:** 1 (README.zh-CN.md)
- **Files moved:** 13 (all doc files to doc/)
- **Files updated:** 7 (README.md + 6 doc files)
- **Lines translated:** ~428 lines (Chinese README)

## Related Documentation

- [README.md](../README.md) - Main project documentation (English)
- [README.zh-CN.md](../README.zh-CN.md) - 主要项目文档（简体中文）
- [DOCS_INDEX.md](DOCS_INDEX.md) - Complete documentation index
- [CONTRIBUTING.md](CONTRIBUTING.md) - Contribution guidelines

---

**Note:** All links in documentation have been updated to reflect the new structure. If you encounter any broken links, please report them as issues.
