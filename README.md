# YabaSanshiro libretro core

A pruned YabaSanshiro (Sega Saturn emulator) source tree with a libretro
core glue, maintained as part of the Minime project.

## Branches

- `main` — this port: pruned emulator core + libretro glue (what Minime
  submodules at `src/yabause`).
- `upstream` — pristine YabaSanshiro source tarball snapshots, one per
  release. New releases are committed here verbatim.
- `archive` — the final commit of the deleted `devmiyax/yabause` repo
  (v1.17.7), recovered via `Hengle/yabause`. Read-only.

## Layout

- `yabause/src/` — upstream-shaped emulator core. Kept byte-identical to
  upstream except a small documented set of fixes (see below).
- `yabause/src/libretro/` — the libretro glue (ours; heavily diverges from
  the dead upstream glue).
- `libchdr/` — vendored libchdr + deps (zlib/zstd/lzma) for CHD disc
  support, matching beetle-saturn's approach.

## Updating from a new upstream release

1. Commit the new source tarball to `upstream`.
2. Copy changed core files over from `upstream`; do not copy the dead
   libretro glue.
3. Re-apply pruning (delete newly-added Android/Qt/iOS/etc.).
4. Update `yabause/src/libretro/Makefile.common` for renamed/moved files.
5. Verify the build; commit to `main`.

See `docs/adr/0015-yabasanshiro-libretro-core-topology.md` and
`docs/adr/0016-yabasanshiro-libretro-renderer.md` in Minime for the full
rationale.

## Build

From `yabause/src/libretro/`:

    make platform=unix        # Linux: yabasanshiro_libretro.so
    make platform=osx         # macOS (dev): yabasanshiro_libretro.dylib

The core targets OpenGL ES 3.0 on-device (`FORCE_GLES=1`); the desktop GL
path is used for local development.

## Deliberate divergences from upstream core files

- `yabause/src/scsp.cpp` — fixed two broken lines in the `ARCH_IS_LINUX`
  audio-sync path (`time(&tm)` → `clock_gettime(CLOCK_REALTIME,&tm)` and
  `ctime(&tm)` → `&tm`). Upstream code does not compile.
- `yabause/src/yui.h` — added prototypes for `YuiMsg` and `YuiGetFB`
  (frontend callbacks the libretro glue implements).

Everything else in `yabause/src/` is byte-identical to the upstream tarball.

## License

GPLv2. See `yabause/COPYING.txt` and `LICENSE`.
