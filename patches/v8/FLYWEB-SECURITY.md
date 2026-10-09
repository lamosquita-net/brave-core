# FlyWeb: V8 12.7.224.20 (Chrome 127, no LTS) and its security backports

Since engine level 118 each level carries the V8 of Chrome N (decision of the HUMAN, 05-10; task FM.5). This branch uses
V8 **12.7.224.20** (daea53b9, `FlyWeb/v8-revision`: Chrome 127.0.6533.144's 12.7.224.18 plus two security merges,
356196918 TurboFan and 360700873 Wasm), checked out by `FlyWeb/scripts/build.sh` (softmac) before `apply_patches`, on top
of Chromium 116.0.5845.188. SEGURIDAD-PORTES, 09-10-2026 (task FS.9), on top of MOTOR-2's `nube/motor-127` adc17adc.

**Rule for this level (COORDINACIÓN, HUMAN): 1.11 does not ship with fewer V8 fixes than 1.10.** 12.7 had no LTS branch:
the 12.6 branch kept receiving Google's M126-LTS merges until 03-2025 and FlyWeb 1.10 (12.6.228.49) has them natively,
but 12.7.224.20 does not. Everything below was compared on a clean 12.7.224.20 (`git apply`, `-R`, 3-way, and by hand).

## 1. FlyWeb 1.10's own patches (`seg/v8-12.6` c3d250ff), carried over

All 32 of them: 29 apply unchanged; `js-inlining.cc` (CHECK on the frame-state parameter count) and `preparser.cc`
(`eval` with escapes) with a smaller context; `js-regexp.cc` (`EscapeRegExpSource` in `uint32_t`, 62ee3244/20c1b8e9)
redone by hand because 12.7 uses `DirectHandle`. That is: CVE-2025-6554 (MOTOR-2's clean version), CVE-2025-13223 +
9b5250b9, CVE-2025-5419 + Turboshaft store-store fixes, `IsOnlyUserOf`, CVE-2026-87491, `groupBy` (77df647d + 92aba703),
5cf18d03 (late load elimination map aliasing), the M132-LTS ports (2b2a58c2, 1c7ff4d5, ca504d09 + f6961c40, 20c1b8e9,
42421010, d6a6e7c3, 725b6a21, 3bdf541a, 815da990) and the option changes below.

## 2. Google's M126-LTS merges that 12.7.224.20 lacks (in 1.10 natively)

Of the 21 merges on the 12.6 branch after 12.6.228.28, 3 are already in 12.7.224.20 (79c429b, 9d79b3b, 7acd351) and one
is Maglev and already there in its 12.8 form (066d846). The other 17, all applied unchanged:
c34adce, 96932a9 (TurboFan), **7c53644 (interpreter: also without JIT)**, fd73f2e (Liftoff), e379c53, 77a4fa1, 4f4cd3f,
96a23b2, bcb83bc (Wasm), 6e1cc25, 4b30831 (API, `include/v8-primitive.h`), **ab08344 as its 12.9 merge 2b72d95**,
**31c7633 + 74d8224 (sandbox EPT during scavenges) as the 12.8 merge e75055b + 74d8224**, and, for parity with 1.10
although off or not x64 here, 14d323f, 5c4b4ba (Maglev) and d120b8c (ARM).

## 3. New fixes from the 12.8 / 12.9 / 13.0 / 13.1 branches and M132-LTS that apply to 12.7

167 merges (12.8.374.38, 12.9.202.28, 13.0.245.25, 13.1.201.22, 13.2.152.56) = 110 distinct fixes, classified one by one.
Ported because their code exists in 12.7 (not in 12.6, or not reachable there):

| Fix | What |
|---|---|
| 7929e3e + f58e00f + 917b1e2 | TurboFan-Wasm: 12.7 puts the non-null `TypeGuard` of `br_on_non_null` before the null check, without a control dependency (type confusion); `br_on_cast` with a null type; endless loop in the `WasmTyper`. Wasm inlining ships on by default in 12.7 |
| 19301b9 | Upper 32 bits of `Int32MulOvfCheck` are not zero |
| 22e3faf | Turboshaft loop unrolling: signed division overflow (unrolling is on) |
| 2e96808 (by hand) | Turboshaft: no partial unrolling in functions over 1 M operations (memory/hang) |
| 309f157, e1497b2 | JS-to-Wasm wrappers: no tier-up for `WasmJSFunction`s; no inlining with `ref extern` parameters |
| 62a670a (by hand) | Top-level await with an errored async parent (runtime: **also without JIT**); `src/objects` part only |
| 5a7e02c (3-way) | Intl: an exception instead of a fatal crash when ICU runs out of resources. **Not tested in `d8`** (no Intl) |

Checked and **not applicable** to 12.7 (or off in FlyWeb): Maglev (off), JSPI, `wasm_deopt` and `call_indirect` inlining
(experimental), Turboshaft-Wasm (off by default in 12.7), ARM/LoongArch/MIPS, `d8`/build-only, the wasm-to-js wrapper
`TrustedHeapConstant` escape analysis (91343bb: no such opcode in 12.7, the build caught it), the wasm-to-js wrapper tier-up across instances (153d4e8: the generic wasm-to-js wrapper is off here), the 13.x type canonicalizer fixes
(a4a402d, 7615ae1, 77b7318, 0e98fad: 12.7 compares whole `ValueType` bit fields, relative bit included, and
`StructType::operator==` includes mutability), and bugs in code born in 12.8+ (ScopeInfo reuse, side-step transitions —
off in 12.7 —, the `Object.assign` fast path, `FastCloneJSObject`, the deserializer's `ExpectedTransition`, the global
regexp result cache, big WeakMaps).

## 4. Options (`src-flags-flag-definitions.h.patch`)

As in 1.7.1–1.10: `maglev`, `maglev_untagged_phis`, `turboshaft_instruction_selection` and `wasm_to_js_generic_wrapper`
off; `turboshaft_load_elimination` and `turboshaft_loop_unrolling` stay on (HUMAN, 08-10). `turboshaft_wasm` is off by
default in 12.7. **New defaults in 12.7, left as Chrome 127 ships them (HUMAN to confirm, as with 12.6):** Wasm inlining
(`experimental_wasm_inlining` moved to shipped), `profile_guided_optimization` (+ `_for_empty_feedback_vector`),
`incremental_marking_for_gc_in_background`; `wasm_memory64_trap_handling` only matters with memory64 (still staging).

## 5. Cache epoch

`kFlyWebCacheEpoch` 5 → **6** (MOTOR-2 set 5 for level 127; 1.10 ships with 4). 12.7.224.20 already changes
`Version::Hash()`; the epoch is raised by the rule.

## 6. Tests

See `docs/TAREAS.md`, FS.9.
