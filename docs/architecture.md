# Architecture

DOOM renders internally at 320x200 with an 8-bit palette. The generic video layer converts each frame to RGB565 in the staging buffer.

The Nios II platform copies a 320x240 RGB565 image into the hardware pixel buffer. The pixel-buffer row stride is 1024 bytes.

The WAD is memory-resident rather than filesystem-backed. Keyboard input uses the University Program PS/2 peripheral and is polled by the DOOM event layer. Timing uses the interval timer and a millisecond accumulator.

Sound, mouse, and performance tuning are intentionally deferred until the first playable build works.
