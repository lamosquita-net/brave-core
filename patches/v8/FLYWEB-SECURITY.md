# FlyWeb: V8 13.1.201.22 (Chrome 131, no LTS) and its security backports

Since engine level 118 each level carries the V8 of Chrome N (FM.5). This branch uses V8 **13.1.201.22** (9c09e787,
`FlyWeb/v8-revision`: the head of the 13.1 branch and the V8 of Chromium 131.0.6778.265), on top of MOTOR-11's
`nube/motor-131` 9b19e7dafc (V8 step: patches rebased with v8re.sh, two Blink API adaptations).
SEGURIDAD-PORTES-3, 15-10-2026 (task FS.18), for **FlyWeb 1.15**.

**Rule (COORDINACIÓN): 1.15 does not ship with fewer V8 fixes than 1.14** (`seg/v8-13.0` d6b66b7994, epoch 15).
Everything below was compared on a clean 13.1.201.22 (`git apply`, `-R`, by hand).

## 1. FlyWeb 1.14's patches (`seg/v8-13.0`), carried over

All 68 accounted for (MOTOR-11's rebase, reviewed here):

- **62 apply** on a clean 13.1.201.22 (63 in MOTOR-11's step; one dropped below).
- **3 dropped as already in 13.1** (the whole diff reverses cleanly): `ast/scopes.h`, `heap/incremental-marking.cc`,
  `parsing/parser-base.h`.
- **2 moved**: `wasm/module-compiler.cc` and `wasm/module-instantiate.cc` only carried 1.14's adaptation of v8
  924c1a79 (crbug 391907159). 13.1 compiles import wrappers in `WasmImportWrapperCache::CompileWasmImportCallWrapper`,
  as 13.2 does, so the check now sits where upstream's 924c1a79 puts it; the other `MaybeGet()` + `AddWrapper()`
  callers (C API, `wasm-objects.cc`, `runtime-wasm.cc`) get `nullptr` for a dying wrapper and compile a new one, also
  as upstream. Checked: `wasm-code-manager.h` and `wasm-engine.h` reverse cleanly; `wasm-engine.cc` is upstream's
  except the size checks (`WasmEngine` 800 − 40 = 760, `IsolateInfo` 168 as in 13.1). Still not ported: the `SBXCHECK`
  in `WasmDispatchTable::SetForImport`, which in 13.1 takes a `WasmCodePointer`.
- **Dropped here (FS.18): `wasm/wasm-objects.cc`.** It only carried 94e495e1d (crbug 366323508: in
  `GetOrCreateFuncRef`, set up the Wasm-to-JS generic wrapper under `wasm_to_js_generic_wrapper`, not the JS-to-Wasm
  flag). 13.1 removed `wasm_to_js_generic_wrapper` and made `wasm_generic_wrapper` the single switch for both
  directions (upstream's own replacements in `wasm-objects.cc` and `module-instantiate.cc`), so the 13.1 line is
  already right; with the patch, V8 13.1 does not compile.
- **Resolved by MOTOR-11, checked:** `loop-unrolling-reducer.h` is now upstream's 2e96808d as in 13.2 (all its added
  lines present).

## 2. Fixes after 13.1.201.22

13.1.201.22 is the head of its branch (all 13.1 merges included). The merges of 13.2.152.56 (M132 LTS, the last tag),
one by one against 13.1 **with FlyWeb's patches applied**: 20 already in, the rest:

- **Already in FlyWeb, adapted** (only tracing, test-runtime functions or `DCHECK`s of the upstream diff are
  missing): 8834c16a, ca504d09, 2e96808d, 42421010, 20c1b8e9, 5cf18d03, 80600881, 91343bb4 (crbug 390743124),
  924c1a79 (crbug 391907159).
- **Canonical types** (92ed656a, aad03217, 66e97ace, 3fdedec4): 13.1 introduced the relative-index canonicalization
  these fix, and its own branch already carries the same fixes (7615ae1b, 77b73180, a4a402d3, 0e98fadd are ancestors
  of 13.1.201.22).
- **Big WeakMaps** (60092919 reverts b63f8324, main #96798): 13.1 branched at main #96554, before the reland; its
  `FindKeyIndex` has no `Coefficient`. Not applicable.
- **Not applicable with FlyWeb's options:** Maglev (off): 45d40ca2, 97e828af, e5a3e62a, 8131c09b, be40691f, b6f6f8eb,
  d52cf43e, 42889d58. Turboshaft-Wasm (off by default in 13.1): b6d3d1e9, 52f0e1b2, d0e4805f, 72d0b3a6. JSPI central
  stack (d6e86387, 86e6857f), `wasm_tier_mask_for_testing` (2603ba09), ARM64 (385d92ca, 53176fd5), LoongArch/MIPS
  (65a1429d).

**CVE-2026-11645** (exploited, KEV 09-06-2026; crbug 506689381; Chrome 149.0.7827.103): v8 3c869652b0 (main #107009,
May 2026, after 13.2's LTS). `TryFastAddDataProperty` (object model, not the JIT: `--jitless` does not mitigate it)
migrated the object to whatever `Map::PrepareForDataProperty` returned, also a normalized dictionary map, and then
wrote to a field index that no longer exists (OOB read/write). 13.1.201.22 has the function exactly as before the fix
(the 116-era triage said "not applicable" because 11.6–12.1 lacked it). Ported as is in `objects/js-objects.cc`
(SEGURIDAD-PORTES-6, FS.24, 26-10-2026; FlyWeb 2.0.1). Epoch 16 → 17.

## 3. Embedder API between 13.0 and 13.1 (MOTOR-11)

`include/` +186/−41. `Isolate::kImportAssertionDeprecatedSyntax` becomes `kOBSOLETE_…` (case removed from Blink's
use counter); `ModuleRequest::GetImportAssertions()` is deprecated and Blink's `module_record.cc` now calls
`GetImportAttributes()`, as Chromium 131 (the deprecated one only forwarded). No change to sandbox tags, `Internals`
constants, fast API callbacks or the platform; gin untouched. The inspector protocol changes `Debugger.DebugSymbols`
(Wasm debugging in the 116's DevTools only).

## 4. Options

Unchanged from 1.14: `maglev` and `maglev_untagged_phis` off (to decide at level 132), `turboshaft_wasm` off (as
upstream in 13.1), `turboshaft_instruction_selection` on. `wasm_to_js_generic_wrapper` is gone in 13.1 (the generic
wrapper is always used, as FlyWeb already had it). Renamed, same value: `experimental_wasm_inlining` → `wasm_inlining`
(on, as in 1.13 and 1.14). New defaults from upstream, none a new tier: `intel_jcc_erratum_mitigation` (code
padding), `decommit_pooled_pages`; `wasm_deopt` and `wasm_inlining_call_indirect` stay off.

## 5. Cache epoch

`kFlyWebCacheEpoch` **17** (2.0.1, CVE-2026-11645; 16 was 1.15 and 2.0.0, 15 is `seg/v8-13.0`).

## 6. Tests (cloud `d8`, Linux x64, V8 only; `FlyWeb/scripts/v8-d8.sh`)

`d8` 13.1.201.22 with these 62 patches (and, for the `d8` only, without Brave's two `//brave` hooks), release with
DCHECKs, clang of Chromium 116, libstdc++.

- Build: with MOTOR-11's 63 patches V8 13.1 did not compile (`wasm_to_js_generic_wrapper` in the
  `wasm-objects.cc` patch, §1); after dropping it, it builds.
- Options by default (`--print-flag-values`): `--no-maglev`, `--no-maglev-untagged-phis`,
  `--turboshaft-instruction-selection`, `--no-turboshaft-wasm`, `--wasm-generic-wrapper`, `--wasm-inlining`,
  `--no-wasm-deopt`, `--regexp-simd`.
- FlyWeb's regression tests (`FlyWeb/tools/v8-pruebas/`): **8/8**.
- **mjsunit 7279/7280** (default variant). The 1 is `maglev/extras-cped` (asserts Maglev code), as in 1.13 and 1.14.
  The test changes of f6961c40 (lower `kMaxArguments`) were applied first, now including
  `turboshaft/maglev-frontend/regress-359266991` (test-only).
- `mjsunit/wasm/*` with `--stress-wasm-code-gc --wasm-code-gc` (v8 924c1a79 in its new place): **394/394**.
- Not covered by the `d8`: Intl (no ICU) and Chromium 116's libc++. **Not compiled in FlyWeb**: LOCAL builds it.
