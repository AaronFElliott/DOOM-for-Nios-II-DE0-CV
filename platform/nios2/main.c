#include "doomgeneric.h"

static char a0[] = "doom";
static char a1[] = "-iwad";
static char a2[] = "doom1.wad";
static char a3[] = "-gfxmode";
static char a4[] = "rgb565";
static char a5[] = "-mb";
static char a6[] = "2";
static char *argv[] = { a0, a1, a2, a3, a4, a5, a6, 0 };

int main(void)
{
    doomgeneric_Create(7, argv);
    for (;;)
        doomgeneric_Tick();
    return 0;
}
