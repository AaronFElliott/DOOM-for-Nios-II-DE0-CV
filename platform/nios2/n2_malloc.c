#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "n2_platform.h"

#define N2_SCREEN_BYTES (320u * 200u * 4u)

void *n2_malloc(size_t size)
{
    static int screen_buffer_given;

    /*
     * doomgeneric.c allocates its 320x200 32-bit screen buffer before
     * D_DoomMain() starts the DOOM zone allocator. Keep that 256 KB buffer
     * outside the general-purpose heap so the zone allocation has room.
     */
    if (!screen_buffer_given && size == N2_SCREEN_BYTES) {
        screen_buffer_given = 1;
        return (void *)(uintptr_t)N2_SCREEN_BASE;
    }

    return malloc(size);
}
