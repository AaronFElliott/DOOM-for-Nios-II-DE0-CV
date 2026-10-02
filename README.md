# DOOM for Nios II — DE0-CV

Bare-metal DOOM targeting the Altera DE0-CV, using CPUlator's **Nios II DE0** system as the first bring-up target.

- `third_party/doomgeneric`: pinned upstream DOOM engine submodule
- `platform/nios2`: video, timer, PS/2 keyboard, and memory-resident WAD layer
- `software/nios2`: BSP build configuration
- `docs/`: bring-up notes

The first playable target is shareware DOOM. Proprietary WAD files are deliberately not committed.

Start with `docs/cpulator-bringup.md`.
