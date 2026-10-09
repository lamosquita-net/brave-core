# FlyWeb: V8 12.6.228.49 (Chrome 126 + M126-LTS) and its security backports

Since engine level 118 each level carries the V8 of Chrome N (decision of the HUMAN, 05-10; task FM.5). This branch uses
V8 **12.6.228.49** (0b23496f, `FlyWeb/v8-revision`: Chrome 126.0.6478.182 plus Google's M126-LTS merges), checked out by
`FlyWeb/scripts/build.sh` (softmac) before `apply_patches`, on top of Chromium 116.0.5845.188. SEGURIDAD, 08-10-2026
(task FS.8). From FlyWeb 1.6 the JIT and WebAssembly run on every site (HUMAN, 07-10), so JIT and Wasm fixes count as
much as runtime ones.

## What 12.6.228.49 already has

The whole M126-LTS branch (up to 01-2025): every M126-LTS fix that levels 124/125 needed as a backport is native here
(c34adcef, 96932a98, 6e1cc25a, 7c536445, fd73f2e2, ba6cab40, e379c539, ab08344c, 77a4fa1e, 4f4cd3f0, 96a23b24,
7acd3517, 4cc886d6, 40f9e572...), and so are the M120-LTS ones (CVE-2024-7971, CVE-2024-4761, CVE-2024-5274...), the
`js-objects`/`map` and instruction-selector fixes, the Turboshaft `LoadStackArgument` fix (v8 96493c74, same code), and
c8c02de5 / 8d6bd5e1 / 31c7633a / 74d82249 (generic wasm-to-js wrapper fixes and EPT compaction during scavenges, all in
the history of 12.6.228.49). Checked by applying the 12.5 patch set to a clean 12.6.228.49 (`git apply -R --check`
per file): of its 42 patch files, 25 are already in.

## Patches in this directory

| Patch | What | Status on 12.6.228.49 |
|---|---|---|
| `BUILD.gn.patch`, `src-codegen-compiler.cc.patch` | Brave (chromium_src include dir, PageGraph eval hook) | NUBE's rebase |
| `src-interpreter-bytecode-generator.cc.patch` | CVE-2025-6554 (exploited): TDZ hole-check elision across optional chains (M132-LTS f3962853) | NUBE's, identical to mine on 12.5 (the switch hunks are native) |
| `src-utils-version.h.patch` | `kFlyWebCacheEpoch` in `Version::Hash()` | NUBE's; epoch 4 since LOCAL's `gni-v8.gni.patch` (rule: every change to `patches/v8` raises it) |
| `gni-v8.gni.patch` | Build only (LOCAL, 502c090f): `v8_use_perfetto` keeps 12.5's condition `build_with_chromium && use_perfetto_client_library`; the 116 has no Perfetto chrome/v8 protos on Mac | patched file = 12.5.227.13's `gni/v8.gni` except one comment line; V8 without Perfetto, as in the tested `d8`. No security effect |
| `src-compiler-js-native-context-specialization.cc.patch`, `src-compiler-access-builder.{cc,h}.patch` | CVE-2025-13223 (exploited, TurboFan `BuildExtendPropertiesBackingStore`) + 9b5250b9 (crbug 475479135, the representation dependency) | as on 12.5 (the `InferHasInPrototypeChain` hunk is native) |
| `src-compiler-turboshaft-store-store-elimination-reducer-inl.h.patch` | CVE-2025-5419 (exploited; 7bc0a67e = M132-LTS 80600881) + Turboshaft store-store elimination loop fixes (67c8f3a9) | as on 12.5 |
| `src-compiler-turboshaft-operations.cc.patch` | `IsOnlyUserOf` with saturated use counts | as on 12.5 |
| `src-builtins-builtins-wasm-gen.cc.patch`, `src-builtins-wasm.tq.patch`, `src-builtins-builtins-definitions.h.patch` | CVE-2026-87491 (exploited, 08-09-2026): `WasmGetOwnProperty` in CSA without invoking getters (v8 36079c36) | as on 12.5 |
| `src-builtins-builtins-collections-gen.{cc,h}.patch`, `src-builtins-object-groupby.tq.patch`, `src-runtime-runtime-collections.cc.patch` | `Object.groupBy`/`Map.groupBy` with too many groups: `RangeError`, not a crash (77df647d + 92aba703) | as on 12.5 |
| `src-flags-flag-definitions.h.patch` | Attack-surface options, see below | |
| `src-compiler-turboshaft-late-load-elimination-reducer.cc.patch`, `src-compiler-turboshaft-snapshot-table-opindex.h.patch` | M132-LTS 5cf18d03 (crbug 417169470): a map store invalidates the known maps of **all** objects, not only of its base (they can alias). Needed because load elimination is on here (upstream's tracing code left out) | new on this level |
| 12 new files: `src-codegen-handler-table.cc`, `src-bigint-fromstring.cc`, `src-compiler-representation-change.cc`, `src-objects-js-date-time-format.cc`, `src-compiler-access-info.cc`, `src-compiler-property-access-builder.cc`, `src-deoptimizer-deoptimizer.cc`, `src-objects-code.h`, `src-parsing-preparser.cc`, `src-compiler-js-call-reducer.cc`, `src-compiler-js-inlining.cc`, `src-objects-js-regexp.cc` | M132-LTS, see next section | new on this level |

## New on this level: Google's M132-LTS branch (V8 13.2.152.x, up to 13.2.152.56, 09-2025)

M126-LTS ended in 01-2025; the next Google LTS is M132-LTS. Its 51 merges since 11-2024 (the `[M132-LTS]` ones and the
Chrome 132 stable merges) were classified against the clean 12.6.228.49 (`git apply --check`, `-R --check`, and by hand
for the conflicts); the table is in `FlyWeb/docs/cve-triage.md` (section "Nivel 126") and in `docs/TAREAS.md` (FS.8).
Ported:

- 2b2a58c2 (crbug 420637585): TurboFan converts Smi to Word64 with zero extension when the type range is non-negative.
- 1c7ff4d5 (crbug 390465670): `LoadField`'s type from a recorded `FieldType` depends on map stability.
- ca504d09 (crbug 385155406) and f6961c40 (reland of "Lower the maximum JS parameter count"): `kMaxArguments` 65534 ->
  65526; bound functions whose argument count would overflow the 16-bit input count are not inlined; `CHECK` in
  `CreateArtificialFrameState`. The upstream test changes (`regress-11491`, `regress-crbug-724153`) were applied to the
  mjsunit tests.
- 20c1b8e9: `EscapeRegExpSource` counted escapes in an `int` (overflow with a huge `source`); now `uint32_t`.
- 42421010 (crbug 430344952): preparser: `eval` is still `eval`.
- d6a6e7c3 (crbug 380308813): deoptimizer handler for a Wasm `externref` return value.
- 725b6a21 (crbug 427600180) and 3bdf541a (crbug 443765373): release `CHECK`s (BigInt `FromString` against concurrent
  in-sandbox mutation; handler-table offsets fit the bitfield).
- 815da990 (crbug 386857213): out-of-bounds read in the time-zone offset parser of `Intl.DateTimeFormat`. **Not tested in
  `d8`** (the `d8` is built without Intl); the change is one length check.

Not applicable to 12.6: 91343bb4 (no `TrustedHeapConstant` yet), ccc23e07 (no `Object.assign` fast path yet), 8834c16a
(no `FastCloneJSObject`), 01a74bae (no `reuse_scope_infos`), 924c1a79 (`dead_code_` and the import-wrapper cache of
13.x), 92ed656a/aad03217/66e97ace/3fdedec4 (the type canonicalizer of 13.x; 12.6 compares types with
`StructType::operator==`, which includes mutability), d0e4805f/72d0b3a6 (call_indirect inlining: `turboshaft_wasm` and
the experimental flag, both off), 2603ba09. Native or equal: 80600881, f3962853, 9209292e, 3c2d220a, b27e7ac0, 97e828af,
7e36549f. Not ported on purpose: Maglev (off), ARM, JSPI (off: d6e86387, 86e6857f), Turboshaft-Wasm (off), Turboshaft
loop unrolling for huge Wasm functions (2e96808d), 5a7e02c2 (Intl: out-of-resources `FATAL` ->
exception, availability only), fc26d62d, 65a1429d.

## Attack surface (flags)

- Maglev off (HUMAN, 05-10) and `maglev_untagged_phis` off (CVE-2026-3910).
- Turboshaft instruction selection off (HUMAN, 06-10).
- `wasm_to_js_generic_wrapper` off, as on 1.7.1-1.9 and before (SEGURIDAD, 07-10). V8 12.6 has the later fixes
  (c8c02de5, 8d6bd5e1), so it *could* be on; it stays off for coherence with the earlier levels. The HUMAN can reverse it.
- **`turboshaft_load_elimination` and `turboshaft_loop_unrolling` stay ON**, as in V8 12.6 (they are on by default since
  12.6; off in 12.0-12.5). Decision of the HUMAN, 08-10-2026 ("we move forward, not back"). SEGURIDAD had left them off
  as a precaution; with them on, the load-elimination fix of the M132-LTS (5cf18d03) is ported (see above) and the
  loop-unrolling fix 2e96808d is not (it is about huge Wasm functions; 12.6's unroller has no unroll count, and
  `turboshaft_wasm` is off). A flag-by-flag comparison of 12.5.227.13 and 12.6.228.49 shows nothing else changes its
  default (`enable_avx_vnni` is new, but it is a CPU feature switch tested at run time, not a requirement).

## Tests (SEGURIDAD, 08-10-2026)

On a Linux x64 `d8` of 12.6.228.49 built with `FlyWeb/scripts/v8-d8.sh` (Release, `dcheck_always_on`, no Intl) with exactly
these patches (the 27 non-Brave files apply to the clean tree and give the tested tree byte for byte):

- `d8` version 12.6.228.49; `--no-maglev`, `--no-maglev-untagged-phis`, `--no-turboshaft-instruction-selection`,
  and `--no-wasm-to-js-generic-wrapper` by default; `--turboshaft-load-elimination` and `--turboshaft-loop-unrolling` on, as in V8.
- Regression tests (`FlyWeb/tools/v8-pruebas/`, upstream's `regress-420636529`, `regress-543557673`, `regress-430344952`,
  the groupBy test with 17 M groups -> `RangeError`, CVE-2025-13223 and CVE-2024-7971 tests, the M126-LTS ones that
  ship with the tree): all pass.
- **mjsunit: 6810/6810** (`tools/run-tests.py --outdir=out/x64 -j4 mjsunit`), with load elimination and loop unrolling on (second round, after the HUMAN decision).
- Ad-hoc sanity checks of the ported behaviour (regexp `source` escapes, parameter limit, escaped `eval`, `bind` with
  30 000 arguments).

**FlyWeb itself is not compiled in the cloud**: LOCAL must build it. Also for LOCAL: the PDFium patches of
`patches/third_party/pdfium/` must apply on PDFium `6c2c8ce8` (the DEPS revision of Chromium 116), which the cloud
container cannot reach; `chk116.py` (softmac) checks the Chromium/Blink patches only: 1056/1056 apply on 116.0.5845.188 (at 2697eff3).
