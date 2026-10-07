# FlyWeb: V8 12.5.227.13 (Chrome 125) and its security backports

Since engine level 118 each level carries the V8 of Chrome N (decision of the HUMAN, 05-10; task FM.5). This branch uses
V8 **12.5.227.13** (fbcea6ee, `FlyWeb/v8-revision`, Chrome 125.0.6422.141), checked out by `FlyWeb/scripts/build.sh`
(softmac) before `apply_patches`, on top of Chromium 116.0.5845.188. SEGURIDAD, 07-10-2026. From FlyWeb 1.6 the JIT
and WebAssembly run on every site (HUMAN, 07-10), so JIT and Wasm fixes count as much as runtime ones.

## The level-123 set, minus what 12.5 already has

Already in 12.5.227.13: CVE-2024-5274 (scopes/parser), CVE-2024-4947 (access-info, Maglev), the IC/`Array` fixes for
non-JSObjects (`ic.cc`, `builtins-array.cc`), the `js-objects.cc` fix and V8 12.3's own fixes. The 12.5 branch has two
later Wasm merges (2f810041 adds a path for imports through the generic js-to-wasm wrapper, fa25b906 turns it off
again): .13 never had that path, so nothing to port.

Ported as on level 123 (see that branch): the M120-LTS ports that apply (CVE-2024-7971, CVE-2024-4761, 73c61498,
872ec583, 8856a2a6, d6287039, ff462a28, b91805d0, 8d0519c8, 41a7b57b, 1e1a2073), CVE-2025-6554 (same patch as NUBE's),
CVE-2025-13223 + 9b5250b9 (by hand: 12.5 adds an `SBXCHECK_GE(length, 0)`, kept), `groupBy` (77df647d + 92aba703),
CVE-2025-5419 + Turboshaft A3/A4/A6 (store-store elimination lives in `store-store-elimination-reducer-inl.h` in 12.5),
CVE-2026-87491, `kFlyWebCacheEpoch`, Brave's `BUILD.gn`/`src-codegen-compiler.cc`.

## New: Google's M126-LTS branch (V8 12.6.228.x, up to 01-2025)

The M120-LTS used for levels 120-123 stopped in 08-2024; M126-LTS has later fixes for code that 12.5 shares. Applied as
is: c34adcef (TurboFan `SameValue` with `None`), 96932a98 (TurboFan `CallWithSpread`), 7c536445 (interpreter: hole
elision scope in switch jump tables; also without JIT), fd73f2e2 (Liftoff x64 scratch register), e379c539 (Wasm default
externref/exnref), ab08344c (exception prediction with inlined Wasm frames), 6e1cc25a (TurboFan, WasmStruct in
`InferHasInPrototypeChain`), 77a4fa1e (Wasm wrapper), 4f4cd3f0 (Wasm streaming module size), 96a23b24 (Wasm Tag
imports), 7acd3517 (parser `HomeObjectScope`), ba6cab40 (Liftoff), 4cc886d6 (`TryFastAddDataProperty` with deprecated
maps). Already in 12.5: 79c429b4, 01630b99, f77c44d6, 0bf66c72, 40f9e572, 4fbbbb89, 34f357a1. Not ported: Maglev fixes
(Maglev is off), ARM-only fixes, JSPI (off), Turboshaft-Wasm, Turboshaft load elimination and loop unrolling (all off in
12.5), 2ec24eb3 (ARM64 pointer authentication), 4b308315 (embedder API), 31c7633a/74d82249 (EPT compaction during
scavenges, written for 12.6's heap; a GC-timing bug that a page cannot drive directly: residual risk).

## Attack surface (flags)

- Maglev off (HUMAN, 05-10) and `maglev_untagged_phis` off (CVE-2026-3910).
- Turboshaft instruction selection off (HUMAN, 06-10; on for every architecture in 12.5).
- **New: `wasm_to_js_generic_wrapper` off**, as in V8 12.0-12.2 (on by default since 12.3). It needed later fixes
  (c8c02de5: constructors and other import kinds the generic wrapper does not support; 8d6bd5e1: GC scanning of
  tiered-up wrapper frames) that are large and not in this V8; with it off the per-signature wrappers are used, as in
  FlyWeb 1.6. SEGURIDAD, 07-10-2026; the HUMAN can reverse it.

Tested by SEGURIDAD on a Linux x64 d8 of 12.5.227.13 with all of these patches: no build errors; `--no-maglev`,
`--no-maglev-untagged-phis`, `--no-turboshaft-instruction-selection`, `--no-wasm-to-js-generic-wrapper` by default; 12
regression tests pass (those of the level-123 set plus the M126-LTS ones: regress-385386138, -374627491, -367818758,
-359949835, -378779897); groupBy throws RangeError; **mjsunit 6748/6748**. **FlyWeb itself is not compiled in the
cloud**: LOCAL must build it.
