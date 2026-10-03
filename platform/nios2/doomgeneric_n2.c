#include <stdint.h>
#include <string.h>
#include "io.h"
#include "doomgeneric.h"
#include "n2_platform.h"

#define DOOM_W 320
#define DOOM_H 200
#define OUT_W 320
#define OUT_H 240
#define FB_PITCH 1024

#ifdef N2_TARGET_DE0CV

static void fb_write32(uint32_t byte_offset, uint32_t value)
{
    volatile void *addr =
        (volatile void *)(uintptr_t)(N2_PIXEL_BASE + byte_offset);
    __builtin_stwio(addr, value);
}

#else

/*
 * CPUlator-only framebuffer path: N2_PIXEL_BASE points into ordinary SDRAM.
 * The VGA controller is redirected to this buffer in DG_Init(), so these
 * normal memory stores are much cheaper to simulate than per-pixel I/O.
 */
static uint32_t *const fb = (uint32_t *)(uintptr_t)N2_PIXEL_BASE;

#endif

void DG_Init(void)
{
    int y, x;
    n2_platform_init();

#ifndef N2_TARGET_DE0CV
    /*
     * Point both VGA buffer pointers at the SDRAM framebuffer. Direct writes
     * to the Buffer/Backbuffer registers set their addresses; a swap is not
     * needed because both pointers are initialized to the same buffer.
     */
    IOWR_32DIRECT(N2_PIXEL_CTRL_BASE, 0, N2_PIXEL_BASE);
    IOWR_32DIRECT(N2_PIXEL_CTRL_BASE, 4, N2_PIXEL_BASE);
#endif

#ifdef N2_TARGET_DE0CV
    /* Clear the full on-chip DE0-CV framebuffer once. */
    for (y = 0; y < OUT_H; ++y) {
        uint32_t row_offset = (uint32_t)y * FB_PITCH;
        for (x = 0; x < OUT_W; x += 2) {
            fb_write32(row_offset + (uint32_t)x * 2u, 0);
        }
    }
#else
    /* Clear the full simulator framebuffer in ordinary SDRAM. */
    memset((void *)(uintptr_t)N2_PIXEL_BASE, 0, OUT_H * FB_PITCH);
#endif
}

void DG_DrawFrame(void)
{
    const uint16_t *src = (const uint16_t *)DG_ScreenBuffer;
    int y, x;

#ifdef N2_TARGET_DE0CV

    /* DOOM renders 320x200; the 20-pixel top/bottom bars stay black. */
    for (y = 0; y < DOOM_H; ++y) {
        uint32_t row_offset = (uint32_t)(y + 20) * FB_PITCH;
        const uint16_t *src_row = src + (uint32_t)y * DOOM_W;

        for (x = 0; x < DOOM_W; x += 2) {
            uint32_t packed =
                (uint32_t)src_row[x] |
                ((uint32_t)src_row[x + 1] << 16);
            fb_write32(row_offset + (uint32_t)x * 2u, packed);
        }
    }

#else

    /* CPUlator framebuffer is ordinary SDRAM: pack two RGB565 pixels/word. */
    for (y = 0; y < DOOM_H; ++y) {
        uint32_t *dst_row =
            fb + (uint32_t)(y + 20) * (FB_PITCH / 4u);
        const uint16_t *src_row = src + (uint32_t)y * DOOM_W;

        for (x = 0; x < DOOM_W; x += 2) {
            dst_row[x / 2] =
                (uint32_t)src_row[x] |
                ((uint32_t)src_row[x + 1] << 16);
        }
    }

#endif
}

void DG_SleepMs(uint32_t ms) { n2_sleep_ms(ms); }
uint32_t DG_GetTicksMs(void) { return n2_ticks_ms(); }
int DG_GetKey(int *pressed, unsigned char *key) { return n2_ps2_getkey(pressed, key); }
void DG_SetWindowTitle(const char *title) { (void)title; }
