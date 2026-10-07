# FlyWeb: V8 12.4.254.15 + ac8da461 (Chrome 124) and its security backports

Since engine level 118 each level carries the V8 of Chrome N (decision of the HUMAN, 05-10; task FM.5). This branch uses
V8 **12.4.254.15 + ac8da461** (`FlyWeb/v8-revision`, Chrome 124.0.6367.207), checked out by `FlyWeb/scripts/build.sh`
(softmac) before `apply_patches`, on top of Chromium 116.0.5845.188. SEGURIDAD, 07-10-2026. From FlyWeb 1.6 the JIT
and WebAssembly run on every site (HUMAN, 07-10), so JIT and Wasm fixes count as much as runtime ones.

## The level-123 set, minus what 12.4 already has

Already in 12.4.254.15: the `js-objects.cc` fix and V8 12.3's own fixes. The later merges of the 12.4 branch
(12.4.254.16-.21: 6e5e1053 CVE-2024-5274, f911ff37 and e7b64c6e for non-JSObjects, 42831144 CVE-2024-4947, f6e569b2 =
ac8da461) are all covered by the patches below or by the pinned revision.

Ported as on level 123 (see that branch): the M120-LTS ports that apply (CVE-2024-7971, CVE-2024-4761, CVE-2024-4947, CVE-2024-5274 (same patches as NUBE's), 73c61498,
872ec583, 8856a2a6, d6287039, ff462a28, b91805d0, 8d0519c8, 41a7b57b, 1e1a2073), CVE-2025-6554 (same patch as NUBE's),
CVE-2025-13223 + 9b5250b9 (by hand: 12.4 adds an `SBXCHECK_GE(length, 0)`, kept), `groupBy` (77df647d + 92aba703),
CVE-2025-5419 + Turboshaft A3/A4/A6 ,
CVE-2026-87491, `kFlyWebCacheEpoch`, Brave's `BUILD.gn`/`src-codegen-compiler.cc`.

## New: Google's M126-LTS branch (V8 12.6.228.x, up to 01-2025)

The M120-LTS used for levels 120-123 stopped in 08-2024; M126-LTS has later fixes for code that 12.4 shares. Applied as
is: c34adcef (TurboFan `SameValue` with `None`), 96932a98 (TurboFan `CallWithSpread`), 7c536445 (interpreter: hole
elision scope in switch jump tables; also without JIT), fd73f2e2 (Liftoff x64 scratch register), e379c539 (Wasm default
externref/exnref), 6e1cc25a (TurboFan, WasmStruct in
`InferHasInPrototypeChain`), 77a4fa1e (Wasm wrapper), 4f4cd3f0 (Wasm streaming module size), 96a23b24 (Wasm Tag
imports), 7acd3517 (parser `HomeObjectScope`), ba6cab40 (Liftoff). By hand: ab08344c (exception prediction with
inlined Wasm frames; skip non-JavaScript summaries in the loop of 12.4's `PredictException`) and 40f9e572
(`JSObject::cast()` on objects that are not JSObjects, e.g. Wasm GC structs: everything except `src/api/` (but for the
`WasmModuleObject` `ToLocal` that the value serializer needs) and the `IsJSApiWrapperObject(HeapObject)` overload, which
12.4 does not have; Google merged it to 12.5 and 12.6 only, after 124
reached its end). Already in 12.4: 79c429b4, 01630b99, f77c44d6, 0bf66c72, 4fbbbb89, 34f357a1. Not applicable: 4cc886d6
(`TryFastAddDataProperty` does not exist in 12.4). Not ported: Maglev fixes
(Maglev is off), ARM-only fixes, JSPI (off), Turboshaft-Wasm, Turboshaft load elimination and loop unrolling (all off in
12.4), 2ec24eb3 (ARM64 pointer authentication), 4b308315 (embedder API), 31c7633a/74d82249 (EPT compaction during
scavenges, written for 12.6's heap; a GC-timing bug that a page cannot drive directly: residual risk).

## Attack surface (flags)

- Maglev off (HUMAN, 05-10) and `maglev_untagged_phis` off (CVE-2026-3910).
- Turboshaft instruction selection off (HUMAN, 06-10; on for x64 in 12.4).
- **New: `wasm_to_js_generic_wrapper` off**, as in V8 12.0-12.2 (on by default since 12.3). It needed later fixes
  (c8c02de5: constructors and other import kinds the generic wrapper does not support; 8d6bd5e1: GC scanning of
  tiered-up wrapper frames) that are large and not in this V8; with it off the per-signature wrappers are used, as in
  FlyWeb 1.6. SEGURIDAD, 07-10-2026; the HUMAN can reverse it.

Tested by SEGURIDAD on a Linux x64 d8 of 12.4.254.15 + ac8da461 with all of these patches: no build errors;
`--no-maglev`, `--no-maglev-untagged-phis`, `--no-turboshaft-instruction-selection`, `--no-wasm-to-js-generic-wrapper`
by default; the same 12 regression tests as level 125 pass; groupBy throws RangeError; **mjsunit 6685/6685**.
**FlyWeb itself is not compiled in the cloud**: LOCAL must build it.
