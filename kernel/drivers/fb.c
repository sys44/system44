#include "fb.h"
fb_info_t fb;
#include <stdint.h>
#include "../mm/pmm.h"
#include "../fs/kfs.h"
#include "../fs/kfs.h"
#include "fb.h"
#include "../drivers/ata.h"
#include <stddef.h>
static inline uint16_t rgb565(uint8_t r,uint8_t g,uint8_t b){
    return (uint16_t)(((b & 0xF8)<<8)|((g & 0xFC)<<3)|(r>>3));
}

static int kfsExtension0(const struct kfs_file* f, uint32_t offset, uint8_t* buf, uint32_t size){
    if(offset >= f->size) return 0;
    if(offset + size > f->size) size = f->size - offset;
    uint32_t sector = f->start_lba + offset / 512;
    uint32_t sector_offset = offset % 512;
    uint32_t remaining = size;
    uint8_t temp[512];
    uint8_t* p = buf;
    while(remaining){
        if(ata_read_sectors(sector,1,temp) < 0) return -1;
        uint32_t copy_len = 512 - sector_offset;
        if(copy_len > remaining) copy_len = remaining;
        for(uint32_t i=0;i<copy_len;i++) p[i] = temp[sector_offset + i];
        remaining -= copy_len;
        p += copy_len;
        sector++;
        sector_offset = 0;
    }
    return 0;
}

#pragma pack(push,1)
typedef struct {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
} bmp_file_header_t;

typedef struct {
    uint32_t biSize;
    int32_t  biWidth;
    int32_t  biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t  biXPelsPerMeter;
    int32_t  biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} bmp_info_header_t;
#pragma pack(pop)

int fblogo(const char* filename, int start_x, int start_y) {
    struct kfs_file* f = kfs_find(filename);
    if(!f) return -1;
    bmp_file_header_t fh;
    bmp_info_header_t ih;
    if(kfsExtension0(f, 0, (uint8_t*)&fh, sizeof(fh)) < 0) return -2;
    if(kfsExtension0(f, sizeof(fh), (uint8_t*)&ih, sizeof(ih)) < 0) return -2;
    if(fh.bfType != 0x4D42) return -3;
    if(ih.biBitCount != 24 || ih.biCompression != 0) return -4;
    int w = ih.biWidth;
    int h = ih.biHeight;
    int row_size = (w * 3 + 3) & ~3;
    size_t img_size = (size_t)row_size * h;
    uint32_t pages = (img_size + 4095) / 4096;
    uint8_t* img_buf = (uint8_t*)pmm_alloc_pages(pages);
    if(!img_buf) return -6;
    if(kfsExtension0(f, fh.bfOffBits, img_buf, img_size) < 0) {
        pmm_free_pages(img_buf, pages);
        return -5;
    }
    uint8_t* dst = fb.addr + start_y * fb.pitch + start_x * 3;
    int max_w = fb.width  - start_x;
    int max_h = fb.height - start_y;
    for(int y = 0; y < h && y < max_h; y++) {
        uint8_t* src_row = img_buf + (h - 1 - y) * row_size;
        uint8_t* dst_row = dst + y * fb.pitch;
        uint8_t* s = src_row;
        uint8_t* d = dst_row;
        int limit = (w < max_w) ? w : max_w;
        for(int x = 0; x < limit; x++) {
            *d++ = *s++;
            *d++ = *s++;
            *d++ = *s++;
        }
    }
    pmm_free_pages(img_buf, pages);
    return 0;
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


void fb_rect(u32 x1, u32 y1, u32 x2, u32 y2, u32 colour) {
    for (u32 i = 0; i < x2 - x1; i++) {
        for (u32 j = 0; j < y2 - y1; j++) {
            fb_putpixel(x1 + i, y1 + j, colour);
        }
    }
}
