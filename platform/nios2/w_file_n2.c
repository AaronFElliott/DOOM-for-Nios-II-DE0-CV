#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "doomtype.h"
#include "n2_platform.h"
#include "w_file.h"
#include "z_zone.h"

typedef struct {
    wad_file_t wad;
} n2_wad_file_t;

wad_file_t *W_OpenFile(char *path)
{
    n2_wad_file_t *f;

    if (!n2_wad_is_available(path))
        return NULL;

    f = Z_Malloc(sizeof(*f), PU_STATIC, 0);
    f->wad.file_class = NULL;
    f->wad.mapped = (byte *)(uintptr_t)N2_WAD_BASE;
    f->wad.length = n2_wad_length();

    return &f->wad;
}

void W_CloseFile(wad_file_t *wad)
{
    Z_Free(wad);
}

size_t W_Read(wad_file_t *wad, unsigned int offset,
              void *buffer, size_t length)
{
    if (offset >= wad->length)
        return 0;

    if (length > wad->length - offset)
        length = wad->length - offset;

    memcpy(buffer, wad->mapped + offset, length);
    return length;
}
