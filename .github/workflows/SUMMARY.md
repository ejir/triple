# CI工作流总结

## 🎯 主要特性

本CI/CD管道**专门使用Cosmopolitan**工具链进行构建和测试。

### 构建方式
- ✅ **Cosmopolitan**: 生成Actually Portable Executable (APE)
- ❌ **GCC**: 已移除，不再使用

### 工作流任务

1. **build** - 使用Cosmopolitan构建APE
2. **test** - 使用Cosmopolitan运行测试
3. **lint** - 使用Cosmopolitan进行代码质量检查
4. **integration** - 使用APE进行集成测试
5. **release** - 创建APE发布制品

### 制品输出

- `app.com` - Actually Portable Executable
- 可在Linux、Windows、macOS、BSD等系统上运行

### 优势

- 🌍 真正的跨平台支持
- 📦 单一二进制文件
- 🚀 无外部依赖
- ⚡ 智能工具链缓存

## 快速命令

```bash
# 本地构建
make BUILD_MODE=cosmo

# 本地测试
make BUILD_MODE=cosmo test

# 清理
make clean
```
