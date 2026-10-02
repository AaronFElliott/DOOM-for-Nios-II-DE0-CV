#include <stdint.h>
#include "doomgeneric.h"
#include "n2_platform.h"

#define DOOM_W 320
#define DOOM_H 200
#define OUT_W 320
#define OUT_H 240
#define FB_PITCH 1024

static volatile uint16_t *const fb = (volatile uint16_t *)N2_PIXEL_BASE;

void DG_Init(void)
{
    int y, x;
    n2_platform_init();
    for (y = 0; y < OUT_H; ++y) {
        volatile uint16_t *row = (volatile uint16_t *)((volatile uint8_t *)fb + y * FB_PITCH);
        for (x = 0; x < OUT_W; ++x) row[x] = 0;
    }
}

void DG_DrawFrame(void)
{
    const uint16_t *src = (const uint16_t *)DG_ScreenBuffer;
    int y, x;
    for (y = 0; y < OUT_H; ++y) {
        volatile uint16_t *dst = (volatile uint16_t *)((volatile uint8_t *)fb + y * FB_PITCH);
        if (y < 20 || y >= 220) {
            for (x = 0; x < OUT_W; ++x) dst[x] = 0;
        } else {
            for (x = 0; x < OUT_W; ++x) dst[x] = src[(y - 20) * DOOM_W + x];
        }
    }
}

void DG_SleepMs(uint32_t ms) { n2_sleep_ms(ms); }
uint32_t DG_GetTicksMs(void) { return n2_ticks_ms(); }
int DG_GetKey(int *pressed, unsigned char *key) { return n2_ps2_getkey(pressed, key); }
void DG_SetWindowTitle(const char *title) { (void)title; }
