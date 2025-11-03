# 版块详情页国际化 (Board Details Page Internationalization)

## 更新日期 (Update Date)
2024-11-03

## 变更概述 (Change Summary)

完成了版块详情页（Board Details Page）的完整中文化，用户现在可以在英文和中文之间无缝切换。

Completed full internationalization of the Board Details Page, allowing users to seamlessly switch between English and Chinese.

## 实现的功能 (Implemented Features)

### 1. 语言检测 (Language Detection)
- ✅ 从URL参数获取语言偏好 (`?lang=zh-cn` 或 `?lang=en`)
- ✅ 从Cookie读取持久化的语言设置
- ✅ 默认英文，支持中文切换

### 2. 页面元素本地化 (Localized Page Elements)

#### 页面头部 (Page Header)
```
English: /general/ - General Discussion
Chinese: /general/ - General Discussion
```

#### 导航链接 (Navigation Links)
```
English: ← Back to boards
Chinese: ← 返回版块列表
```

#### 主题列表标题 (Thread List Header)
```
English: 💬 Threads
Chinese: 💬 主题列表
```

#### 创建主题表单 (Create Thread Form)

**表单标题 (Form Title)**
```
English: ✏️ Create New Thread
Chinese: ✏️ 发表新主题
```

**表单字段 (Form Fields)**
```
Subject:
  English: Subject
  Chinese: 主题

Name:
  English: Name
  Chinese: 名称
  
Content:
  English: Content
  Chinese: 内容

Anonymous placeholder:
  English: Anonymous
  Chinese: 匿名

Submit button:
  English: CREATE THREAD
  Chinese: 发表主题

Kaomoji button:
  English: 😊 Kaomoji
  Chinese: 😊 颜文字
```

### 3. 语言切换器 (Language Switcher)
- ✅ 位于页面右上角的语言切换按钮
- ✅ 当前语言高亮显示（白色半透明背景）
- ✅ 点击后自动保存Cookie并刷新页面
- ✅ 保持在当前版块页面

```javascript
function setLanguage(lang) {
  document.cookie = 'lang=' + lang + '; path=/; max-age=31536000';
  window.location.href = window.location.pathname + '?id=' + board_id + '&lang=' + lang;
}
```

## 技术实现 (Technical Implementation)

### 代码修改 (Code Changes)

**文件**: `src/board.c`  
**函数**: `board_view_handler()`

#### 1. 添加语言检测
```c
language_t lang = i18n_get_language(req);
```

#### 2. 错误消息国际化
```c
char error_html[512];
snprintf(error_html, sizeof(error_html),
    "<html><body><h1>%s</h1><a href=\"/\">%s</a></body></html>",
    i18n_get(lang, "board_not_found"),
    i18n_get(lang, "back_to_boards"));
```

#### 3. 表单字段国际化
```c
len += snprintf(html + len, 65536 - len,
    "<h2>✏️ %s</h2>\n"
    "...\n"
    "<label>%s</label>\n"
    "<input type=\"text\" name=\"subject\" required>\n"
    "...\n"
    "<label>%s</label>\n"
    "<input type=\"text\" name=\"author\" placeholder=\"%s\">\n"
    "...\n"
    "<label>%s</label>\n"
    "<textarea name=\"content\" required></textarea>\n",
    i18n_get(lang, "create_new_thread"),
    i18n_get(lang, "subject"),
    i18n_get(lang, "name"),
    i18n_get(lang, "anonymous"),
    i18n_get(lang, "content"));
```

### 增加的缓冲区大小 (Increased Buffer Size)
```c
char *html = malloc(65536);  // 从 32768 增加到 65536
```

为了容纳额外的语言切换器HTML和国际化内容，增加了HTML缓冲区大小。

## 测试结果 (Test Results)

### 英文测试 (English Test)
```bash
curl -s "http://localhost:8080/board?id=1" | grep -E "(Create New Thread|Subject|Name|Content)"
```
**输出 (Output)**:
```html
<h2>✏️ Create New Thread</h2>
<label>Subject</label>
<label>Name</label>
<label>Content</label>
```

### 中文测试 (Chinese Test)
```bash
curl -s "http://localhost:8080/board?id=1&lang=zh-cn" | grep -E "(发表新主题|主题|名称|内容)"
```
**输出 (Output)**:
```html
<h2>💬 主题列表</h2>
<h2>✏️ 发表新主题</h2>
<label>主题</label>
<label>名称</label>
<label>内容</label>
```

### Placeholder测试 (Placeholder Test)
```bash
# English
curl -s "http://localhost:8080/board?id=1" | grep -o 'placeholder="[^"]*"'
# Output: placeholder="Anonymous"

# Chinese
curl -s "http://localhost:8080/board?id=1&lang=zh-cn" | grep -o 'placeholder="[^"]*"'
# Output: placeholder="匿名"
```

## 用户体验 (User Experience)

### 语言切换流程 (Language Switching Flow)
1. 用户访问版块页面（默认英文）
2. 点击右上角 "中文" 按钮
3. Cookie自动保存语言偏好
4. 页面刷新，显示中文界面
5. 所有后续页面自动使用中文

### 持久化 (Persistence)
- Cookie有效期：1年
- Cookie路径：站点根目录 (`/`)
- 跨页面保持语言设置

## 相关文件 (Related Files)
- `src/board.c` - 版块处理器实现
- `src/i18n.c` - 国际化翻译数据
- `src/i18n.h` - 国际化API定义
- `LOCALIZATION.md` - 详细本地化文档
- `I18N_FEATURE_SUMMARY.md` - 功能总结

## 下一步 (Next Steps)
- [ ] 主题详情页（Thread View）本地化
- [ ] 回复表单本地化
- [ ] 管理面板本地化
- [ ] 错误页面本地化

---

**状态 (Status)**: ✅ 完成 (Completed)  
**测试状态 (Test Status)**: ✅ 通过 (Passed)
