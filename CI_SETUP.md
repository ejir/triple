# GitHub Actions CI/CD 设置

## 概述

本文档描述了为Cosmopolitan Web Application项目添加的GitHub Actions CI/CD管道。

## 添加的内容

### 1. GitHub Actions 工作流 (`.github/workflows/ci.yml`)

使用Cosmopolitan工具链的综合CI/CD管道，包括：

#### 构建任务
- **build**: 使用Cosmopolitan Libc构建Actually Portable Executable (APE)
  - 自动安装和缓存Cosmopolitan工具链
  - 创建跨平台的`app.com`二进制文件
  - 上传构建制品供后续任务使用

#### 测试任务
- **test**: 使用Cosmopolitan构建运行完整的测试套件
  - 执行所有测试用例
  - 验证功能和兼容性

#### 质量检查任务
- **lint**: 使用Cosmopolitan编译器进行代码质量检查
  - 使用严格的编译器警告 (-Wall -Wextra -Werror)
  - 确保代码符合质量标准

- **integration**: 集成测试
  - 启动APE应用程序
  - 测试HTTP端点
  - 验证服务器功能

#### 发布任务
- **release**: 创建发布制品（仅在main分支或标签上）
  - 打包APE二进制文件
  - 生成SHA256校验和
  - 创建可发布的制品包

### 2. 文档
- `.github/workflows/README.md`: CI/CD管道的完整文档
- `CI_SETUP.md` (本文件): 概述和设置信息

### 3. 更新的.gitignore
- 添加了`app`二进制文件到忽略列表

## 特性

### 智能缓存
- Cosmopolitan工具链被缓存以加快后续构建
- 缓存键: `cosmo-toolchain-v1`
- 缓存位置: `/opt/cosmo`

### 制品管理
- **app-cosmopolitan-ape**: APE二进制文件（30天保留期）
- **integration-test-logs**: 集成测试的应用程序日志（7天保留期）
- **release-artifacts**: 带校验和的发布包（90天保留期）

### 触发条件
CI管道在以下情况下运行：
- 推送到 `main`、`develop` 或 `github-actions-ci-build-test` 分支
- 针对 `main` 或 `develop` 分支的拉取请求
- 手动工作流调度（可从GitHub UI手动触发）

### 跨平台支持
Cosmopolitan构建创建的Actually Portable Executable可在以下平台运行：
- Linux (x86_64)
- Windows
- macOS
- FreeBSD
- OpenBSD
- NetBSD

## 使用方法

### 查看CI结果
1. 转到GitHub仓库的"Actions"选项卡
2. 点击工作流运行以查看详细结果
3. 点击单个任务查看日志和制品

### 下载制品
1. 导航到完成的工作流运行
2. 滚动到底部的"Artifacts"部分
3. 点击制品名称下载

### 手动触发
1. 转到"Actions"选项卡
2. 选择"CI Build and Test"工作流
3. 点击"Run workflow"按钮
4. 选择分支并点击"Run workflow"

## 本地测试

在推送之前，您可以在本地测试相同的命令：

```bash
# Cosmopolitan构建和测试
make BUILD_MODE=cosmo
make BUILD_MODE=cosmo test

# 代码质量检查
make BUILD_MODE=cosmo CFLAGS="-Wall -Wextra -Werror -std=c11 -Isrc -Ithird_party/sqlite3"

# 清理构建制品
make clean
```

## CI/CD 管道流程

```
推送/PR
  ↓
┌─────────────────────────────────────┐
│  构建任务                            │
│  - build (Cosmopolitan)             │
└─────────────────────────────────────┘
  ↓
┌─────────────────────────────────────┐
│  测试和检查任务 (并行)               │
│  - test (需要: build)               │
│  - lint                             │
└─────────────────────────────────────┘
  ↓
┌─────────────────────────────────────┐
│  集成测试                            │
│  (需要: build, test)                │
└─────────────────────────────────────┘
  ↓
┌─────────────────────────────────────┐
│  发布 (如果是main/tag)               │
│  (需要: build, test)                │
└─────────────────────────────────────┘
```

## 自定义

### 更改保留期
编辑工作流文件中的`retention-days`值：
```yaml
retention-days: 7  # 更改为所需的天数
```

### 添加新测试
只需将新测试文件添加到`tests/`目录。Makefile会自动发现并构建它们。

### 修改构建标志
在Makefile中更新`CFLAGS`或通过命令行传递：
```yaml
make BUILD_MODE=cosmo CFLAGS="-O3 -march=native"
```

## 故障排除

### Cosmopolitan下载失败
如果Cosmopolitan工具链下载失败：
1. 检查 https://cosmo.zip 是否可访问
2. 更新工作流中的下载URL
3. 通过更改缓存键清除缓存

### 测试超时或失败
1. 在GitHub Actions UI中查看测试日志
2. 在本地运行相同的测试进行重现
3. 检查是否存在平台特定的问题

### 集成测试失败
1. 下载`integration-test-logs`制品
2. 查看应用程序日志中的错误
3. 确保端口8080未被阻止或占用

## 优势

1. **自动化质量保证**: 每次推送都会自动构建和测试
2. **跨平台验证**: 使用Cosmopolitan构建的APE经过测试
3. **早期错误检测**: 集成测试捕获运行时问题
4. **发布自动化**: 自动生成发布制品
5. **代码质量**: 严格的编译器警告强制执行代码质量
6. **完整文档**: 用于调试的详细日志和制品

## 为什么选择Cosmopolitan

- **真正的可移植性**: 一个二进制文件可在多个操作系统上运行
- **无依赖**: 静态链接，无需外部库
- **性能**: 优化的二进制文件，接近本地性能
- **简化部署**: 只需分发一个文件
- **CI/CD友好**: 缓存工具链以加快构建速度

## 下一步

考虑添加：
- 自动部署到staging/production环境
- 安全扫描 (SAST/DAST)
- 代码覆盖率报告
- 性能基准测试
- 自动发布说明生成
- 容器镜像构建和发布
- 多架构构建 (ARM等)

## 参考资料

- [GitHub Actions 文档](https://docs.github.com/zh/actions)
- [Cosmopolitan Libc](https://github.com/jart/cosmopolitan)
- [项目 README](README.md)
- [工作流文档](.github/workflows/README.md)
