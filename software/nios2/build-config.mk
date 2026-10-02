DOOM_DIR := ../../third_party/doomgeneric
N2_DIR := ../../platform/nios2

DOOM_CPPFLAGS += -I$(DOOM_DIR) -I$(N2_DIR)
DOOM_CPPFLAGS += -DDOOMGENERIC_RESX=320 -DDOOMGENERIC_RESY=200

DOOM_SOURCES := $(wildcard $(DOOM_DIR)/*.c)
DOOM_SOURCES := $(filter-out $(DOOM_DIR)/m_misc.c $(DOOM_DIR)/w_file.c $(DOOM_DIR)/w_file_stdc.c $(DOOM_DIR)/doomgeneric_%.c $(DOOM_DIR)/i_sdlmusic.c $(DOOM_DIR)/i_sdlsound.c $(DOOM_DIR)/i_allegromusic.c $(DOOM_DIR)/i_allegrosound.c,$(DOOM_SOURCES))

N2_PLATFORM_SOURCES := $(N2_DIR)/main.c $(N2_DIR)/doomgeneric_n2.c $(N2_DIR)/n2_platform.c $(N2_DIR)/w_file_n2.c $(N2_DIR)/m_misc_n2.c
