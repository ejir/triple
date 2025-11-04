#ifndef KAOMOJI_H
#define KAOMOJI_H

typedef struct {
    const char *title;
    const char **items;
    int count;
} kaomoji_category_t;

const kaomoji_category_t *kaomoji_get_categories(void);
int kaomoji_get_categories_count(void);
void kaomoji_render_picker(char *buffer, int buffer_size, int *offset);

#endif
