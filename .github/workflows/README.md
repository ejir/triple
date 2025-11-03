# GitHub Actions CI/CD

此目录包含用于持续集成和部署的GitHub Actions工作流文件。

## 工作流

### CI 构建和测试 (`ci.yml`)

使用Cosmopolitan工具链的自动化构建和测试管道，在每次推送和拉取请求时运行。

#### 任务

1. **build** - 使用Cosmopolitan构建
   - 使用Cosmopolitan Libc工具链
   - 创建Actually Portable Executable（APE）：`app.com`
   - 缓存工具链以加快后续构建
   - 生成跨平台二进制文件

2. **test** - 运行测试
   - 使用Cosmopolitan构建执行所有测试套件
   - 验证跨平台兼容性
   - 确保所有测试通过

3. **lint** - 代码质量检查
   - 使用严格的警告进行编译（`-Wall -Wextra -Werror`）
   - 使用Cosmopolitan编译器检查代码质量
   - 确保代码符合质量标准

4. **integration** - 集成测试
   - 启动APE应用程序
   - 测试HTTP端点
   - 验证服务器功能
   - 收集应用程序日志

5. **release** - 创建发布制品
   - 仅在main分支或标签上运行
   - 打包Cosmopolitan APE构建
   - 生成校验和
   - 创建可发布的制品

## 触发条件

CI管道在以下情况下运行：
- 推送到 `main`、`develop` 或 `github-actions-ci-build-test` 分支
- 针对 `main` 或 `develop` 分支的拉取请求
- 手动工作流调度（可从GitHub UI手动触发）

## 制品

工作流生成以下制品：

- **app-cosmopolitan-ape** - Actually Portable Executable（30天保留期）
- **integration-test-logs** - 集成测试的应用程序日志（7天保留期）
- **release-artifacts** - 带校验和的发布包（90天保留期）

## 缓存

工作流缓存Cosmopolitan工具链以加快构建速度：
- 缓存键：`cosmo-toolchain-v1`
- 位置：`/opt/cosmo`

## 本地测试

要在本地测试工作流，您可以运行相同的命令：

### Cosmopolitan 构建和测试
```bash
make BUILD_MODE=cosmo
make BUILD_MODE=cosmo test
```

### 代码质量检查
```bash
make BUILD_MODE=cosmo CFLAGS="-Wall -Wextra -Werror -std=c11 -Isrc -Ithird_party/sqlite3"
```

### 清理构建制品
```bash
make clean
```

## 自定义

### 更改构建配置

编辑工作流文件以修改：
- 编译器标志
- 测试超时
- 制品保留期
- 分支触发器

### 添加新测试

测试会从 `tests/` 目录自动发现并由Makefile构建。

### 修改发布流程

可以自定义发布任务以：
- 自动创建GitHub发布
- 部署到服务器
- 发布到包注册表
- 生成发布说明

## 故障排除

### Cosmopolitan安装失败

如果Cosmopolitan工具链下载失败：
1. 检查 https://cosmo.zip/pub/cosmos/bin/ 是否可访问
2. 更新工作流中的下载URL
3. 通过更改缓存键使缓存失效

### 测试失败

在Actions选项卡中检查测试日志：
1. 点击失败的任务
2. 展开"Build and run tests"步骤
3. 查看错误消息

### 集成测试超时

如果应用程序未及时启动：
1. 增加"Wait for application to start"步骤中的休眠时间
2. 检查应用程序日志制品
3. 验证端口8080未被占用

## 最佳实践

1. **始终在本地运行测试** 然后再推送
2. **检查制品** 以确保构建正确
3. **监控缓存使用** 以优化构建时间
4. **定期更新依赖项** 在工作流中
5. **保持工作流简单** 和可维护

## 跨平台支持

Cosmopolitan构建创建的Actually Portable Executable可以在以下平台上运行：
- Linux (x86_64)
- Windows
- macOS
- FreeBSD
- OpenBSD
- NetBSD

## 参考资料

- [GitHub Actions 文档](https://docs.github.com/zh/actions)
- [Cosmopolitan Libc](https://github.com/jart/cosmopolitan)
- [项目构建文档](../../README.md)
