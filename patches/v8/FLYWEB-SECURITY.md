# FlyWeb: V8 13.2.152.56 (Chrome 132, M132-LTS) and its security backports

Since engine level 118 each level carries the V8 of Chrome N (FM.5). Level 132 uses V8 **13.2.152.56** (5eabe0e1,
`FlyWeb/v8-revision`: the head of branch-heads/13.2, i.e. 132.0.6834.160's 13.2.152.36 plus the M132-LTS merges),
on top of MOTOR-25's `nube/motor25-p25` fbe2394fe (FlyWeb 2.0.3). SEGURIDAD-PORTES-9, 27-10-2026 (task FS.28).

## 1. Rebase of the 61 patches of V8 13.1.201.22

Each patch checked on a clean 13.2.152.56 (`git apply --check`, `-R --check`, `git apply -3` and by hand):

- **26 already in 13.2** (the whole patch reverses cleanly): bigint `fromstring.cc`, `code-stub-assembler-inl.h` and
  `.h`, `handler-table.cc`, `access-info.cc`, arm64 instruction selector, `property-access-builder.cc`,
  `representation-change.cc`, Turboshaft ISel phase (`.cc`, `.h`), `snapshot-table-opindex.h`, `deoptimizer.cc`,
  `bytecode-generator.cc` (CVE-2025-6554), `objects/code.h`, the 11 Intl `js-*.cc`, `wasm-code-manager.h`,
  `wasm-engine.h`.
- **10 more already in 13.2** (after `git apply -3`, only FlyWeb's comments differed, or 13.2 has upstream's own
  version of the fix): `code-stub-assembler.cc` (13.2 keeps upstream's `CSA_DCHECK`), `escape-analysis.cc`
  (91343bb4), `js-call-reducer.cc` and `js-inlining.cc` (d3577087), `late-load-elimination-reducer.cc` (5cf18d03),
  `loop-unrolling-reducer.h` (2e96808d), `js-regexp.cc` (62ee3244), `preparser.cc` (9a884b1b), `wasm-engine.cc` and
  `wasm-import-wrapper-cache.cc` (924c1a79). Each file equals 13.2's once the conflict keeps 13.2's side.
- **2 rebased by hand:** `codegen/compiler.cc` (Brave's `BRAVE_COMPILER_GET_FUNCTION_FROM_EVAL` hook after 13.2's
  `CHECK`, identical to Brave 1.74.51's patch) and `store-store-elimination-reducer-inl.h` (v8 67c8f3a9, crbug
  547936520, kept on 13.2's `is_fixed_offset_store`; the CVE-2025-5419 half, 7bc0a67e, is in 13.2).
- **23 apply unchanged** (all are fixes from after the 13.2 LTS, or FlyWeb's own): `BUILD.gn` (Brave's hook),
  collections and `Object.groupBy` (`builtins-collections-gen.{cc,h}`, `object-groupby.tq`,
  `runtime-collections.cc`), CVE-2026-87491 (`builtins-definitions.h`, `builtins-wasm-gen.cc`, `wasm.tq`),
  JS↔Wasm `check()`s (`js-to-wasm.tq`, `wasm-to-js.tq`), CVE-2025-13223's TurboFan part and 9b5250b9
  (`js-native-context-specialization.cc`, `access-builder.{cc,h}`), `turboshaft/operations.cc`, access checks
  (`messages.cc`, `ic.cc`, `accessor-assembler.cc`), `microtask-queue.{cc,h}`, `cppgc/js-cpp-heap.cc`,
  CVE-2026-11645 (`js-objects.cc`), options (`flag-definitions.h`) and the cache epoch (`version.h`).

**25 patches** (61 − 36). `git apply` of all 25 on a clean 13.2.152.56 leaves 25 files changed, +321/−93.

## 2. Options (rule 19)

| Option | 13.2 upstream | FlyWeb level 132 |
|---|---|---|
| `turboshaft_wasm` | **true** (new default) | true, as upstream (no patch) |
| `maglev` | true | **false** (patch kept) until COORDINACIÓN decides (FS.28) |
| `maglev_untagged_phis` | true | **false** (patch kept: main turned it off for CVE-2026-3910, 7076ba13, after 13.2) |
| `turboshaft_instruction_selection` | true | true |

## 3. Cache epoch

`kFlyWebCacheEpoch` **18** (17 was 2.0.1–2.0.3 on 13.1).
