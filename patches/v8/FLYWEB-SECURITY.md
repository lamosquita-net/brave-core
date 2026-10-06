# FlyWeb: V8 11.9.169.7 (Chrome 119.0.6045.199) and its security backports

Since engine level 118 each level carries the V8 of Chrome N (decision of the HUMAN, 05-10; task FM.5). This branch
uses V8 **11.9.169.7** (1d8f52fa183e, `FlyWeb/v8-revision`), checked out by `FlyWeb/scripts/build.sh` (softmac) before
`apply_patches`, on top of Chromium 116.0.5845.188.

| Patch | What | Status on 11.9.169.7 |
|---|---|---|
| `BUILD.gn.patch`, `src-codegen-compiler.cc.patch` | Brave (chromium_src include dir, PageGraph eval hook) | rebased; same as Brave's own patches for this V8 |
| `src-interpreter-bytecode-generator.cc.patch` | CVE-2025-6554 (crbug 427663123, exploited): TDZ hole-check elision across optional chains, v8 22e9d9621 | rebased, no conflicts |
| `src-parsing-parser-base.h.patch`, `src-ast-scopes.cc.patch` | CVE-2024-5274 (crbug 341663589, exploited): class static blocks, v8 3e037e195 | applies unchanged |
| `src-utils-version.h.patch` | `kFlyWebCacheEpoch` in `Version::Hash()` (LOCAL, step 51) | rebased |
| — | chromium:1479104 (`Promise.any`) and chromium:1477588 (TDZ in `do`-`while`) | **already in 11.9.169.7**: patches dropped |
| — | Object.groupBy/Map.groupBy and Promise.withResolvers ports to 11.6 | native in this V8: patches dropped |
| `src-runtime-runtime-object.cc.patch` | CVE-2024-0519 (crbug 1517354, exploited): drop the fast last-property deletion, v8 389ea9be | SEGURIDAD, redone for 11.9 |
| `src-compiler-access-builder.{cc,h}.patch`, `src-compiler-js-native-context-specialization.cc.patch` | CVE-2025-13223 (exploited) + its follow-up 9b5250b9 | SEGURIDAD, as in 11.8 (`seg/v8-11.8`) |
| `src-wasm-baseline-liftoff-{assembler.cc,assembler.h,compiler.cc}.patch` | CVE-2024-7971 (exploited), v8 9797576 | SEGURIDAD, as in 11.8 |
| `src-builtins-builtins-collections-gen.{cc,h}.patch`, `src-builtins-object-groupby.tq.patch`, `src-runtime-runtime-collections.cc.patch` | groupBy renderer crash: v8 77df647d + 92aba703 (runtime part) | SEGURIDAD, as in 11.8 |
| `src-compiler-access-info.cc.patch`, `src-maglev-maglev-graph-builder.cc.patch` | CVE-2024-4947 (exploited, Maglev), v8 b3c01ac1 | SEGURIDAD, defense in depth (Maglev is off) |
| `src-flags-flag-definitions.h.patch` | **Maglev off** (HUMAN, 05-10) and `maglev_untagged_phis` off (CVE-2026-3910, v8 7076ba1) | SEGURIDAD |
| `src-objects-js-objects.cc.patch` | **CVE-2024-4761** (crbug 339458194, exploited, Wasm GC): only normalize JSObject targets in `SetOrCopyDataProperties`, v8 f320600c | SEGURIDAD; **new with 11.9: Wasm GC ships in this V8** (off in 11.8) |

**SEGURIDAD (06-10, FS.4):** triage redone for 11.9.169.7 in `cve-triage.md` (softmac); the original note said: it has to be redone (fixes after
this V8 that apply to it, and the ones of 11.6 that are no longer needed). Tests and golden files are left out.
**Not compiled in the cloud**: LOCAL must build it.

