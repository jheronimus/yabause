#ifndef YABAUSE_RA_INTEGRATION_CWRAPPER_H
#define YABAUSE_RA_INTEGRATION_CWRAPPER_H

/*
 * Stub RetroAchievements integration for the libretro core.
 *
 * The upstream YabaSanshiro emulator core (memory.c, save states) includes
 * this header unconditionally.  The standalone app backs it with rcheevos
 * (the Android RetroAchievements integration).  The libretro core has no
 * such integration: RetroArch implements RetroAchievements itself through
 * the core's memory-map ABI.  These stubs keep the pruned core building
 * byte-identical to upstream while disabling RA progress serialization.
 */

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

size_t YabauseRA_GetProgressSize(void);
int YabauseRA_SerializeProgress(uint8_t *buffer, size_t buffer_size);
int YabauseRA_DeserializeProgress(const uint8_t *buffer, size_t buffer_size);
int YabauseRA_Initialize(void);
void YabauseRA_Shutdown(void);

#ifdef __cplusplus
}
#endif

#endif /* YABAUSE_RA_INTEGRATION_CWRAPPER_H */
