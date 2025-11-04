# 本地化/国际化功能 (Localization/Internationalization)

## 概述 (Overview)

本应用现在支持多语言界面，目前支持：
- 英文 (English)
- 简体中文 (Simplified Chinese)

The application now supports multilingual interfaces, currently supporting:
- English
- Simplified Chinese (简体中文)

## 语言切换 (Language Switching)

### 方式一：通过界面切换 (Method 1: UI Language Switcher)

在页面右上角有语言切换按钮：
- **English** - 切换到英文
- **中文** - 切换到简体中文

Language switcher buttons are available in the top-right corner:
- **English** - Switch to English
- **中文** - Switch to Simplified Chinese

### 方式二：通过URL参数 (Method 2: URL Parameter)

在任何页面的URL后添加 `?lang=zh-cn` 或 `?lang=en`:
- `http://localhost:8080/?lang=zh-cn` - 切换到中文
- `http://localhost:8080/?lang=en` - 切换到英文

Add `?lang=zh-cn` or `?lang=en` to any page URL:
- `http://localhost:8080/?lang=zh-cn` - Switch to Chinese
- `http://localhost:8080/?lang=en` - Switch to English

### 方式三：通过Cookie (Method 3: Cookie)

语言选择会自动保存在Cookie中，一年有效。
一旦设置，您的语言偏好会在所有页面保持。

Language preference is automatically saved in a cookie (valid for 1 year).
Once set, your language preference persists across all pages.

## 技术实现 (Technical Implementation)

### 文件结构 (File Structure)

- **src/i18n.h** - 国际化头文件 (Internationalization header)
- **src/i18n.c** - 国际化实现，包含所有翻译字符串 (Implementation with all translations)

### 添加新翻译 (Adding New Translations)

要添加新的翻译字符串，在 `src/i18n.c` 的 `translations` 数组中添加条目：

To add new translation strings, add entries to the `translations` array in `src/i18n.c`:

```c
{"key_name", "English Text", "中文文本"},
```

### 在代码中使用 (Using in Code)

```c
#include "i18n.h"

// 获取用户语言偏好 (Get user's language preference)
language_t lang = i18n_get_language(req);

// 获取翻译文本 (Get translated text)
const char *text = i18n_get(lang, "key_name");
```

### 支持的键值 (Supported Keys)

当前支持的翻译键包括 (Currently supported translation keys include):

- **页面标题**: `message_boards`, `board`, `threads`, etc.
- **按钮**: `create_board`, `create_thread`, `reply`, `login`, `logout`, etc.
- **表单标签**: `name`, `title`, `subject`, `author`, `content`, etc.
- **消息**: `success`, `error`, `access_denied`, etc.
- **导航**: `back_to_boards`, `back_to_site`, etc.

完整列表请参阅 `src/i18n.c`。
For a complete list, see `src/i18n.c`.

## 浏览器语言检测 (Browser Language Detection)

当前版本通过以下优先级确定语言：
1. URL参数 (`?lang=xx`)
2. Cookie (`lang=xx`)
3. 默认英文

The current version determines language by priority:
1. URL parameter (`?lang=xx`)
2. Cookie (`lang=xx`)
3. Default to English

## 未来改进 (Future Improvements)

- [ ] 从HTTP Accept-Language头自动检测浏览器语言
- [ ] 添加更多语言支持（日语、韩语、繁体中文等）
- [ ] 用户配置文件中的语言设置
- [ ] 导出/导入翻译文件（JSON/YAML格式）
- [ ] 翻译覆盖率统计工具

- [ ] Auto-detect browser language from HTTP Accept-Language header
- [ ] Add more language support (Japanese, Korean, Traditional Chinese, etc.)
- [ ] Language setting in user profiles
- [ ] Export/import translation files (JSON/YAML format)
- [ ] Translation coverage statistics tool

## 已本地化页面 (Localized Pages)

目前已完成本地化的页面：
- ✅ 首页/版块列表 (Home/Board List)
- ✅ 版块详情页 (Board Details) - 包括表单、导航和标题
- ✅ 主题详情页 (Thread Details) - 包括回复表单、导航和Kaomoji选择器
- ✅ 管理面板 (Admin Panel) - 包括登录、仪表板和管理功能
- ✅ Kaomoji选择器 (Kaomoji Picker) - 所有分类标签

Currently localized pages:
- ✅ Home/Board List
- ✅ Board Details - Including forms, navigation and headers
- ✅ Thread Details - Including reply forms, navigation and kaomoji picker
- ✅ Admin Panel - Including login, dashboard and management features
- ✅ Kaomoji Picker - All category labels

## 字体支持 (Font Support)

为了更好地显示中文，CSS中添加了中文字体：
- Microsoft YaHei (微软雅黑)
- SimHei (黑体)

For better Chinese display, Chinese fonts are added in CSS:
- Microsoft YaHei
- SimHei
