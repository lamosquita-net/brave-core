# FlyWeb: V8 12.2.281.22 (Chrome 122) and its security backports

Since engine level 118 each level carries the V8 of Chrome N (decision of the HUMAN, 05-10; task FM.5). This branch uses
V8 **12.2.281.22** (6ac54d1c, `FlyWeb/v8-revision`, the last release of Chrome 122; there is no LTS branch for it),
checked out by `FlyWeb/scripts/build.sh` (softmac) before `apply_patches`, on top of Chromium 116.0.5845.188.
SEGURIDAD, FS.6 (06-10-2026).

## Ported from Google's M120-LTS branch of V8 12.0 (12.0.267.18-.36)

Applied as is: 1e1a2073, 2944ee98 (CVE-2024-4947), 73c61498, 872ec583, 8856a2a6, 8f1c780b (CVE-2024-5274; same
as NUBE's former patches), ae63f970 (CVE-2024-7971), d6287039, de7a07b7 (CVE-2024-4761), ff462a28. From the
original main commit: b91805d0 (8a69c788). By hand: 8d0519c8 (canonical types limit) and 41a7b57b (exception type
canonicalization; `Handle` instead of `DirectHandle`). Already in 12.2.281.22: ce430536, 6feeaeae, Turboshaft A1 (afc18842). Not ported: d5bede9c (Maglev, which is off; an earlier commit of this branch carried only its `maglev-ir.cc` half, which broke a DCHECK in Maglev tests: removed).
In 12.2 the instance passed to `WasmTagObject::New` comes from `trusted_instance_data->instance_object()`.

## From V8's 12.3 branch (Chrome 123), for the JIT opened to every site from FlyWeb 1.6

Decision of the HUMAN (07-10-2026): from 1.6 the JIT (and with it WebAssembly) is no longer limited to the trusted
sites list. SEGURIDAD checked the 15 cherry-picks of V8's 12.3 branch against this 12.2; already here or not
applicable: 9be3957f (CVE-2024-2887), 8caee386 (enum cache, Pwn2Own 2024), 2288a7c1 (TurboFan part; the Turboshaft-Wasm
part is off in 12.2), ef0aebab, 604c9e66, 5de25df8, 81731641. **Ported:**
- **74e61717** (bug 329130358): the GC must visit `WasmInternalFunction::code`, an indirect code pointer since 12.2
  (`src-objects-objects-body-descriptors-inl.h`); without it a Wasm function's code can be freed while in use. Only
  12.2 has this bug (12.0/12.1 do not use that pointer; 12.3 has the fix).
- **69cecb7f** (bug 1507779): no Sparkplug compile in the middle of OSR, which could jump to deoptimized OSR code
  (`src-execution-tiering-manager.cc`).
- **615c099b** (bug 327740539): the home object proxy is resolved after parsing, not off-thread on a main-thread
  `Handle` (`src-ast-*`, `src-parsing-*`; on top of CVE-2024-5274 and CVE-2025-6554).
Not ported: c6026170, 33316df1, c8952db9, c689e68b, 937c5cb0 (experimental features or DevTools only).

## On top (as on level 120)

- CVE-2025-6554 (NUBE, `src-interpreter-bytecode-generator.cc.patch`); Brave's `BUILD.gn`/`src-codegen-compiler.cc`;
  `kFlyWebCacheEpoch` (`src-utils-version.h.patch`).
- CVE-2025-13223 + 9b5250b9; groupBy crash fix (77df647d + 92aba703, with this V8's `has_exception()`/`clear_exception()`).
- Maglev off (HUMAN, 05-10) and `maglev_untagged_phis` off (CVE-2026-3910).
- CVE-2025-5419 + Turboshaft A3 (store-store elimination), A4 (`IsOnlyUserOf`), A6
  (`LoadStackArgument`). A2, A5 and A7 are already in 12.2.
- CVE-2024-0519: fixed in V8 12.0 itself. CVE-2026-87491: V8 sandbox escape, not a boundary before Chrome 123.

Tested by SEGURIDAD on a Linux x64 d8 of 12.2.281.22 with all of these patches: no build errors, `--no-maglev`,
`--turboshaft`, the 6 regression tests pass, groupBy throws RangeError, **mjsunit 6536/6536**; with the 12.3-branch fixes (07-10), the 7 regression tests (with regress-329130358) and **mjsunit 6537/6537**.
**FlyWeb itself is not compiled in the cloud**: LOCAL must build it.
