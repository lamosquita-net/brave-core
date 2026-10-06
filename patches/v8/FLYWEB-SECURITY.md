# FlyWeb: V8 12.0.267.36 (M120-LTS, Chrome 120) and its security backports

Since engine level 118 each level carries the V8 of Chrome N (decision of the HUMAN, 05-10; task FM.5). For level 120
SEGURIDAD moved this branch (FS.5, 06-10-2026) from **12.0.267.17** (the V8 of Chrome 120.0.6099.234) to
**12.0.267.36** (5d02023981f2, `FlyWeb/v8-revision`): the last release of Google's **M120-LTS** branch of this same V8,
i.e. 12.0.267.17 plus 38 commits of security fixes backported and tested by Google (until 08-2024). Same Chrome 120 web
platform. `FlyWeb/scripts/build.sh` (softmac) checks it out before `apply_patches`, on top of Chromium 116.0.5845.188.

## Already in 12.0.267.36 (patches dropped)

- From M120-LTS: CVE-2024-5274 (8f1c780b; NUBE's `src-ast-scopes.cc`/`src-parsing-parser-base.h` patches dropped),
  CVE-2024-4947 (2944ee98), CVE-2024-4761 (de7a07b7), CVE-2024-7971 (ae63f970), and the non-exploited fixes NUBE listed
  in softmac `FlyWeb/docs/v8-12.0-turboshaft.md` (section C: 73c61498, b91805d0, d6287039, 6feeaeae, 1e1a2073,
  872ec583, 8856a2a6, 8d0519c8, 41a7b57b, ff462a28, ce430536, d5bede9c) and Turboshaft A1 (3c44945b).
- CVE-2024-0519: fixed in V8 12.0 itself.
- chromium:1479104, chromium:1477588, groupBy and Promise.withResolvers: native in this V8.

## Patches in this directory

| Patch | What |
|---|---|
| `BUILD.gn.patch`, `src-codegen-compiler.cc.patch` | Brave (chromium_src include dir, PageGraph eval hook), rebased by NUBE |
| `src-interpreter-bytecode-generator.cc.patch` | CVE-2025-6554 (crbug 427663123, exploited), v8 22e9d9621 (NUBE) |
| `src-utils-version.h.patch` | `kFlyWebCacheEpoch` in `Version::Hash()` (LOCAL, step 51) |
| `src-compiler-access-builder.{cc,h}.patch`, `src-compiler-js-native-context-specialization.cc.patch` | CVE-2025-13223 (exploited) + its follow-up 9b5250b9 |
| `src-builtins-builtins-collections-gen.{cc,h}.patch`, `src-builtins-object-groupby.tq.patch`, `src-runtime-runtime-collections.cc.patch` | groupBy renderer crash: v8 77df647d + 92aba703 (runtime part) |
| `src-flags-flag-definitions.h.patch` | **Maglev off** (HUMAN, 05-10) and `maglev_untagged_phis` off (CVE-2026-3910) |
| `src-compiler-turboshaft-store-store-elimination-reducer.h.patch` | **CVE-2025-5419** (crbug 420636529, exploited; **new with 12.0: Turboshaft runs for optimized JS**), v8 7bc0a67e (load part), + Turboshaft A3: v8 67c8f3a9 (crbug 547936520, in-loop bases), by hand |
| `src-compiler-turboshaft-builtin-call-descriptors.h.patch` | Turboshaft A2: `CanAllocate`/`CanCallAnything` effects of SameValue, StringComparison, FindOrderedHashEntry (v8 e5e3ce3b, 9ccc063e, 7d12441a; crbug 1489500, 1508367, 1507235) |
| `src-compiler-turboshaft-operations.cc.patch` | Turboshaft A4: use-count saturation in `IsOnlyUserOf` (v8 23ec84a3, crbug 488803413) |
| `src-compiler-turboshaft-machine-optimization-reducer.h.patch`, `src-compiler-turboshaft-operations.h.patch` | Turboshaft A5: Load offset underflow with tagged base (v8 7c0f4cc3, crbug 1520362), by hand, without its new DCHECKs |
| `src-compiler-turboshaft-machine-lowering-reducer-inl.h.patch` | Turboshaft A6: `LoadStackArgument` lowered to a Tagged load (v8 96493c74, crbug 347724915) |

Not ported: Turboshaft A7 (a5e22070, crbug 1520697): its bug needs `BitcastWordToTaggedSigned` FrameState inputs, which
12.0's `recreate-schedule.cc` never emits (it only uses `BitcastWordToTagged`). CVE-2026-87491: V8 sandbox escape, the
sandbox is not a security boundary before Chrome 123 (softmac `cve-triage.md`).

Tested by SEGURIDAD on a Linux x64 d8 of 12.0.267.36 with all of these patches: builds with no errors; `--no-maglev`,
`--turboshaft`; regression tests 420636529 (CVE-2025-5419), 347724915 (A6), 360700873, 475479135-1/-2 and FlyWeb's
13223 test pass; groupBy throws RangeError; **mjsunit 6458/6458**. Note: the 420636529 and 347724915 tests also pass
**without** their patches on 12.0, so those two fixes could not be reproduced on this V8 (the tests target 2024-25
code); the patches only make the optimizer more conservative.
**FlyWeb itself is not compiled in the cloud**: LOCAL must build it.
