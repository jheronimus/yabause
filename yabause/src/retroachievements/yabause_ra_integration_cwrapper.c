/*
 * Stub RetroAchievements integration for the libretro core.
 *
 * The pruned core keeps upstream's memory.c byte-identical, which calls
 * YabauseRA_* from its save-state serialization path.  The standalone app
 * backs these with rcheevos; the libretro core deliberately does not (see
 * yabause_ra_integration_cwrapper.h).  These no-ops keep the core building
 * while RetroArch's own RetroAchievements handles the feature via the
 * memory-map ABI.
 */

#include "retroachievements/yabause_ra_integration_cwrapper.h"

size_t YabauseRA_GetProgressSize(void)
{
   return 0;
}

int YabauseRA_SerializeProgress(uint8_t *buffer, size_t buffer_size)
{
   (void)buffer;
   (void)buffer_size;
   return 0;
}

int YabauseRA_DeserializeProgress(const uint8_t *buffer, size_t buffer_size)
{
   (void)buffer;
   (void)buffer_size;
   return 0;
}

int YabauseRA_Initialize(void)
{
   return 0;
}

void YabauseRA_Shutdown(void)
{
}
