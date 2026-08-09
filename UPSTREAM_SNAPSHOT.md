# Upstream Snapshot Archive

This branch is an **immutable snapshot** of the final state of the upstream
Yaba Sanshiro source tree. It is read-only and exists only to preserve the
codebase that this fork's libretro core port is based on.

## Provenance

- **Upstream repository:** `devmiyax/yabause` (Yaba Sanshiro), now deleted
  from GitHub.
- **Recovered via:** `Hengle/yabause` — a fork that preserved the full
  upstream history, including the final commit.
- **Pinned commit:** `71c973f92966d33de464c1ba4dac1953af6ec462`
  — "Merge pull request #1030 from devmiyax/m17" (version 1.17.7),
  committed 2025-12-30.
- **Snapshot date:** recovered 2026-08-09.

## Purpose

The upstream `devmiyax/yabause` repository was deleted from GitHub. This
branch preserves the complete tree (source, CMake config, Qt/Android/iOS
ports, prebuilt Vulkan SDK libraries, and templates) exactly as it existed at
the final upstream commit, so the conversion work in `main` has a stable,
verifiable reference.

## Rules

- **Do not push to this branch.** It is protected to remain read-only.
- Porting work happens on `main`.
- If a newer upstream state is ever recovered, record it here before
  overwriting this snapshot.
