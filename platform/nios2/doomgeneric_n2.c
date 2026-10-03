#include <stdint.h>
#include "doomgeneric.h"
#include "n2_platform.h"

#define DOOM_W 320
#define DOOM_H 200
#define OUT_W 320
#define OUT_H 240
#define FB_PITCH 1024

static void fb_write32(uint32_t byte_offset, uint32_t value)
{
    volatile void *addr =
        (volatile void *)(uintptr_t)(N2_PIXEL_BASE + byte_offset);
    __builtin_stwio(addr, value);
}

static void fb_write16(uint32_t byte_offset, uint16_t value)
{
    volatile void *addr =
        (volatile void *)(uintptr_t)(N2_PIXEL_BASE + byte_offset);
    __builtin_sthio(addr, value);
}

void DG_Init(void)
{
    int y, x;
    n2_platform_init();

    /* Clear the full 320x240 framebuffer once. */
    for (y = 0; y < OUT_H; ++y) {
        uint32_t row_offset = (uint32_t)y * FB_PITCH;
        for (x = 0; x < OUT_W; x += 2) {
            fb_write32(row_offset + (uint32_t)x * 2u, 0);
        }
    }
}

void DG_DrawFrame(void)
{
    const uint16_t *src = (const uint16_t *)DG_ScreenBuffer;
    int y, x;

    /* DOOM renders 320x200; the 20-pixel top/bottom bars stay black. */
    for (y = 0; y < DOOM_H; ++y) {
        uint32_t row_offset = (uint32_t)(y + 20) * FB_PITCH;
        const uint16_t *src_row = src + (uint32_t)y * DOOM_W;

        /* Two RGB565 pixels per 32-bit I/O store. */
        for (x = 0; x < DOOM_W; x += 2) {
            uint32_t packed =
                (uint32_t)src_row[x] |
                ((uint32_t)src_row[x + 1] << 16);
            fb_write32(row_offset + (uint32_t)x * 2u, packed);
        }
    }
}

void DG_SleepMs(uint32_t ms) { n2_sleep_ms(ms); }
uint32_t DG_GetTicksMs(void) { return n2_ticks_ms(); }
int DG_GetKey(int *pressed, unsigned char *key) { return n2_ps2_getkey(pressed, key); }
void DG_SetWindowTitle(const char *title) { (void)title; }
