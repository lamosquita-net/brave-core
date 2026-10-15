# FlyWeb: V8 13.0.245.25 (Chrome 130, no LTS) and its security backports

Since engine level 118 each level carries the V8 of Chrome N (FM.5). This branch uses V8 **13.0.245.25** (3551594a,
`FlyWeb/v8-revision`: the head of the 13.0 branch, merges up to 06-01-2025; the V8 of Chromium 130.0.6723.192 DEPS),
on top of MOTOR-10's `nube/motor-130` a7b5f3f4 (V8 step: patches rebased, gin option B, fast_float).
SEGURIDAD-PORTES-3, 15-10-2026 (task FS.14), for **FlyWeb 1.14**.

**Rule (COORDINACIÓN): 1.14 does not ship with fewer V8 fixes than 1.13** (`seg/v8-12.9` 2ffa1017, epoch 13).
Everything below was compared on a clean 13.0.245.25 (`git apply`, `-R`, by hand).

## 1. FlyWeb 1.13's patches (`seg/v8-12.9`), carried over

All 71 accounted for (MOTOR-10's rebase in a7b5f3f4, reviewed here):

- **61 apply** on a clean 13.0.245.25.
- **10 dropped as already in 13.0**: `wasm-compiler.cc`, `maglev-phi-representation-selector.cc`, `map-inl.h`,
  `map.h`, `runtime-wasm.cc`, `liftoff-assembler-{arm,x64}-inl.h`, `wasm-js.cc` (the whole diff reverses cleanly);
  `builtins-object-gen.cc` (357d0dd4, crbug 383647255: 13.0 has the identical properties-or-hash check) and
  `wasm/wrappers.cc` (`kNoExtern`/`kNoExn` already in the slow path; only the `wasm::` qualifier differs) were
  checked by hand.
- **4 conflicts resolved by MOTOR-10**, checked: `ic.cc` (bc67262 is in 13.0, at lines 3280 and 3557; 279d28029
  kept), `js-to-wasm.tq` and `js-call-reducer.cc` (the a7c018806 parts are in 13.0; 34587b687's `check`s and
  d3577087 + 9209292e kept), `builtins-wasm-gen.cc` (moved builtin, same body).
- a7c018806, 153d4e84e, bc67262 and 357d0dd4 are ancestors of 13.0.245.25. 94e495e1d (`wasm-objects.cc`),
  34587b687 (`js-to-wasm.tq`), fc26d62d0 (instruction-selection phase) and 279d28029 (`ic.cc`) are carried.

## 2. Fixes after 13.0.245.25

13.0.245.25 is the head of its branch. The 72 merges of 13.1.201.22 and 13.2.152.56 (M132 LTS, the last tag), one
by one against 13.0 **with FlyWeb's patches applied**: 26 already in, 11 apply, 35 conflict.

**New in this branch:**

- **v8 924c1a79** (crbug 391907159, M132-LTS): replace the `dead_code_` set with an `is_dying` bit, and never hand
  out a dying wrapper from the import wrapper cache. 13.0's wrapper cache is process-wide (12.9's was per module, so
  1.13 is not affected): a wrapper that became potentially dead could be revived from the cache and, once in
  `dead_code_`, be freed when its count dropped again without a stack scan (use after free). 13.0 has no
  `CompileWasmImportCallWrapper`; the check is in `MaybeGet()`, the fallback compile in `module-instantiate.cc` and
  `CompileImportWrapperForTest`. Not ported: the `SBXCHECK` in `WasmDispatchTable::SetForImport`, which in 13.0
  takes a call target, not a `WasmCode*`.
- **v8 91343bb4** (crbug 390743124, M132-LTS): TurboFan escape analysis must not elide objects holding a
  `TrustedHeapConstant` (`AllocationBuilder` stores them since 12.9; mixed with ordinary `HeapConstant`s in a Phi
  they were mis-tagged by decompression elimination: memory corruption). **Also missing in 1.13** (12.9 has the
  same `allocation-builder.h`; 12.8 does not): reported to COORDINACIÓN on 15-10.

**Already in FlyWeb, adapted** (only tracing, test-runtime functions or `DCHECK`s of the upstream diff are
missing): 8834c16a, ca504d09, 2e96808d, 42421010, 20c1b8e9, 5cf18d03, 80600881.

**Not applicable** with FlyWeb's options and 13.0's code:

- Maglev (off): 14bbcf10/d52cf43e, a4912252/9fed86ea, 45d40ca2, 97e828af, e5a3e62a, 8131c09b, be40691f, b6f6f8eb,
  42889d58.
- Turboshaft-Wasm (off by default in 13.0): WasmGCTypeAnalyzer 576543f8/52f0e1b2, `array.fill`/`array.new`
  b6d3d1e9, `call_indirect` inlining d0e4805f/72d0b3a6, `wasm_deopt` 39db300e.
- Canonical types (7615ae1b/92ed656a, 77b73180/aad03217, a4a402d3/66e97ace, 0e98fadd/3fdedec4): 13.0 still
  compares whole `TypeDefinition`s with `is_relative_supertype`, as 12.9.
- Not in 13.0: the global ATOM regexp cache (33a0366d), big WeakMaps (60092919 reverts them).
- 53ad46b4: 13.0 already bails out when the implicit argument is not a `WasmTrustedInstanceData`
  (`js-call-reducer.cc:3841`).
- Central-stack switching for JSPI (d6e86387, 86e6857f), `wasm_tier_mask_for_testing` (2603ba09), ARM64/BTI,
  big-endian, LoongArch/MIPS, gcc, `d8` (97f474b2).

## 3. Embedder API between 12.9 and 13.0

`git diff 12.9.202.28 13.0.245.25 -- include/` (+409/−204):

- `v8::Platform::GetForegroundTaskRunner(Isolate*, TaskPriority)` is now the pure virtual; MOTOR-10 overrides it
  in gin and returns the same task runner for every priority (option B of FM.20, never `nullptr`). Checked.
  `cppgc::Platform` gains a priority overload with a default; no subclass in the 116.
- No change to external-pointer or sandbox tags, `Internals` constants, fast API callbacks or
  `ExceptionPropagationCallback`. `TracedReference`, `MemorySpan` and `WrappedIterator`: types and C++20
  operators only.
- `AllowCodeGenerationFromStringsCallback` removed (unused by the 116), `Isolate::ReportExternalAllocationLimitReached`
  renamed (not called by Blink), `Module::InstantiateModule` gains an optional source-phase callback,
  `PageAllocator::SealPages` (virtual, default `false`).

## 4. Options

Unchanged from 1.13: `maglev` and `maglev_untagged_phis` off (to decide at level 132), `turboshaft_wasm` off (as
upstream in 13.0), `turboshaft_instruction_selection` and `wasm_to_js_generic_wrapper` on.

New defaults in 13.0 that touch code generation, all upstream's, none turns on a new optimizing tier:

- `regexp_simd` = true: SIMD in irregexp's `SkipUntilBitInTable`, only with SSSE3; AVX encodings only if the CPU has
  AVX (the MacPro5,1 has SSSE3/SSE4.2 and no AVX). To be checked on the 5,1 by LOCAL.
- `turboshaft_csa` = true: builtins are generated with Turboshaft at build time (mksnapshot), not at run time.
- `turbo_inline_js_wasm_calls` stays true; `turboshaft_wasm_in_js_inlining` is experimental (off).

## 5. Cache epoch

`kFlyWebCacheEpoch` **15** (13 is `seg/v8-12.9`; 14 was reserved and never shipped).

## 6. Tests (cloud `d8`, Linux x64, V8 only; `FlyWeb/scripts/v8-d8.sh`)

Pending.
