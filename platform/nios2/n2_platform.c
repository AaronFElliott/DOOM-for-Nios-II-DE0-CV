#include <stdint.h>
#include <string.h>
#include "io.h"
#include "n2_platform.h"

static uint32_t last_count, remainder, elapsed_ms;
static int timer_ready;

static uint16_t timer_read16(uint32_t offset)
{
    return IORD_16DIRECT(N2_TIMER_BASE, offset);
}

static void timer_write16(uint32_t offset, uint16_t value)
{
    IOWR_16DIRECT(N2_TIMER_BASE, offset, value);
}

static uint32_t snapshot(void)
{
    uint16_t low, high;

    timer_write16(N2_TIMER_SNAPL, 0);
    low = timer_read16(N2_TIMER_SNAPL);
    high = timer_read16(N2_TIMER_SNAPH);

    return ((uint32_t)high << 16) | low;
}

static void timer_init(void)
{
    timer_write16(N2_TIMER_CONTROL, 0);
    timer_write16(N2_TIMER_STATUS, 0);
    timer_write16(N2_TIMER_PERIODL, 0xFFFFu);
    timer_write16(N2_TIMER_PERIODH, 0xFFFFu);
    timer_write16(N2_TIMER_CONTROL, 6);
    last_count = snapshot();
    remainder = elapsed_ms = 0;
    timer_ready = 1;
}

uint32_t n2_ticks_ms(void)
{
    uint32_t now, delta, per_ms = N2_TIMER_HZ / 1000u;
    if (!timer_ready) timer_init();
    now = snapshot();
    delta = last_count - now;
    last_count = now;
    remainder += delta;
    elapsed_ms += remainder / per_ms;
    remainder %= per_ms;
    return elapsed_ms;
}

void n2_sleep_ms(uint32_t ms)
{
    uint32_t target = n2_ticks_ms() + ms;
    while ((int32_t)(n2_ticks_ms() - target) < 0) { }
}

static unsigned char scan(unsigned int c)
{
    switch (c) {
    case 0x1C:return 'a'; case 0x32:return 'b'; case 0x21:return 'c';
    case 0x23:return 'd'; case 0x24:return 'e'; case 0x2B:return 'f';
    case 0x34:return 'g'; case 0x33:return 'h'; case 0x43:return 'i';
    case 0x3B:return 'j'; case 0x42:return 'k'; case 0x4B:return 'l';
    case 0x3A:return 'm'; case 0x31:return 'n'; case 0x44:return 'o';
    case 0x4D:return 'p'; case 0x15:return 'q'; case 0x2D:return 'r';
    case 0x1B:return 's'; case 0x2C:return 't'; case 0x3C:return 'u';
    case 0x2A:return 'v'; case 0x1D:return 'w'; case 0x22:return 'x';
    case 0x35:return 'y'; case 0x1A:return 'z';
    case 0x16:return '1'; case 0x1E:return '2'; case 0x26:return '3';
    case 0x25:return '4'; case 0x2E:return '5'; case 0x36:return '6';
    case 0x3D:return '7'; case 0x3E:return '8'; case 0x46:return '9';
    case 0x45:return '0'; case 0x29:return ' '; case 0x5A:return 13;
    case 0x76:return 27; case 0x0D:return 9; case 0x66:return 0x7F;
    case 0x12:return 0xB6; case 0x59:return 0xB6; default:return 0;
    }
}

static unsigned char ext_scan(unsigned int c)
{
    switch (c) {
    case 0x14:return 0x9D; case 0x11:return 0xB8; case 0x6B:return 0xAC;
    case 0x72:return 0xAF; case 0x74:return 0xAE; case 0x75:return 0xAD;
    case 0x5A:return 13; default:return 0;
    }
}

int n2_ps2_getkey(int *pressed, unsigned char *key)
{
    static int release, extended;
    volatile uint32_t *ps2 = (volatile uint32_t *)N2_PS2_BASE;
    for (;;) {
        uint16_t v = (uint16_t)ps2[0];
        unsigned int c;
        unsigned char mapped;
        if (!(v & N2_PS2_RVALID)) return 0;
        c = v & 0xFFu;
        if (c == 0xE0) { extended = 1; continue; }
        if (c == 0xF0) { release = 1; continue; }
        mapped = extended ? ext_scan(c) : scan(c);
        extended = 0;
        if (!mapped) { release = 0; continue; }
        *pressed = !release; *key = mapped; release = 0;
        return 1;
    }
}

static int is_doom1(const char *p)
{
    const char *s;
    if (!p) return 0;
    s = strrchr(p, '/'); if (s) p = s + 1;
    s = strrchr(p, '\\'); if (s) p = s + 1;
    return (p[0]=='d'||p[0]=='D')&&(p[1]=='o'||p[1]=='O')&&
           (p[2]=='o'||p[2]=='O')&&(p[3]=='m'||p[3]=='M')&&
           p[4]=='1'&&p[5]=='.'&&(p[6]=='w'||p[6]=='W')&&
           (p[7]=='a'||p[7]=='A')&&(p[8]=='d'||p[8]=='D')&&p[9]==0;
}

int n2_wad_is_available(const char *path)
{
    const volatile unsigned char *w = (const volatile unsigned char *)(uintptr_t)N2_WAD_BASE;
    if (!is_doom1(path)) return 0;
    if (w[0]!='I'||w[1]!='W'||w[2]!='A'||w[3]!='D') return 0;
    return n2_wad_length()!=0;
}

uint32_t n2_wad_length(void)
{
    const volatile uint32_t *w = (const volatile uint32_t *)(uintptr_t)N2_WAD_BASE;
    uint32_t count=w[1], dir=w[2];
    if (!count || count>65535u || dir<12u || dir>0x08000000u) return 0;
    return dir + count*16u;
}

void n2_platform_init(void) { timer_init(); }
