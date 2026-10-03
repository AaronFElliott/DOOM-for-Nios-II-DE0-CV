#ifndef N2_PLATFORM_H
#define N2_PLATFORM_H

#include <stdint.h>

#ifdef N2_TARGET_DE0CV
#define N2_PIXEL_BASE 0x08000000u
#define N2_PIXEL_CTRL_BASE 0xFF203020u
#define N2_PS2_BASE   0xFF200100u
#define N2_TIMER_BASE 0xFF202000u
#define N2_TIMER_HZ   100000000u
/* Upper half of the DE0-CV's 64 MiB SDRAM. */
#define N2_WAD_BASE   0x02000000u
#define N2_SCREEN_BASE 0x00360000u
#else
/*
 * CPUlator DE2: use ordinary SDRAM for the framebuffer so simulation does
 * not spend excessive time emulating per-pixel writes to the VGA device.
 * The VGA pixel-buffer controller is pointed at this region during init.
 */
#define N2_PIXEL_BASE 0x003A0000u
#define N2_PIXEL_CTRL_BASE 0x10003020u
#define N2_PS2_BASE   0x10000100u
#define N2_TIMER_BASE 0x10002000u
#define N2_TIMER_HZ   50000000u
/* CPUlator DE2 SDRAM reservation for the external WAD image. */
#define N2_WAD_BASE   0x003E0000u
#define N2_SCREEN_BASE 0x00360000u
#endif

#define N2_TIMER_STATUS   0x00u
#define N2_TIMER_CONTROL  0x04u
#define N2_TIMER_PERIODL  0x08u
#define N2_TIMER_PERIODH  0x0Cu
#define N2_TIMER_SNAPL    0x10u
#define N2_TIMER_SNAPH    0x14u

#define N2_PS2_DATA   0x00u
#define N2_PS2_RVALID 0x8000u

int n2_wad_is_available(const char *path);
uint32_t n2_wad_length(void);
uint32_t n2_ticks_ms(void);
void n2_sleep_ms(uint32_t ms);
int n2_ps2_getkey(int *pressed, unsigned char *key);
void n2_platform_init(void);

#endif
