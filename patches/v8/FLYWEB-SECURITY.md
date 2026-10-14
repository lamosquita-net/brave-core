# FlyWeb: V8 12.9.202.28 (Chrome 129, no LTS) and its security backports

Since engine level 118 each level carries the V8 of Chrome N (FM.5). This branch uses V8 **12.9.202.28** (f372edd3,
`FlyWeb/v8-revision`: the head of the 12.9 branch, merges up to 08-10-2024), on top of MOTOR-8's `nube/motor-129`
b589e86 (step 1: V8 12.9 and the 116 adapted to its API). SEGURIDAD-PORTES, 14-10-2026 (task FS.13), for **FlyWeb 1.13**.

**Rule (COORDINACIÓN): 1.13 does not ship with fewer V8 fixes than 1.12.x** (`seg/v8-12.8` 5aa57049f, epoch 12).
Everything below was compared on a clean 12.9.202.28 (`git apply`, `-R`, by hand).

## 1. FlyWeb 1.12.x's patches (`seg/v8-12.8`), carried over

All 70 accounted for:

- **55 apply unchanged** (MOTOR-8 rebased them in b589e86).
- **6 are already in 12.9** (the whole diff reverses cleanly): `accessors.cc` (6b85b7c6b), `heap-refs.cc`,
  `isolate.cc`, `value-serializer.{cc,h}` (e8d92cc), `graph-builder-interface.cc`.
- **9 conflicted**; by hand in this branch:
  - `builtins-object-gen.cc`: 357d0dd4 (crbug 383647255, `Object.assign` must not overwrite the identity hash),
    placed before 12.9's reorganised clone-map lookup.
  - `access-builder.{h,cc}` + `js-native-context-specialization.cc`: CVE-2025-13223 + 9b5250b9 (the
    representation-aware `ForPropertyArraySlot`); 12.9 still loads extended property backing stores as tagged slots.
  - `ic.cc`: bc67262 (crbug 364422411) moved to 12.9's `SideStepTransition` lookup in `GetCloneTargetMap`, which had
    no native-context check at all; 279d28029 (crbug 563235411) unchanged.
  - `bytecode-generator.cc`: the `switch` hole-check elision fix, only `base::Optional` → `std::optional` around it.
  - `snapshot-table-opindex.h`: the `begin()`/`end()` of the CVE-2025-5419 store-store fix, same change.
  - Already in 12.9: the rest of `js-to-wasm.tq` (converted reference parameters), of `access-info.cc` and
    `js-native-context-specialization.cc` (`IsJSObject()` on prototypes) and of `js-objects.cc` (6b85b7c6b).

## 2. Fixes after 12.9.202.28

The 81 merges of the 13.0.245.25, 13.1.201.22 and 13.2.152.56 (M132 LTS) branches, one by one against this tree:
31 are already in 12.9; the ones that apply to code 12.9 runs were already in FlyWeb's set (357d0dd4, bc67262,
8834c16, 2e96808, ca504d09, …), plus fc26d62d0 now that Turboshaft instruction selection is on (§4). **Not applicable** with FlyWeb's options: Maglev (off), Turboshaft-Wasm (off by
default in 12.9: `WasmGCTypeAnalyzer`, `array.fill`), `call_indirect` inlining and `wasm_deopt` (need Turboshaft-Wasm),
JSPI and central-stack switching (experimental; Blink 116 has no JSPI origin trial), the 13.x canonical-type fixes (12.9 still compares whole `ValueType`s with the
relative bit and `is_relative_supertype`), the global ATOM regexp cache (born in 13.0), the process-wide import
wrapper cache (`is_dying_`, 391907159: 12.9's cache is per module), 53ad46b44 (12.9 already bails out when the
exported function's implicit argument is not a `WasmTrustedInstanceData`), ARM/LoongArch/big-endian and `d8`.

## 3. Embedder API between 12.8 and 12.9

- `FastApiCallbackOptions::fallback` and `wasm_memory` are gone: fast API callbacks must throw themselves. Blink's
  side (8d78666bade + d9bfd74484c, MOTOR-8) gets its own review (FS.13 item 3).
- `experimental_report_exceptions_from_callbacks` defaults to true but only runs with an
  `ExceptionPropagationCallback`, which neither Blink 116 nor 128 installs.
- `ZoneBackingAllocator` and `new` in cppgc mixins: build-time only.
- `Internals` constants inlined by the embedder change (isolate offsets, root addresses, string map range): Blink is
  built against these headers, so they match.

## 4. Options

`maglev` and `maglev_untagged_phis` stay off, as in 1.7.1–1.12.x (to be decided at level 132, V8 13.2 LTS); load
elimination, loop unrolling and Wasm inlining on; `turboshaft_wasm` off, as upstream in 12.9. **Back to upstream's
defaults (decisions of the HUMAN, 14-10-2026; principle: what FlyWeb turns off comes back on once upstream ships it
and it is mature):**

- **`turboshaft_instruction_selection` = true** (upstream since 12.5). With it off, 12.9 runs TurboFan's instruction
  selection on the recreated schedule, a path Chrome no longer uses for JavaScript, and it does not handle 12.9's
  `kTrustedHeapConstant` (`ToConstant()` → `UNREACHABLE`): 100 mjsunit failures out of the first 2123 tests and
  claude.ai crashing in LOCAL's Static. Its only fix in the 13.0–13.2 merges, **fc26d62d0** (crbug 383661627,
  `SparseBitVector` in `ComputeLoopInfo`), is carried.
- **`wasm_to_js_generic_wrapper` = true** (upstream since 12.3). With it off but `wasm_generic_wrapper` on,
  `GetOrCreateFuncRef` still used the generic wrapper for re-exported imports (also in 12.8, FlyWeb 1.12.x; no failure
  in `d8`, but the switch was not complete). Carried with it: **153d4e84e** (crbug 371565065, no wrapper tier-up when
  the signature depends on another instance), **94e495e1d** (crbug 366323508, the right flag in `GetOrCreateFuncRef`)
  and **34587b687** (`check` instead of `dcheck` on the value kinds in both generic wrappers: heap-sandbox hardening).

**Also carried: a7c018806** (crbug 364917763, never merged to a release branch): the JS-to-Wasm conversion let
non-null values into null-only reference types (`nullexternref`, `nullexnref`) in the generic wrapper, the TurboFan
wrapper and the Turboshaft one.

## 5. Cache epoch

`kFlyWebCacheEpoch` **13** (12 is `seg/v8-12.8`).

## 6. Tests (cloud `d8`, Linux x64, V8 only; `FlyWeb/scripts/v8-d8.sh`)

- Options as above by default (`--print-flag-values`: `--no-maglev`, `--no-maglev-untagged-phis`,
  `--turboshaft-instruction-selection`, `--wasm-to-js-generic-wrapper`).
- FlyWeb's regression tests and upstream ones (including the embedder-API pass of 1.12.x): **17/17**.
- **mjsunit 7216/7217**: the 1 is `maglev/extras-cped` (asserts Maglev code). With the old options (TurboFan
  instruction selection) the run stopped at 100 failures out of 2123 tests, all `UNREACHABLE` in `ToConstant()`.
- Not covered by the `d8`: Intl (no ICU) and Chromium 116's libc++. **Not compiled in FlyWeb**: LOCAL builds it.

