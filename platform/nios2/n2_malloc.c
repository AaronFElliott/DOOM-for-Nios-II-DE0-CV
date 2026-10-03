#include <stddef.h>
#include <stdint.h>

#include "n2_platform.h"

#define N2_SCREEN_BYTES (320u * 200u * 4u)

/* GNU ld --wrap=malloc routes malloc() references here while leaving the
 * real newlib allocator available through __real_malloc(). */
extern void *__real_malloc(size_t size);

void *__wrap_malloc(size_t size)
{
    static int screen_buffer_given;

    /* doomgeneric.c requests this exact 256 KiB buffer before D_DoomMain().
     * Keep it outside the general-purpose heap so the DOOM zone has room. */
    if (!screen_buffer_given && size == N2_SCREEN_BYTES) {
        screen_buffer_given = 1;
        return (void *)(uintptr_t)N2_SCREEN_BASE;
    }

    return __real_malloc(size);
}
