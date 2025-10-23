#include "fb.h"

fb_info_t fb;
static inline u16 rgb565(u8 r, u8 g, u8 b){
    return (u16)(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3));
}


void fb_init(u8 *vbe){
    if(!vbe) return;
    u16 w   = *(u16*)(vbe + 0x12);
    u16 h   = *(u16*)(vbe + 0x14);
    u16 p   = *(u16*)(vbe + 0x10);
    u8  bpp = *(u8* )(vbe + 0x19);
    u32 phys= *(u32*)(vbe + 0x28);
    if(!w || !h) return;
    if(!bpp) bpp = 32;
    if(!p) p = (u32)w * (bpp / 8);
    fb.addr   = (u8*)(unsigned long)phys;
    fb.width  = w;
    fb.height = h;
    fb.pitch  = p;
    fb.bpp    = bpp;
}

void fb_fill(u32 color){
    if(!fb.addr) return;
    u32 y, x;
    u32 bpp_bytes = fb.bpp / 8;
    for(y = 0; y < fb.height; ++y){
        u8 *row = fb.addr + (unsigned long)y * fb.pitch;
        if(bpp_bytes == 4){
            volatile u32 *r32 = (volatile u32*)row;
            for(x = 0; x < fb.width; ++x) r32[x] = color;
        } else if(bpp_bytes == 3){
            for(x = 0; x < fb.width; ++x){
                u8 *px = row + x*3;
                px[0] = (u8)( color        & 0xFF);
                px[1] = (u8)((color >> 8)  & 0xFF);
                px[2] = (u8)((color >> 16) & 0xFF);
            }
        } else if(bpp_bytes == 2){
            u16 c16 = rgb565((u8)((color>>16)&0xFF),(u8)((color>>8)&0xFF),(u8)(color&0xFF));
            for(x = 0; x < fb.width; ++x){
                volatile u16 *p16 = (volatile u16*)(row + x*2);
                *p16 = c16;
            }
        } else {
            for(x = 0; x < fb.width; ++x) row[x] = (u8)(color & 0xFF);
        }
    }
}

void fb_putpixel(u32 x, u32 y, u32 c){
    if(!fb.addr) return;
    if(x >= fb.width || y >= fb.height) return;
    u8 *row = fb.addr + (unsigned long)y * fb.pitch;
    u32 bpp_bytes = fb.bpp / 8;
    if(bpp_bytes == 4){
        volatile u32 *p32 = (volatile u32*)(row + x*4);
        *p32 = c;
    } else if(bpp_bytes == 3){
        u8 *px = row + x*3;
        px[0] = (u8)( c        & 0xFF);
        px[1] = (u8)((c >> 8) & 0xFF);
        px[2] = (u8)((c >>16) & 0xFF);
    } else if(bpp_bytes == 2){
        u16 v = rgb565((u8)((c>>16)&0xFF),(u8)((c>>8)&0xFF),(u8)(c&0xFF));
        volatile u16 *p16 = (volatile u16*)(row + x*2);
        *p16 = v;
    } else {
        row[x] = (u8)(c & 0xFF);
    }
}
