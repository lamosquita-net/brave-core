# FlyWeb: V8 12.0.267.17 (Chrome 120.0.6099.234) and its security backports

Since engine level 118 each level carries the V8 of Chrome N (decision of the HUMAN, 05-10; task FM.5). This branch
uses V8 **12.0.267.17** (6372efd0ead5, `FlyWeb/v8-revision`), checked out by `FlyWeb/scripts/build.sh` (softmac) before
`apply_patches`, on top of Chromium 116.0.5845.188.

| Patch | What | Status on 12.0.267.17 |
|---|---|---|
| `BUILD.gn.patch`, `src-codegen-compiler.cc.patch` | Brave (chromium_src include dir, PageGraph eval hook) | rebased; same as Brave's own patches for this V8 |
| `src-interpreter-bytecode-generator.cc.patch` | CVE-2025-6554 (crbug 427663123, exploited): TDZ hole-check elision across optional chains, v8 22e9d9621 | rebased, no conflicts |
| `src-parsing-parser-base.h.patch`, `src-ast-scopes.cc.patch` | CVE-2024-5274 (crbug 341663589, exploited): class static blocks, v8 3e037e195 | applies unchanged |
| `src-utils-version.h.patch` | `kFlyWebCacheEpoch` in `Version::Hash()` (LOCAL, step 51) | rebased |
| — | chromium:1479104 (`Promise.any`) and chromium:1477588 (TDZ in `do`-`while`) | **already in 12.0.267.17**: patches dropped |
| — | Object.groupBy/Map.groupBy and Promise.withResolvers ports to 11.6 | native in this V8: patches dropped |
| **missing** | **CVE-2024-0519** (fast last-property deletion, `runtime-object.cc`) | **does not apply: SEGURIDAD must redo it on 12.0.267.17. Do not publish a version from this branch without it.** |

**SEGURIDAD:** the CVE triage (`cve-triage.md`) was done for 11.6.189.20; for 12.0.267.17 it has to be redone (fixes after
this V8 that apply to it, and the ones of 11.6 that are no longer needed). Tests and golden files are left out.
**Not compiled in the cloud**: LOCAL must build it.

