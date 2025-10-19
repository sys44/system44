#include "fb.h"
#include <stddef.h>

typedef struct {
    char ch;
    const char *rows[8];
} fontentry_t;

#include "fonts/basic8x8"

static const fontentry_t *font_get(char c){
    for(size_t i = 0; i < sizeof(ftable)/sizeof(ftable[0]); i++)
        if(ftable[i].ch == c) return &ftable[i];
    return NULL;
}

void fbcputchar(u16 px, u16 py, char c, u32 color){
    const fontentry_t *f = font_get(c);
    if(!f) return;
    for(u8 y = 0; y < 8; y++){
        const char *r = f->rows[y];
        for(u8 x = 0; x < 8; x++){
            if(r[x] == '#') fb_putpixel(px + x, py + y, color);
        }
    }
}

void fbcstr(u16 px, u16 py, const char *s, u32 color){
    while(*s){
        fbcputchar(px, py, *s, color);
        px += 8;
        s++;
    }
}
