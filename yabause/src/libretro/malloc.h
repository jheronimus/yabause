/*
 * macOS compatibility shim.
 *
 * The pruned core's yabause.c includes <malloc.h>, which exists on Linux
 * (glibc) and Windows but not macOS.  yabause.c only needs malloc/free,
 * which come from <stdlib.h>.  This empty shim lets the core build on
 * macOS while staying byte-identical to upstream.  It is added to the
 * include path only for the osx platform.
 */
#ifndef __LIBRETRO_MALLOC_COMPAT_H__
#define __LIBRETRO_MALLOC_COMPAT_H__

#endif /* __LIBRETRO_MALLOC_COMPAT_H__ */
