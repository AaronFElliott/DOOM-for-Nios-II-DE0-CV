# Upstream engine

The DOOM core is included as the `ozkl/doomgeneric` Git submodule.

The Nios II build uses the engine's portable `doomgeneric` API and replaces the desktop filesystem/video/timer backends with `platform/nios2`.
