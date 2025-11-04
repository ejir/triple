#include "kaomoji.h"
#include <stdio.h>

static const char *kaomoji_common[] = {
    "(ﾟ∀。)"
};

static const char *kaomoji_hide[] = {
    "|∀ﾟ", "|∀`)", "|д`)", "|дﾟ)", "|ω・´)", "|ー`)", "|-`)"
};

static const char *kaomoji_fist[] = {
    "⊂彡☆))д´)", "⊂彡☆))д`)", "⊂彡☆))∀`)", "(´∀((☆ミつ"
};

static const char *kaomoji_a[] = {
    "(ﾟ∀。)", "(*ﾟ∀ﾟ*)", "(ﾟ∀ﾟ)", "(ノﾟ∀ﾟ)ノ", "(σﾟ∀ﾟ)σ",
    "σ`∀´)", "(*´∀`)", "(´∀`)", "(ゝ∀･)", "(・∀・)",
    "(｡◕∀◕｡)", "(〃∀〃)"
};

static const char *kaomoji_d[] = {
    "(ﾟдﾟ)", "(´ﾟДﾟ`)", "(|||ﾟдﾟ)", "Σ(ﾟдﾟ)", "(((ﾟдﾟ)))",
    "(ﾟДﾟ≡ﾟДﾟ)", "(д)ﾟﾟ", "(☉д⊙)", "(;ﾟдﾟ)", "(σﾟдﾟ)σ",
    "(╬ﾟдﾟ)", "(`д´)", "(つд⊂)", "(>д<)", "(TдT)",
    "(-д-)", "(´д`)", "(*´д`)", "(;´Д`)", "･ﾟ(ﾉд`ﾟ)",
    "ﾟ(つд`ﾟ)"
};

static const char *kaomoji_w[] = {
    "(=ﾟωﾟ)=", "(ﾟωﾟ)", "(oﾟωﾟo)", "(*´ω`*)", "ヾ(´ωﾟ｀)",
    "( ^ω^)", "(・ω・)", "(｀･ω･)", "(`・ω・´)", "(´・ω・`)",
    "(´・ω)", "(｀・ω)", "（<ゝω・）☆"
};

static const char *kaomoji_dash[] = {
    "(・_ゝ・)", "(´_ゝ`)", "(´_っ`)", "(`_っ´)", "(´ー`)",
    "(`ー´)", "(*ﾟーﾟ)", "(・ー・)"
};

static const char *kaomoji_e[] = {
    "(ﾟ3ﾟ)", "(`ε´)", "ヾ(´ε`ヾ)", "(`ε´(つ*⊂)"
};

static const char *kaomoji_other[] = {
    "(＾o＾)ﾉ", "(`ヮ´)", "(´ρ`)", "(`・´)", "(*ﾟ∇ﾟ)",
    "ﾟÅﾟ)", "/(◕‿‿◕)\\"
};

static const kaomoji_category_t kaomoji_categories[] = {
    {"常用", kaomoji_common, sizeof(kaomoji_common) / sizeof(kaomoji_common[0])},
    {"躲", kaomoji_hide, sizeof(kaomoji_hide) / sizeof(kaomoji_hide[0])},
    {"拳", kaomoji_fist, sizeof(kaomoji_fist) / sizeof(kaomoji_fist[0])},
    {"∀", kaomoji_a, sizeof(kaomoji_a) / sizeof(kaomoji_a[0])},
    {"д", kaomoji_d, sizeof(kaomoji_d) / sizeof(kaomoji_d[0])},
    {"ω", kaomoji_w, sizeof(kaomoji_w) / sizeof(kaomoji_w[0])},
    {"ー", kaomoji_dash, sizeof(kaomoji_dash) / sizeof(kaomoji_dash[0])},
    {"ε", kaomoji_e, sizeof(kaomoji_e) / sizeof(kaomoji_e[0])},
    {"其他", kaomoji_other, sizeof(kaomoji_other) / sizeof(kaomoji_other[0])}
};

const kaomoji_category_t *kaomoji_get_categories(void) {
    return kaomoji_categories;
}

int kaomoji_get_categories_count(void) {
    return sizeof(kaomoji_categories) / sizeof(kaomoji_categories[0]);
}

void kaomoji_render_picker(char *buffer, int buffer_size, int *offset) {
    int len = *offset;
    
    len += snprintf(buffer + len, buffer_size - len,
        "<div id=\"kaomojiPicker\" style=\"display:none;position:fixed;top:50%%;left:50%%;transform:translate(-50%%,-50%%);"
        "background:white;border-radius:8px;box-shadow:0 8px 16px rgba(0,0,0,0.2);padding:20px;max-width:500px;"
        "width:90%%;max-height:80vh;overflow-y:auto;z-index:1000;\">\n"
        "<div style=\"display:flex;justify-content:space-between;align-items:center;margin-bottom:16px;\">\n"
        "<h3 style=\"margin:0;color:var(--primary);\">选择颜文字</h3>\n"
        "<button onclick=\"toggleKaomoji()\" style=\"background:none;border:none;font-size:24px;cursor:pointer;color:var(--text-secondary);\">×</button>\n"
        "</div>\n");
    
    int categories_count = kaomoji_get_categories_count();
    const kaomoji_category_t *categories = kaomoji_get_categories();
    
    for (int i = 0; i < categories_count; i++) {
        len += snprintf(buffer + len, buffer_size - len,
            "<div style=\"margin-bottom:16px;\">\n"
            "<h4 style=\"color:var(--text-secondary);margin-bottom:8px;font-size:0.875rem;\">%s</h4>\n"
            "<div style=\"display:flex;flex-wrap:wrap;gap:8px;\">\n",
            categories[i].title);
        
        for (int j = 0; j < categories[i].count; j++) {
            len += snprintf(buffer + len, buffer_size - len,
                "<button type=\"button\" onclick=\"insertKaomoji('%s')\" "
                "style=\"padding:8px 12px;border:1px solid var(--divider);border-radius:4px;"
                "background:var(--surface);cursor:pointer;transition:all 0.2s;font-size:1rem;\">"
                "%s</button>\n",
                categories[i].items[j], categories[i].items[j]);
        }
        
        len += snprintf(buffer + len, buffer_size - len, "</div></div>\n");
    }
    
    len += snprintf(buffer + len, buffer_size - len,
        "</div>\n"
        "<div id=\"kaomojiOverlay\" onclick=\"toggleKaomoji()\" style=\"display:none;position:fixed;top:0;left:0;"
        "width:100%%;height:100%%;background:rgba(0,0,0,0.5);z-index:999;\"></div>\n"
        "<script>\n"
        "function toggleKaomoji() {\n"
        "  var picker = document.getElementById('kaomojiPicker');\n"
        "  var overlay = document.getElementById('kaomojiOverlay');\n"
        "  if (picker.style.display === 'none') {\n"
        "    picker.style.display = 'block';\n"
        "    overlay.style.display = 'block';\n"
        "  } else {\n"
        "    picker.style.display = 'none';\n"
        "    overlay.style.display = 'none';\n"
        "  }\n"
        "}\n"
        "function insertKaomoji(text) {\n"
        "  var textarea = document.querySelector('textarea[name=\"content\"]');\n"
        "  if (textarea) {\n"
        "    var start = textarea.selectionStart;\n"
        "    var end = textarea.selectionEnd;\n"
        "    var value = textarea.value;\n"
        "    textarea.value = value.substring(0, start) + text + value.substring(end);\n"
        "    textarea.selectionStart = textarea.selectionEnd = start + text.length;\n"
        "    textarea.focus();\n"
        "  }\n"
        "  toggleKaomoji();\n"
        "}\n"
        "</script>\n");
    
    *offset = len;
}
