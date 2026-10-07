# FlyWeb: V8 12.3.219.16 (Chrome 123) and its security backports

Since engine level 118 each level carries the V8 of Chrome N (decision of the HUMAN, 05-10; task FM.5). This branch uses
V8 **12.3.219.16** (1191d73c, `FlyWeb/v8-revision`, the last release of Chrome 123; there is no LTS branch for it),
checked out by `FlyWeb/scripts/build.sh` (softmac) before `apply_patches`, on top of Chromium 116.0.5845.188.
SEGURIDAD, FS.6 (06-10-2026).

## Ported from Google's M120-LTS branch of V8 12.0 (12.0.267.18-.36)

Applied as is: 1e1a2073, 2944ee98 (CVE-2024-4947), 73c61498, 872ec583, 8856a2a6, 8f1c780b (CVE-2024-5274; same
as NUBE's former patches), ae63f970 (CVE-2024-7971), d6287039, de7a07b7 (CVE-2024-4761), ff462a28. From the
original main commit: b91805d0 (8a69c788). By hand: 8d0519c8 (canonical types limit) and 41a7b57b (exception type
canonicalization; `Handle` instead of `DirectHandle`). Already in 12.3.219.16: also 1e1a2073's neighbours graph-assembler/map-updater/module-decoder fixes; ce430536, 6feeaeae, Turboshaft A1 (afc18842). Not ported: d5bede9c (Maglev, which is off; an earlier commit of this branch carried only its `maglev-ir.cc` half, which broke a DCHECK in Maglev tests: removed).
As in 12.2, the instance passed to `WasmTagObject::New` comes from `trusted_instance_data->instance_object()`.

## FlyWeb 1.7.1: Google's M126-LTS fixes and the generic wasm-to-js wrapper (SEGURIDAD, 07-10-2026)

Google's M120-LTS (used above) stopped in 08-2024; M126-LTS (V8 12.6.228.x, up to 01-2025) has later fixes for code that
12.3 shares, and the JIT and WebAssembly run on every site since FlyWeb 1.6. Applied as is: c34adcef (TurboFan
`SameValue` with `None`), 96932a98 (TurboFan `CallWithSpread`), 7c536445 (interpreter: hole elision scope in switch jump
tables; also without JIT), fd73f2e2 (Liftoff x64), e379c539 (Wasm default externref/exnref), 6e1cc25a (TurboFan,
WasmStruct in `InferHasInPrototypeChain`), 77a4fa1e (Wasm wrapper), 4f4cd3f0 (Wasm streaming module size), 96a23b24
(Wasm Tag imports), 7acd3517 (parser `HomeObjectScope`), ba6cab40 (Liftoff). By hand: ab08344c (skip non-JavaScript
summaries in `PredictException`, JS-to-Wasm inlining is on), 40f9e572 (`JSObject::cast()` on objects that are not
JSObjects, e.g. Wasm GC structs; without `src/api/` but for the `WasmModuleObject` `ToLocal`), 8d6bd5e1 (GC scanning of
the tagged parameters of tiered-up wasm-to-js wrapper frames) and c8c02de5 (only use the generic wasm-to-js wrapper for
funcrefs of imports whose call kind it supports, e.g. not constructors; 12.3 had dropped 12.2's
`setup_new_ref_with_generic_wrapper` switch, which comes back decided that way). Not applicable: 4cc886d6
(`TryFastAddDataProperty` does not exist in 12.3).

**`wasm_to_js_generic_wrapper` off**, as in V8 12.0-12.2 (FlyWeb 1.6) and on levels 124-125: it is new and on by default
in 12.3; with the switch above it can be off without inconsistencies (it could not before: 12.3 always used the generic
wrapper for funcrefs of imports). c8c02de5 and 8d6bd5e1 stay as defense in depth.

## On top (as on level 120)

- CVE-2025-6554 (NUBE, `src-interpreter-bytecode-generator.cc.patch`); Brave's `BUILD.gn`/`src-codegen-compiler.cc`;
  `kFlyWebCacheEpoch` (`src-utils-version.h.patch`).
- CVE-2025-13223 + 9b5250b9; groupBy crash fix (77df647d + 92aba703, with this V8's `has_exception()`/`clear_exception()`).
- Maglev off (HUMAN, 05-10) and `maglev_untagged_phis` off (CVE-2026-3910).
- CVE-2025-5419 + Turboshaft A3 (store-store elimination), A4 (`IsOnlyUserOf`), A6
  (`LoadStackArgument`). A2, A5 and A7 are already in 12.3.
- CVE-2024-0519: fixed in V8 12.0 itself.
- **New with 12.3 (Chrome 123, where the V8 sandbox becomes a security boundary): CVE-2026-87491** (crbug 543557673,
  exploited): `WasmGetOwnProperty` moved from Torque to CSA without invoking getters (v8 36079c36), keeping the old
  interface (context parameter) so Liftoff, TurboFan and Turboshaft callers are unchanged
  (`src-builtins-wasm.tq`, `src-builtins-builtins-definitions.h`, `src-builtins-builtins-wasm-gen.cc` patches).
- **Turboshaft instruction selection off on x64** (`turboshaft_instruction_selection` = `false`, decision of the HUMAN,
  06-10-2026): new in 12.3; TurboFan's instruction selection keeps the JIT surface covered on levels 120-122.

Tested by SEGURIDAD on a Linux x64 d8 of 12.3.219.16 (built with `v8_expose_memory_corruption_api`) with all of these
patches: no build errors, `--no-maglev`, `--turboshaft`, `--no-turboshaft-instruction-selection`, the 6 regression tests
pass, groupBy throws RangeError, **mjsunit 6618/6618**. The upstream test of CVE-2026-87491 (regress-543557673) cannot
validate the fix on 12.3: it corrupts sandbox memory using 2026 object layouts and crashes before its check.
**FlyWeb itself is not compiled in the cloud**: LOCAL must build it.
**1.7.1 (07-10-2026):** on a 12.3.219.16 d8 with all of the above: 12 regression tests (also the M126-LTS ones) pass with
the generic wrapper off and on, groupBy throws RangeError, **mjsunit 6623/6623**, and the Wasm tests with
`--wasm-to-js-generic-wrapper` 751/751.
