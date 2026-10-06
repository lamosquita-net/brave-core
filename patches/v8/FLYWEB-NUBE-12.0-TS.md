# FlyWeb: unexploited V8 12.0.267.17 security fixes (NUBE, for SEGURIDAD, FS.5)

Branch `nube/v8-12.0-ts`, on top of `nube/motor-120`. Only `patches/v8/`. Inventory and reasoning:
softmac `FlyWeb/docs/v8-12.0-turboshaft.md`. **Does not bump `kFlyWebCacheEpoch`**: SEGURIDAD bumps it once when
merging into `seg/v8-12.0`.

| Patch(es) | V8 commit | Bug | How |
|---|---|---|---|
| `src-builtins-builtins-array.cc` | 73c61498 (M120-LTS) | b/338908243 | Google's 12.0 cherry-pick, as is |
| `src-ic-ic.cc` | b91805d0 (M120-LTS) | b/339736513 | as is |
| `src-objects-map-updater.cc` | d6287039 (M120-LTS) | b/330760873 | as is |
| `src-inspector-v8-console-message.cc` | 6feeaeae (M120-LTS) | b:323813642 | as is |
| `src-compiler-backend-instruction-selector.cc` | 1e1a2073 (M120-LTS) + 86c99d86 (TurboFan hunk only) | b/356196918, chromium:1520697 | as is + by hand |
| `src-compiler-graph-assembler.cc`, `src-wasm-module-decoder-impl.h` | 872ec583, 8856a2a6 (M120-LTS) | chromium:330575498, 330589218 | as is |
| `src-wasm-canonical-types.{cc,h}` | 8d0519c8 (M120-LTS) | b/344608204 | as is |
| `src-wasm-{module-instantiate.cc,wasm-js.cc,wasm-objects.cc,wasm-objects.h,wasm-objects.tq}` | 41a7b57b (M120-LTS) | b/346197738 | as is |
| `src-compiler-wasm-compiler.cc` | ff462a28 (M120-LTS) | b/351327767 | as is |
| `src-wasm-graph-builder-interface.cc` | ce430536 (12.0 merge after .17) | chromium:1518257 | as is |
| `src-compiler-turboshaft-machine-optimization-reducer.h`, `src-compiler-turboshaft-operations.h` | 3c44945b (M120-LTS) + 3883ca53 | chromium:1518396, 1520362 | as is + by hand (`tagged_base` threaded through `ReduceMemoryIndex`/`TryAdjustOffset`; the debug-only DCHECKs left out) |
| `src-compiler-turboshaft-builtin-call-descriptors.h` | 3b2395f6, 5dc28a96, b4ee268d | chromium:1489500, 1508367, 1507235 | as is |
| `src-compiler-turboshaft-operations.cc` | ef1b6876 (M138-LTS) | 488803413 | as is |
| `src-compiler-turboshaft-machine-lowering-reducer-inl.h` | 5518d099 | 347724915 | by hand; `AccessBuilder::ForStackArgument()` left in place (unused) so `access-builder.{cc,h}` is not touched |
| **`FLYWEB-NUBE-A3-sse.diff`** (not a `.patch`, so `apply_patches` skips it) | 32f54198 (M152) | 547936520 | by hand for 12.0's `store-store-elimination-reducer.h`. **Same file as CVE-2025-5419**: SEGURIDAD adds it to that `.patch` |

Not compiled when written (see the board, FS.5, for the d8 result).
