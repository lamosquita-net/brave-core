# FlyWeb: V8 13.3.415.23 (Chrome 133) with the M132-LTS backports it lacks

Engine level 133 (FM.49, MOTOR-27, 28-10-2026) uses V8 **13.3.415.23** (93c6f108, head of branch-heads/13.3, the V8 of
133.0.6943.141). Branch 13.3 froze on 22-01-2025, while 13.2.152.56 (M132-LTS, level 132) has merges up to 01-10-2025:
`git rev-list 93c6f108..5eabe0e1` leaves **12 security fixes that 13.3 lacks**, carried here as patches:

| v8 commit | crbug | Files | How |
|---|---|---|---|
| 3bdf541ae | 443765373 | `codegen/handler-table.cc` | as is |
| 19301b95f | 445380761 | arm64 instruction selector | as is |
| 725b6a21c | 427600180 | `bigint/fromstring.cc` | as is |
| 385d92ca6 | 425583995 | arm64 macro assembler | as is |
| 42421010c | 430344952 | `parsing/preparser.cc` | as is |
| f39628536 | 427663123 | `interpreter/bytecode-generator.cc` | as is (with FlyWeb's patch) |
| 5a7e02c2e | 412149700 | 10 Intl `js-*.cc` | as is |
| 80600881c | 420636529 | `store-store-elimination-reducer-inl.h` | as is |
| 2b2a58c26 | 420637585 | `compiler/representation-change.cc` | as is |
| 45d40ca20 | 398065918 | `maglev/maglev-graph-builder.cc` | as is (Maglev stays off until level 134) |
| 20c1b8e9b | 420697404 | `objects/js-regexp.cc` | by hand: 13.3 checks `IsOneByteRepresentation()` |
| 5cf18d038 | 417169470 | Late Load Elimination, `snapshot-table-opindex.h`, `store-store-elimination-phase.cc` | by hand: 13.3 already has the trace flag; only the map wipe on map stores |

The 48 patches of level 132 were moved with a three-way merge (`FlyWeb/tools/port/v8re.sh`); by hand
`late-load-elimination-reducer.cc` (FlyWeb's cabe17f3/153cde95/0652ef4b hunks on 13.3's `ProcessCall` with traces),
`messages.cc` (access checks; `DirectHandle` context), `store-store-elimination-reducer-inl.h` (after 80600881c) and
`js-to-wasm.tq` (one `check()` is already in 13.3). All 76 patches checked with `git apply --check` on a clean 93c6f108.
Cache epoch 21. Not compiled yet. Review: SEGURIDAD-PORTES, row FS.33 (rule 17).

V8 13.3 ships `Atomics.pause` and Wasm Memory64 by default, as Chrome 133; `turboshaft_wasm` and `flush_liftoff_code`
are gone as flags (gin's `kWebAssemblyLiftoffCodeFlushing` only passes the flag when overridden, so it is left as is).

**Stack clash in optimized JS frames** (SEGURIDAD-PORTES-11, 28-10-2026; v8 ea2f03dfff, crbug 574660146, merged into
15.6/15.7 on 26-10-2026): the prologue grows an optimized frame with one stack pointer decrement before the entry stack
check, so a frame over 4 KB can step over the guard page. Optimization now aborts with `kFunctionTooBig` (frames over
4 KB are rare). 13.3 has two register allocation paths (Turboshaft's `pipelines.h` and TurboFan's `PipelineImpl`): both
get the check and report failure to their callers (`pipeline.cc`, `turboshaft/pipelines.{h,cc}`), plus its mjsunit test.

---

# FlyWeb: V8 13.2.152.56 (Chrome 132, M132-LTS) and its security backports

Since engine level 118 each level carries the V8 of Chrome N (FM.5). Level 132 uses V8 **13.2.152.56** (5eabe0e1,
`FlyWeb/v8-revision`: the head of branch-heads/13.2, i.e. 132.0.6834.160's 13.2.152.36 plus the M132-LTS merges),
on top of MOTOR-25's `nube/motor-132` (FlyWeb 2.0.3 + level 132; MOTOR sets `FlyWeb/v8-revision` and adapts Blink).
SEGURIDAD-PORTES-9, 27-10-2026 (task FS.28).

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

## 2. Fixes after the 13.2 LTS (new in level 132)

The M132-LTS branch took fixes until 10-2025 and skipped some; the merges to branch-heads 13.3–15.8 were reviewed
(SEGURIDAD-PORTES-9, 454 commits not on main; 119 groups touching Maglev, Turboshaft or Wasm). Ported (7 new files):

- **JS late load elimination** (`turboshaft_load_elimination`, on in 13.1 and 13.2; earlier triage took it as off,
  as in 12.0): 0652ef4b (crbug 436623746, maps after an allocation), 153cde95 (508811477, maps on calls), cabe17f3
  (554421904, non-writing calls can still create aliases; adapted: 13.2's `ProcessCall` bails out on `!can_write()`
  first, so the non-aliasing inputs are now invalidated before it), 45353e2b (556908674, map changes in loops).
  `WipeAllMaps()`, which 153cde95 calls, does not exist in 13.2: added as upstream. The same four also go to 2.0.3 on
  13.1 (`seg/v8-13.1-lle`, epoch 18).
- **Turboshaft-Wasm** (on by default in 13.2): e60c703d (481074858), 588bb212 (445870128), 43357c8e (505481948),
  4239d7f5 (509268941) and ab3bec43 (518228382) in their final upstream form; 13.2 has no `SparseOpIndexSideTable`
  and ab3bec43 already replaces it with a `ZoneVector` of pairs.
- **TurboFan, Wasm inlined into JS:** 6cbcff99 (427502241), do not drop TypeGuards in `WasmGCLowering`.
- Already in 13.2 or in these patches: b3c01ac1 (CVE-2024-4947), CVE-2025-5419/6554/10585, ef1b6876 (`IsOnlyUserOf`),
  CVE-2026-11645 and CVE-2026-87491 (FlyWeb's own ports). Not applicable: CVE-2026-85046 (13.2 does not inline
  `Array.prototype.sort`).
- **Constness in `DefineNamedOwnProperty`** (FS.29, SEGURIDAD-PORTES-10): main 6937df3c (crbug 567088929, 10-2026;
  22e588df is its merge). New bytecode `DefineNamedOwnPropertyInLiteral` for the initializing stores of object
  literals; `DefineNamedOwnProperty` (spread literals, class fields) now deopts on a const field in TurboFan
  (`AccessMode::kDefine` instead of `kStoreInLiteral`) and in Maglev (adapted to 13.2's void visitors and
  `ReduceResult::Fail()`), so a redefinition can no longer keep a stale constant. Its 4 mjsunit tests come along
  (3 run with `--maglev`; `assertMaglevved` → `assertTrue(isMaglevved())`, which 13.2's `mjsunit.js` has). Not ported: the cctest goldens (13.2's format differs) and the follow-up c58171d6
  (performance only: spread literals back to the literal bytecode). Changes the bytecode list: cache epoch 20.

**48 patches** (32 + 12 new for sources + 4 tests; the `js-native-context-specialization.cc` and `version.h` patches
already existed and were regenerated).

## 3. Options (rule 19)

| Option | 13.2 upstream | FlyWeb level 132 |
|---|---|---|
| `turboshaft_wasm` | **true** (new default) | true, as upstream (no patch) |
| `maglev` | true | **false** (patch kept; COORDINACIÓN-16, 27-10: stays off at level 132) |
| `maglev_untagged_phis` | true | **false** (patch kept: main turned it off for CVE-2026-3910, 7076ba13, after 13.2) |
| `turboshaft_instruction_selection` | true | true |

**Why Maglev stays off (rule 19, to review at each level):** 13.2 is parked, so upstream's later Maglev fixes do not
arrive with it. About 15 security fixes after the LTS apply to 13.2's Maglev (+250/−100 lines, 8 to rewrite; 4–6
days), plus 6 of Phi untagging that `maglev_untagged_phis = false` covers, and about 1–1.5 new ones a month. The Maglev
half of CVE-2025-13223 (4cf93118) is not needed in 13.2 (it feeds Turbolev). TurboFan stays the optimizing tier.

## 4. Cache epoch

`kFlyWebCacheEpoch` **20** (17 = 2.0.1–2.0.2 on 13.1, 18 = 2.0.3 with `seg/v8-13.1-lle`, 19 = level 132, 20 = FS.29).

## 5. Tests (cloud `d8`, Linux x64, V8 only; `FlyWeb/scripts/v8-d8.sh`)

`d8` 13.2.152.56 with these 32 patches (without Brave's two `//brave` hooks), release with DCHECKs, clang of Chromium
116, libstdc++ (SEGURIDAD-PORTES-9, 27-10-2026).

- Build: the first try failed (`InvalidateAtMapOffset()` private; upstream has it public, fixed); then it builds.
- Options (`--print-flag-values`): `--no-maglev`, `--no-maglev-untagged-phis`, `--turboshaft-wasm`,
  `--turboshaft-wasm-load-elimination`, `--turboshaft-load-elimination`, `--turboshaft-instruction-selection`,
  `--wasm-generic-wrapper`, `--regexp-simd`.
- FlyWeb's regression tests (`FlyWeb/tools/v8-pruebas/`): **9/9**.
- **mjsunit 7336/7337** (default variant, includes `mjsunit/wasm`). The 1 is `maglev/extras-cped` (asserts Maglev code),
  as in 1.13–1.15.
- Not covered by the `d8`: Intl (no ICU) and Chromium's libc++. **Not compiled in FlyWeb**: LOCAL builds it.

**FS.29 (SEGURIDAD-PORTES-10, 27-10-2026), same `d8` with the 48 patches:** builds; FlyWeb's regression tests 9/9;
the 4 new tests 4/4; **mjsunit 7340/7341** (the 1 is `maglev/extras-cped`, as before).
