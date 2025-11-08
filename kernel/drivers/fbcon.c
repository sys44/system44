#include "fb.h"
#include "fbcon.h"
#include <stddef.h>

typedef struct {
    char ch;
    const char *rows[8];
} fontentry_t;

#include "fonts/basic8x8"

static const fontentry_t *font_get(char c, font_t font){
    switch(font){
        case FONT_BASIC8X8:
            for(size_t i = 0; i < sizeof(basic8x8)/sizeof(basic8x8[0]); i++)
                if(basic8x8[i].ch == c) return &basic8x8[i];
            break;
    }
    return NULL;
}

void fbcputchar(uint16_t px, uint16_t py, char c, uint32_t color, font_t font){
    const fontentry_t *f = font_get(c, font);
    if(!f) return;
    for(u8 y = 0; y < 8; y++){
        const char *r = f->rows[y];
        for(u8 x = 0; x < 8; x++){
            if(r[x] == '#') fb_putpixel(px + x, py + y, color);
        }
    }
}

void fbcstr(uint16_t x, uint16_t y, const char *s, uint32_t color, font_t font){
    while(*s){
        fbcputchar(x, y, *s, color, font);
        x += 6;
        s++;
    }
}
