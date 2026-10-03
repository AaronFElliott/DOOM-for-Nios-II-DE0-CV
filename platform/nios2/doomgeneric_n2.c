#include <stdint.h>
#include "doomgeneric.h"
#include "n2_platform.h"

#define DOOM_W 320
#define DOOM_H 200
#define OUT_W 320
#define OUT_H 240
#define FB_PITCH 1024

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

    for (y = 0; y < OUT_H; ++y) {
        uint32_t row_offset = (uint32_t)y * FB_PITCH;
        for (x = 0; x < OUT_W; ++x) {
            fb_write16(row_offset + (uint32_t)x * 2u, 0);
        }
    }
}

void DG_DrawFrame(void)
{
    const uint16_t *src = (const uint16_t *)DG_ScreenBuffer;
    int y, x;

    for (y = 0; y < OUT_H; ++y) {
        uint32_t row_offset = (uint32_t)y * FB_PITCH;

        if (y < 20 || y >= 220) {
            for (x = 0; x < OUT_W; ++x) {
                fb_write16(row_offset + (uint32_t)x * 2u, 0);
            }
        } else {
            const uint16_t *src_row =
                src + (uint32_t)(y - 20) * DOOM_W;

            for (x = 0; x < OUT_W; ++x) {
                fb_write16(row_offset + (uint32_t)x * 2u, src_row[x]);
            }
        }
    }
}

void DG_SleepMs(uint32_t ms) { n2_sleep_ms(ms); }
uint32_t DG_GetTicksMs(void) { return n2_ticks_ms(); }
int DG_GetKey(int *pressed, unsigned char *key) { return n2_ps2_getkey(pressed, key); }
void DG_SetWindowTitle(const char *title) { (void)title; }
