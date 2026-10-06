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

## On top (as on level 120)

- CVE-2025-6554 (NUBE, `src-interpreter-bytecode-generator.cc.patch`); Brave's `BUILD.gn`/`src-codegen-compiler.cc`;
  `kFlyWebCacheEpoch` (`src-utils-version.h.patch`).
- CVE-2025-13223 + 9b5250b9; groupBy crash fix (77df647d + 92aba703, with this V8's `has_exception()`/`clear_exception()`).
- Maglev off (HUMAN, 05-10) and `maglev_untagged_phis` off (CVE-2026-3910).
- CVE-2025-5419 + Turboshaft A3 (store-store elimination), A4 (`IsOnlyUserOf`), A6
  (`LoadStackArgument`). A2, A5 and A7 are already in 12.2.
- CVE-2024-0519: fixed in V8 12.0 itself. CVE-2026-87491: V8 sandbox escape, not a boundary before Chrome 123.

Tested by SEGURIDAD on a Linux x64 d8 of 12.2.281.22 with all of these patches: no build errors, `--no-maglev`,
`--turboshaft`, the 6 regression tests pass, groupBy throws RangeError, **mjsunit 6536/6536**.
**FlyWeb itself is not compiled in the cloud**: LOCAL must build it.
