# Cosmopolitan Web 应用程序

一个使用 [Cosmopolitan Libc](https://github.com/jart/cosmopolitan) 构建的可移植 Web 应用程序，可编译为实际可移植可执行文件（APE）——单个二进制文件可在 Linux、Windows、MacOS、FreeBSD、OpenBSD 和 NetBSD 上原生运行。

[English](README.md) | 简体中文

## 特性

- **真正的可移植性**：单个二进制文件可在多个操作系统上运行
- **嵌入式 SQLite**：静态数据库，无需外部依赖
- **模块化架构**：专用模块实现清晰的关注点分离
- **HTTP 服务器**：具有路由功能的独立 Web 服务器
- **管理界面**：内置会话管理的管理面板
- **文件上传**：支持文件上传功能
- **留言板**：论坛/图片板风格的功能，包含主题和帖子
- **国际化**：多语言支持（英语、简体中文），可轻松切换语言
- **Material Design UI**：遵循 Material Design 原则的现代响应式界面
- **颜文字选择器**：内置日本颜文字选择器，增强消息功能

## 目录结构

```
.
├── src/                    # 源代码目录
│   ├── main.c             # 应用程序入口和初始化
│   ├── http.c/h           # HTTP 服务器实现
│   ├── router.c/h         # URL 路由和请求分发
│   ├── db.c/h             # 数据库抽象层（SQLite）
│   ├── render.c/h         # HTML 渲染和模板
│   ├── admin.c/h          # 管理面板处理器
│   ├── auth.c/h           # 认证和会话管理
│   ├── board.c/h          # 留言板功能
│   ├── thread.h           # 主题处理器（占位符）
│   ├── post.h             # 帖子处理器（占位符）
│   ├── upload.c/h         # 文件上传处理
│   ├── i18n.c/h           # 国际化/本地化
│   ├── kaomoji.c/h        # 颜文字数据和选择器
│   ├── utils.c/h          # 通用工具函数
│   └── html_template.c/h  # HTML 模板助手和通用 CSS
├── third_party/           # 第三方库
│   └── sqlite3/           # SQLite3 合并版
├── tests/                 # 测试套件
├── obj/                   # 编译的目标文件（生成）
├── doc/                   # 文档目录
├── Makefile              # 主要构建系统
├── build.sh              # 替代构建脚本
└── README.md             # 英文说明文件
```

## 前置要求

您需要在系统上安装 Cosmopolitan 工具链。

### 安装 Cosmopolitan

**选项 1：下载预构建的二进制文件**

```bash
# 下载并解压到 /opt/cosmo
sudo mkdir -p /opt/cosmo
cd /opt/cosmo
sudo wget https://cosmo.zip/pub/cosmocc/cosmocc.zip
sudo unzip cosmocc.zip
sudo rm cosmocc.zip
sudo chmod +x bin/cosmocc bin/cosmoar
```

**选项 2：从源代码构建**

```bash
git clone https://github.com/jart/cosmopolitan.git
cd cosmopolitan
make -j$(nproc)
sudo make install PREFIX=/opt/cosmo
```

**选项 3：使用自定义位置**

如果您将 Cosmopolitan 安装到不同位置，请设置 `COSMO_DIR` 环境变量：

```bash
export COSMO_DIR=/path/to/your/cosmo
```

## 构建

### 使用 Make（推荐）

```bash
make
```

这将编译所有源文件并生成 `app.com`，一个实际可移植的可执行文件。

**其他 make 目标：**

- `make clean` - 删除构建产物
- `make run` - 构建并运行应用程序
- `make help` - 显示帮助信息

### 使用 build.sh

```bash
./build.sh
```

构建脚本提供了不使用 Make 构建项目的替代方法。

### 构建输出

成功构建后，您将得到：

- `app.com` - 实际可移植的可执行文件
- `obj/*.o` - 编译的目标文件

## 运行

只需执行二进制文件：

```bash
./app.com
```

应用程序将：
1. 初始化数据库
2. 运行迁移
3. 设置路由
4. 在 8080 端口启动 HTTP 服务器

在以下地址访问应用程序：`http://localhost:8080`

**键盘快捷键：**
- `Ctrl+C` - 优雅地关闭服务器

## 模块文档

### HTTP 模块（`http.c/h`）

处理 HTTP 协议实现、服务器生命周期和响应生成。

**主要函数：**
- `http_server_init()` - 在指定端口初始化服务器
- `http_server_run()` - 主服务器循环
- `http_response_create()` - 创建 HTTP 响应
- `http_response_free()` - 清理响应对象

### Router 模块（`router.c/h`）

URL 路由和请求分发到适当的处理器。

**主要函数：**
- `router_init()` - 初始化路由表
- `router_add_route()` - 注册路由处理器
- `router_dispatch()` - 将请求匹配到处理器

### Database 模块（`db.c/h`）

SQLite 数据库抽象，包含连接管理和查询助手。

**主要函数：**
- `db_init()` - 打开数据库连接
- `db_migrate()` - 运行模式迁移
- `db_prepare()` - 准备 SQL 语句
- `db_exec()` - 执行 SQL 查询

### Render 模块（`render.c/h`）

HTML 生成和模板系统。

**主要函数：**
- `render_template()` - 使用数据渲染模板
- `render_html()` - 创建 HTML 响应
- `render_escape_html()` - 清理用户输入

### Board 模块（`board.c/h`）

带有主题和帖子的留言板/论坛功能。

**主要函数：**
- `board_list_handler()` - 列出所有版块
- `board_view_handler()` - 查看版块主题
- `thread_view_handler()` - 查看主题帖子
- `thread_create_handler()` - 创建新主题
- `post_create_handler()` - 向主题添加帖子

### Admin 模块（`admin.c/h`）

管理应用程序的管理界面。

**主要函数：**
- `admin_dashboard_handler()` - 管理控制面板
- `admin_login_handler()` - 认证
- `admin_logout_handler()` - 会话终止
- `admin_board_create_handler()` - 创建新版块
- `admin_board_delete_handler()` - 删除版块

### Auth 模块（`auth.c/h`）

集中式认证和会话管理。

**主要函数：**
- `auth_is_authenticated()` - 检查用户是否有有效会话
- `auth_create_session()` - 创建新会话令牌
- `auth_destroy_session()` - 使会话无效

### Upload 模块（`upload.c/h`）

文件上传处理和存储。

**主要函数：**
- `upload_handler()` - 处理文件上传
- `upload_parse_multipart()` - 解析 multipart 表单数据
- `upload_save_file()` - 保存上传的文件

### Internationalization 模块（`i18n.c/h`）

多语言支持，包含翻译管理和语言检测。

**主要函数：**
- `i18n_get_language()` - 检测用户的语言偏好（从 URL、cookie 或默认值）
- `i18n_get()` - 获取键的翻译文本
- `i18n_get_lang_code()` - 获取语言代码（例如，"en"、"zh-cn"）
- `i18n_get_lang_name()` - 获取语言显示名称

**支持的语言：**
- 英语（`LANG_EN`）
- 简体中文（`LANG_ZH_CN`）

有关详细的本地化文档，请参阅 [doc/LOCALIZATION.md](doc/LOCALIZATION.md)。

### Kaomoji 模块（`kaomoji.c/h`）

日本颜文字数据和选择器功能。

**主要函数：**
- `kaomoji_get_categories()` - 获取所有表情类别
- `kaomoji_get_categories_count()` - 获取类别计数

**类别：**
- 开心、兴奋、爱、悲伤、生气、困惑、惊讶、可爱、动物、动作、物品、符号

### Utils 模块（`utils.c/h`）

应用程序中使用的通用工具函数。

**主要函数：**
- `url_decode()` - 查询字符串的 URL 解码
- `get_cookie_value()` - 从标头解析 cookie 值
- `generate_random_token()` - 生成安全的随机令牌

### HTML Template 模块（`html_template.c/h`）

通用 HTML 渲染函数和共享 CSS。

**主要函数：**
- `html_get_common_css()` - 返回 Material Design CSS
- `html_render_header()` - 渲染 HTML 文档标头
- `html_render_footer()` - 渲染 HTML 文档页脚

## 开发

### 代码风格

- **缩进**：4 个空格（不使用制表符）
- **行长度**：最多 100 个字符
- **命名约定**：
  - 函数：`snake_case`
  - 结构体：`snake_case_t` 后缀
  - 常量：`UPPER_SNAKE_CASE`
  - 宏：`UPPER_SNAKE_CASE`
- **大括号**：K&R 风格（左大括号在同一行）
- **注释**：单行使用 `//`，多行使用 `/* */`

### 添加新模块

1. 创建 `src/module.h`，包含函数声明和类型定义
2. 创建 `src/module.c`，包含实现
3. 将 `#include "module.h"` 添加到 `main.c`
4. 从 `main()` 调用初始化函数
5. 如果需要，注册路由
6. 将模块添加到 `build.sh` 中的 `SOURCES`（Makefile 将自动检测）

### 测试

构建后，测试应用程序：

```bash
./app.com
```

验证：
- 应用程序启动时没有错误
- 所有模块成功初始化
- HTTP 服务器在配置的端口上启动
- 路由正确注册

## 贡献

### 工作流程

1. 创建功能分支
2. 进行更改
3. 彻底测试
4. 提交拉取请求

### 代码指南

- 保持函数专注和小巧（尽可能 < 50 行）
- 为所有系统调用添加错误处理
- 对不可变参数使用 `const`
- 释放所有分配的内存
- 检查所有返回值
- 记录复杂的算法

### 提交消息

使用常规提交格式：

```
<type>(<scope>): <subject>

<body>

<footer>
```

类型：`feat`、`fix`、`docs`、`style`、`refactor`、`test`、`chore`

示例：
```
feat(router): 添加通配符路由匹配

实现路由定义中通配符模式的支持
以实现更灵活的 URL 路由。

Closes #123
```

## 故障排除

### 未找到 Cosmopolitan

**错误**：`Error: Cosmopolitan toolchain not found`

**解决方案**：安装 Cosmopolitan 或设置 `COSMO_DIR`：
```bash
export COSMO_DIR=/path/to/cosmopolitan
```

### 运行时权限被拒绝

**错误**：`bash: ./app.com: Permission denied`

**解决方案**：使二进制文件可执行：
```bash
chmod +x app.com
```

### 端口已被使用

**错误**：`Failed to initialize HTTP server`

**解决方案**：更改端口或停止冲突的服务：
```bash
# 查找使用 8080 端口的进程
lsof -i :8080
# 终止进程或更改 main.c 中的 DEFAULT_PORT
```

## 架构

### 请求流程

```
HTTP 请求
    ↓
http_server_run() - 接受连接
    ↓
router_dispatch() - 匹配路由
    ↓
路由处理器 - 处理请求
    ↓
render_template() - 生成 HTML
    ↓
http_response_create() - 构建响应
    ↓
HTTP 响应
```

### 数据流程

```
客户端 → HTTP 服务器 → 路由器 → 处理器 → 数据库
                                    ↓
客户端 ← HTTP 服务器 ← 路由器 ← 响应 ← 渲染
```

## 文档

📚 **有关完整的文档指南，请参阅 [doc/DOCS_INDEX.md](doc/DOCS_INDEX.md)**

### 核心文档
- [README.md](README.md) - 英文说明文件
- [README.zh-CN.md](README.zh-CN.md) - 本文件（项目概述）
- [doc/ARCHITECTURE.md](doc/ARCHITECTURE.md) - 系统架构和设计
- [doc/INSTALL.md](doc/INSTALL.md) - 安装和构建说明
- [doc/CONTRIBUTING.md](doc/CONTRIBUTING.md) - 贡献指南

### 功能文档
- [doc/LOCALIZATION.md](doc/LOCALIZATION.md) - 国际化/本地化指南
- [doc/UI_FEATURES.md](doc/UI_FEATURES.md) - Material Design UI 和颜文字选择器
- [doc/DATABASE_SCHEMA.md](doc/DATABASE_SCHEMA.md) - 数据库模式和查询
- [doc/API.md](doc/API.md) - HTTP 端点和 API 参考

### 开发历史
- [doc/CHANGELOG.md](doc/CHANGELOG.md) - 版本历史和更改
- [doc/REFACTORING_SUMMARY.md](doc/REFACTORING_SUMMARY.md) - 代码重构详情
- [doc/BUGFIX_SUMMARY.md](doc/BUGFIX_SUMMARY.md) - 数据库稳定性修复
- [doc/BUGFIX_THREAD_CRASH.md](doc/BUGFIX_THREAD_CRASH.md) - 主题视图崩溃修复

## 许可证

本项目根据 MIT 许可证授权 - 有关详细信息，请参阅 [LICENSE](LICENSE) 文件。

## 致谢

- [Cosmopolitan Libc](https://github.com/jart/cosmopolitan) - 作者 Justine Tunney
- [SQLite](https://www.sqlite.org/) - 嵌入式数据库
- [Material Design](https://material.io/) - UI/UX 原则
