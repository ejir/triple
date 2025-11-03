#ifndef I18N_H
#define I18N_H

#include "http.h"

typedef enum {
    LANG_EN = 0,
    LANG_ZH_CN = 1
} language_t;

typedef struct {
    const char *key;
    const char *en;
    const char *zh_cn;
} translation_t;

language_t i18n_get_language(http_request_t *req);
const char *i18n_get(language_t lang, const char *key);
const char *i18n_get_lang_code(language_t lang);
const char *i18n_get_lang_name(language_t lang);

#endif
