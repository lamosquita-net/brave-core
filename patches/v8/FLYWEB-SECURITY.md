# FlyWeb: V8 11.8.172.18 (Chrome 118.0.5993.159) and its security backports

Since engine level 118 each level carries the V8 of Chrome N (decision of the HUMAN, 05-10; task FM.5). This branch
uses V8 **11.8.172.18** (36e4828ab658, `FlyWeb/v8-revision`), checked out by `FlyWeb/scripts/build.sh` (softmac) before
`apply_patches`, on top of Chromium 116.0.5845.188.

| Patch | What | Status on 11.8.172.18 |
|---|---|---|
| `BUILD.gn.patch`, `src-codegen-compiler.cc.patch` | Brave (chromium_src include dir, PageGraph eval hook) | rebased; same as Brave's own patches for this V8 |
| `src-interpreter-bytecode-generator.cc.patch` | CVE-2025-6554 (crbug 427663123, exploited): TDZ hole-check elision across optional chains, v8 22e9d9621 | rebased, no conflicts |
| `src-parsing-parser-base.h.patch`, `src-ast-scopes.cc.patch` | CVE-2024-5274 (crbug 341663589, exploited): class static blocks, v8 3e037e195 | applies unchanged |
| `src-utils-version.h.patch` | `kFlyWebCacheEpoch` in `Version::Hash()` (LOCAL, step 51) | rebased |
| — | chromium:1479104 (`Promise.any`) and chromium:1477588 (TDZ in `do`-`while`) | **already in 11.8.172.18**: patches dropped |
| — | Object.groupBy/Map.groupBy and Promise.withResolvers ports to 11.6 | native in this V8: patches dropped |
| `src-runtime-runtime-object.cc.patch` | CVE-2024-0519 (crbug 1517354, exploited): drop the fast last-property deletion, v8 389ea9be | **SEGURIDAD, redone for 11.8** (same deletion as upstream, 174 lines) |
| `src-compiler-access-builder.{cc,h}.patch`, `src-compiler-js-native-context-specialization.cc.patch` | CVE-2025-13223 (crbug 460017370, exploited): TurboFan property backing store extension, v8 4cf9311 (TurboFan part) + 9b5250b9 (its own follow-up fix) | SEGURIDAD; same port as step 54, regenerated against 11.8 |
| `src-wasm-baseline-liftoff-{assembler.cc,assembler.h,compiler.cc}.patch` | CVE-2024-7971 (crbug 360700873, exploited): Liftoff loop inputs, v8 9797576 | SEGURIDAD; step 55 regenerated; the upstream regression test **crashes this 11.8 without it** |
| `src-builtins-builtins-collections-gen.{cc,h}.patch`, `src-builtins-object-groupby.tq.patch`, `src-runtime-runtime-collections.cc.patch` | groupBy renderer crash with huge inputs: v8 77df647d (crbug 438364208) + runtime part of 92aba703 (crbug 405910175) | SEGURIDAD; step 56 redone on the native groupBy of 11.8, which has the same bug (reproduced) |
| `src-compiler-access-info.cc.patch`, `src-maglev-maglev-graph-builder.cc.patch` | CVE-2024-4947 (crbug 340221135, exploited): Maglev stores to module exports, v8 b3c01ac1 | SEGURIDAD; **new with 11.8: Maglev is on by default from this V8** (it was off in 11.6) |
| `src-flags-flag-definitions.h.patch` (also NUBE's) | CVE-2026-3910 (crbug 491410818, exploited): `maglev_untagged_phis` off, exactly upstream's fix v8 7076ba1 | SEGURIDAD; **new with 11.8** (Maglev on) |

**SEGURIDAD (05-10, FS.4):** triage redone for 11.8.172.18 in `cve-triage.md` (softmac); the original note said: it has to be redone (fixes after
this V8 that apply to it, and the ones of 11.6 that are no longer needed). Tests and golden files are left out.
**Not compiled in the cloud**: LOCAL must build it.

