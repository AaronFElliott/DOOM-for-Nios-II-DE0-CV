#include <stdint.h>
#include <string.h>

#include "n2_platform.h"

static volatile uint16_t *const timer = (volatile uint16_t *)N2_TIMER_BASE;
static volatile uint32_t *const ps2 = (volatile uint32_t *)N2_PS2_BASE;

static uint32_t timer_last;
static uint32_t timer_remainder;
static uint32_t timer_ms;
static int timer_ready;

static uint32_t timer_snapshot(void)
{
    uint16_t lo;
    uint16_t hi;

    timer[N2_TIMER_SNAPL / 2u] = 0;
    lo = timer[N2_TIMER_SNAPL / 2u];
    hi = timer[N2_TIMER_SNAPH / 2u];

    return ((uint32_t)hi << 16) | lo;
}

static void timer_init(void)
{
    uint32_t period = N2_TIMER_HZ / 1000u;

    if (period == 0)
        period = 1;

    timer[N2_TIMER_CONTROL / 2u] = 0;
    timer[N2_TIMER_STATUS / 2u] = 0;
    timer[N2_TIMER_PERIODL / 2u] = (uint16_t)((period - 1u) & 0xFFFFu);
    timer[N2_TIMER_PERIODH / 2u] = (uint16_t)((period - 1u) >> 16);
    timer[N2_TIMER_CONTROL / 2u] = 6;

    timer_last = timer_snapshot();
    timer_remainder = 0;
    timer_ms = 0;
    timer_ready = 1;
}

uint32_t n2_ticks_ms(void)
{
    uint32_t now;
    uint32_t delta;
    uint32_t ticks_per_ms = N2_TIMER_HZ / 1000u;

    if (!timer_ready)
        timer_init();

    now = timer_snapshot();
    delta = timer_last - now;
    timer_last = now;

    timer_remainder += delta;
    timer_ms += timer_remainder / ticks_per_ms;
    timer_remainder %= ticks_per_ms;

    return timer_ms;
}

void n2_sleep_ms(uint32_t ms)
{
    uint32_t target = n2_ticks_ms() + ms;

    while ((int32_t)(n2_ticks_ms() - target) < 0)
        ;
}

static unsigned char translate_scan(unsigned int code)
{
    switch (code) {
    case 0x1C: return 'a'; case 0x32: return 'b'; case 0x21: return 'c';
    case 0x23: return 'd'; case 0x24: return 'e'; case 0x2B: return 'f';
    case 0x34: return 'g'; case 0x33: return 'h'; case 0x43: return 'i';
    case 0x3B: return 'j'; case 0x42: return 'k'; case 0x4B: return 'l';
    case 0x3A: return 'm'; case 0x31: return 'n'; case 0x44: return 'o';
    case 0x4D: return 'p'; case 0x15: return 'q'; case 0x2D: return 'r';
    case 0x1B: return 's'; case 0x2C: return 't'; case 0x3C: return 'u';
    case 0x2A: return 'v'; case 0x1D: return 'w'; case 0x22: return 'x';
    case 0x35: return 'y'; case 0x1A: return 'z';

    case 0x16: return '1'; case 0x1E: return '2'; case 0x26: return '3';
    case 0x25: return '4'; case 0x2E: return '5'; case 0x36: return '6';
    case 0x3D: return '7'; case 0x3E: return '8'; case 0x46: return '9';
    case 0x45: return '0';

    case 0x29: return ' ';
    case 0x5A: return 13;
    case 0x76: return 27;
    case 0x0D: return 9;
    case 0x66: return 0x7F;
    case 0x12: return 0xB6;
    case 0x59: return 0xB6;
    default: return 0;
    }
}

static unsigned char translate_extended(unsigned int code)
{
    switch (code) {
    case 0x14: return 0x9D;
    case 0x11: return 0xB8;
    case 0x6B: return 0xAC;
    case 0x72: return 0xAF;
    case 0x74: return 0xAE;
    case 0x75: return 0xAD;
    case 0x5A: return 13;
    default: return 0;
    }
}

int n2_ps2_getkey(int *pressed, unsigned char *key)
{
    static int release;
    static int extended;

    for (;;) {
        uint16_t value = (uint16_t)ps2[N2_PS2_DATA / 4u];
        unsigned int code;
        unsigned char mapped;

        if ((value & N2_PS2_RVALID) == 0)
            return 0;

        code = value & 0xFFu;

        if (code == 0xE0) {
            extended = 1;
            continue;
        }

        if (code == 0xF0) {
            release = 1;
            continue;
        }

        mapped = extended ? translate_extended(code) : translate_scan(code);
        extended = 0;

        if (!mapped) {
            release = 0;
            continue;
        }

        *pressed = !release;
        *key = mapped;
        release = 0;
        return 1;
    }
}

static int doom1_name(const char *path)
{
    const char *p;

    if (!path)
        return 0;

    p = strrchr(path, '/');
    if (p) path = p + 1;
    p = strrchr(path, '\\');
    if (p) path = p + 1;

    return (path[0] == 'd' || path[0] == 'D') &&
           (path[1] == 'o' || path[1] == 'O') &&
           (path[2] == 'o' || path[2] == 'O') &&
           (path[3] == 'm' || path[3] == 'M') &&
           path[4] == '1' && path[5] == '.' &&
           (path[6] == 'w' || path[6] == 'W') &&
           (path[7] == 'a' || path[7] == 'A') &&
           (path[8] == 'd' || path[8] == 'D') &&
           path[9] == '\0';
}

int n2_wad_is_available(const char *path)
{
    const volatile unsigned char *w =
        (const volatile unsigned char *)(uintptr_t)N2_WAD_BASE;

    if (!doom1_name(path))
        return 0;

    if (w[0] != 'I' || w[1] != 'W' || w[2] != 'A' || w[3] != 'D')
        return 0;

    return n2_wad_length() != 0;
}

uint32_t n2_wad_length(void)
{
    const volatile uint32_t *w =
        (const volatile uint32_t *)(uintptr_t)N2_WAD_BASE;
    uint32_t numlumps = w[1];
    uint32_t dir = w[2];

    if (numlumps == 0 || numlumps > 65535u)
        return 0;

    if (dir < 12u || dir > 0x08000000u)
        return 0;

    return dir + numlumps * 16u;
}

void n2_platform_init(void)
{
    timer_init();
}
