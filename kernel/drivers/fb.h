#pragma once
typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;
typedef struct {
    u8  *addr;
    u32  width;
    u32  height;
    u32  pitch;
    u8   bpp;
} fb_info_t;
extern fb_info_t fb;
void fb_init(u8 *vbe_modeinfo);
void fb_fill(u32 color);
void fb_putpixel(u32 x, u32 y, u32 c);
int fblogo(const char* filename, int start_x, int start_y);
