#include "i18n.h"
#include <string.h>
#include <stdio.h>

static const translation_t translations[] = {
    {"message_boards", "Message Boards", "留言板"},
    {"board", "Board", "版块"},
    {"boards", "Boards", "版块列表"},
    {"threads", "Threads", "主题列表"},
    {"posts", "Posts", "帖子"},
    {"create_new_board", "Create New Board", "创建新版块"},
    {"create_new_thread", "Create New Thread", "发表新主题"},
    {"create_thread", "Create Thread", "发表主题"},
    {"create_board", "Create Board", "创建版块"},
    {"reply", "Reply", "回复"},
    {"post_reply", "Post Reply", "发表回复"},
    {"name", "Name", "名称"},
    {"title", "Title", "标题"},
    {"subject", "Subject", "主题"},
    {"description", "Description", "描述"},
    {"author", "Author", "作者"},
    {"content", "Content", "内容"},
    {"anonymous", "Anonymous", "匿名"},
    {"back_to_boards", "Back to boards", "返回版块列表"},
    {"back_to_site", "Back to Site", "返回网站"},
    {"back_to_board", "Back to board", "返回版块"},
    {"back_to_thread", "Back to thread", "返回主题"},
    {"view_board", "View Board", "查看版块"},
    {"view_thread", "View Thread", "查看主题"},
    {"kaomoji_picker", "Kaomoji Picker", "颜文字选择器"},
    {"quote", "Quote", "引用"},
    {"delete", "Delete", "删除"},
    {"admin", "Admin", "管理"},
    {"admin_dashboard", "Admin Dashboard", "管理面板"},
    {"admin_login", "Admin Login", "管理员登录"},
    {"login", "Login", "登录"},
    {"logout", "Logout", "退出登录"},
    {"username", "Username", "用户名"},
    {"password", "Password", "密码"},
    {"change_password", "Change Password", "修改密码"},
    {"new_password", "New Password", "新密码"},
    {"confirm_password", "Confirm Password", "确认密码"},
    {"access_denied", "Access Denied", "访问被拒绝"},
    {"login_required", "You must be logged in to access this page.", "您必须登录才能访问此页面。"},
    {"error", "Error", "错误"},
    {"success", "Success", "成功"},
    {"board_created", "Board Created!", "版块已创建！"},
    {"board_created_msg", "Board '%s' has been created.", "版块 '%s' 已创建。"},
    {"thread_created", "Thread Created!", "主题已创建！"},
    {"post_created", "Post Created!", "回复已发表！"},
    {"board_not_found", "Board Not Found", "版块未找到"},
    {"thread_not_found", "Thread Not Found", "主题未找到"},
    {"out_of_memory", "Out of memory", "内存不足"},
    {"no_form_data", "No form data", "没有表单数据"},
    {"required_fields", "Name and title are required", "名称和标题为必填项"},
    {"admin_only", "Only administrators can create boards.", "只有管理员可以创建版块。"},
    {"statistics", "Statistics", "统计信息"},
    {"total_boards", "Total Boards", "版块总数"},
    {"total_threads", "Total Threads", "主题总数"},
    {"total_posts", "Total Posts", "帖子总数"},
    {"recent_activity", "Recent Activity", "最近活动"},
    {"latest_threads", "Latest Threads", "最新主题"},
    {"login_failed", "Login Failed", "登录失败"},
    {"invalid_credentials", "Invalid username or password.", "用户名或密码错误。"},
    {"try_again", "Try Again", "重试"},
    {"logged_out", "Logged Out", "已退出登录"},
    {"logged_out_msg", "You have been successfully logged out.", "您已成功退出登录。"},
    {"login_again", "Login Again", "重新登录"},
    {"default_credentials", "Default credentials: admin / admin", "默认凭据：admin / admin"},
    {"already_logged_in", "Already logged in. Redirecting...", "已登录。正在跳转..."},
    {"login_success", "Login successful! Redirecting...", "登录成功！正在跳转..."},
    {"password_changed", "Password Changed!", "密码已修改！"},
    {"password_changed_msg", "Your password has been changed successfully.", "您的密码已成功修改。"},
    {"password_mismatch", "Passwords do not match.", "密码不匹配。"},
    {"old_password", "Old Password", "旧密码"},
    {"all_boards", "All Boards", "所有版块"},
    {"kaomoji", "Kaomoji", "颜文字"},
    {"common", "Common", "常用"},
    {"hide", "Hide", "躲"},
    {"fist", "Fist", "拳"},
    {"other", "Other", "其他"},
    {"language", "Language", "语言"},
    {"english", "English", "英文"},
    {"chinese", "中文（简体）", "中文（简体）"},
    {NULL, NULL, NULL}
};

static char *get_cookie_value(const char *cookies, const char *name) {
    if (!cookies || !name) {
        return NULL;
    }
    
    static char value[64];
    char search_name[64];
    snprintf(search_name, sizeof(search_name), "%s=", name);
    
    const char *start = strstr(cookies, search_name);
    if (!start) {
        return NULL;
    }
    
    start += strlen(search_name);
    const char *end = strchr(start, ';');
    size_t len;
    
    if (end) {
        len = end - start;
    } else {
        len = strlen(start);
    }
    
    if (len >= sizeof(value)) {
        len = sizeof(value) - 1;
    }
    
    memcpy(value, start, len);
    value[len] = '\0';
    
    return value;
}

language_t i18n_get_language(http_request_t *req) {
    if (req->query_string) {
        if (strstr(req->query_string, "lang=zh") || strstr(req->query_string, "lang=zh-cn")) {
            return LANG_ZH_CN;
        }
        if (strstr(req->query_string, "lang=en")) {
            return LANG_EN;
        }
    }
    
    if (req->cookies) {
        char *lang = get_cookie_value(req->cookies, "lang");
        if (lang) {
            if (strcmp(lang, "zh-cn") == 0 || strcmp(lang, "zh") == 0) {
                return LANG_ZH_CN;
            }
            if (strcmp(lang, "en") == 0) {
                return LANG_EN;
            }
        }
    }
    
    return LANG_EN;
}

const char *i18n_get(language_t lang, const char *key) {
    if (!key) {
        return "";
    }
    
    for (int i = 0; translations[i].key != NULL; i++) {
        if (strcmp(translations[i].key, key) == 0) {
            if (lang == LANG_ZH_CN) {
                return translations[i].zh_cn;
            } else {
                return translations[i].en;
            }
        }
    }
    
    return key;
}

const char *i18n_get_lang_code(language_t lang) {
    switch (lang) {
        case LANG_ZH_CN:
            return "zh-cn";
        case LANG_EN:
        default:
            return "en";
    }
}

const char *i18n_get_lang_name(language_t lang) {
    switch (lang) {
        case LANG_ZH_CN:
            return "中文";
        case LANG_EN:
        default:
            return "English";
    }
}
