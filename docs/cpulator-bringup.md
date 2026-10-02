# CPUlator bring-up

Select **Architecture → Nios II** and **System → Nios II DE0**.

Compile this multi-file application to an ELF with the Nios II toolchain, then load the ELF into CPUlator.

## WAD

Load your local `doom1.wad` into simulated memory at:

`0x003E0000`

The port treats that region as the read-only WAD image.

## Runtime

The Nios II entry point supplies:

`-iwad doom1.wad -gfxmode rgb565 -mb 3`

## Useful breakpoints

`main` → `DG_Init` → `n2_wad_is_available` → `W_OpenFile` → `D_DoomMain` → `DG_DrawFrame`

For input/timing, use `n2_ps2_getkey` and `n2_ticks_ms`.
