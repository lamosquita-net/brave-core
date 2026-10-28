# Maglev debt for engine level 134 (V8 13.4)

SEGURIDAD-PORTES-11, 28-10-2026. Directive (Titín, 27-10): Maglev on by level 134 at the latest, with its security debt
ported first. These `git format-patch` files apply with `git am` on a **clean V8 13.4.114.21** (4f282ae4, head of
`branch-heads/13.4`). They are a series, not `patches/v8`: whoever builds the level 134 V8 applies them on top of the
13.4 tree, then regenerates the per-file `patches/v8` the usual way. Maglev itself **stays off** here; turning it on is
the `maglev` line of `src-flags-flag-definitions.h.patch` (keep `maglev_untagged_phis` off), after the measurements on
the 5,1 and 6,1.

## Inventory (COORDINACIÓN-18 criterion: merged to a release branch, x64, not only Turbolev)

All 92 originals behind the 147 merges that touch Maglev in branches 13.2 (LTS) and 13.5–15.8, checked one by one
against 13.4: **16 Maglev fixes** (+2 TurboFan-only ones), 10 already in 13.4, 4 only needed with untagged phis, 57 not
applicable (the code they fix does not exist in 13.4: optimizer, KNA, HAMT, hole tracking, range analysis, Turbolev,
other architectures). Fixes on main never merged to a release branch are out of scope.

| # | v8 (main) | Merged as | crbug | How |
|---|---|---|---|---|
| 1 | 9315baa436 | 13.5 a0f977f182 | 400944465 | as is |
| 2 | 7ef5ae531a | 13.8 ddc4473ba6 | 457351015 | DEBUG check with `has_register()` |
| 3 | f22ca7b61a | 13.8 99209d9542 | 430572435 | check in `ShouldInlineCall` |
| 4 | 618d1a0290 | 14.0 645f3356d3 | 444048019 | bail-out in `MaglevCompiler::Compile` before graph building; `kMaxInputs` made public |
| 5 | 24ae3662bf | 14.0 07ad1d3ae2 | 441668149 | **equivalent, not the abort machinery**: no Maglev for bytecode ≥ `kMaxInputs / 2` or over `max_optimized_bytecode_size`, which bounds every merge's predecessor count |
| 6 | 10f09c4018 | 14.2 96975bd638 | 447658917 | as upstream + test |
| 7 | 15a7214174 | 14.2 ec40416449 | 447693720 | SBXCHECK in `get_by_index`/`set_by_index` (sandbox hardening) |
| 8 | 00f6ecd8a7 | 14.4 be59876b1d | 508811474 | by hand; not the `runtime.h` arity change nor its racy test |
| 9 | 811ebc89d5 | 14.8 b9013994f3 | 523884658 | `CheckMaps` on the inlined-allocation path + test |
| 10 | d6a92ca485 | 14.4 0313bc720f | 536945254 | both bail-outs in `Compile` (frame over 4 KB, bytecode size) + test |
| 11 | af7d1483a5 | 15.5 6d44504cf5 | 559932543 | interpreter, without the parser hunks (`set_force_eager_compilation` does not exist; auto accessors are behind `--js-decorators`) + test; **changes bytecode: raise the cache epoch** |
| 12 | a068030f51 | 14.4 c9856baec5 | 495751197 | without the `HasEscapingUses` rename + test |
| 13 | 7d5eb1df2a | 15.0 a7c1a46375 | 547819997 | HeapNumber as a captured object with its own id + test |
| 14 | ea2f03dfff | 15.6 18321f161f | 574660146 | TurboFan/Turboshaft, not Maglev (same as `seg/v8-stackclash-133`) |
| 15 | dd7e9b5ef6 | 14.4 73ef2fa5e9 | 550360762 | **test only**: 13.4 resets the for-in check of every parent builder and never copies the inlinee's state back |
| 16 | dd4c05cef2 | 13.5 0aaafa850b | 402646504 | as is |
| 17 | b04613a419 | 13.7 fbaaf59c3e | 413419840 | **test only**: 13.4's type lattice combines by OR and has no empty type |
| 18 | f7bf2ff1fa | 13.5 beff10e3f6 | 400944465 | `maglev_allocation_folding` 1 by default: no folding across allocation sites |
| 19 | b5db50f99b | 14.6 5fc1531adc | 489633232 | SBXCHECK of Maglev's direct dispatch calls at finalization (sandbox hardening) |
| 20 | — | — | — | FlyWeb: a job whose `ExecuteJob` fails in the background is destroyed unparked, as upstream does since these bail-outs exist (13.3/13.4 dropped it parked: `UnregisterStrongRoots` DCHECK with concurrent OSR) |

Also counted: 6937df3c3d (crbug 567088929) is already in FlyWeb's `patches/v8` (FS.29) and has to be carried to 13.4
with the rest of them; 4cf9311810's Maglev half only feeds Turbolev. Left out: 809ec6cf18 (doubtful, no security crbug)
and the 4 untagged-phi fixes (e91f547823, a54bf5cd45, 473f721634, c1e78b455e).

## Tests

On a clean 13.3 `d8` with `--maglev`, the upstream regression tests of 495751197, 523884658, 536945254 and 547819997
fail (two of them crash); 413419840, 447658917, 550360762, 559932543 and 574660146 pass.

With this series, cloud `d8` (Linux x64, `dcheck_always_on`, no Intl), 28-10-2026:
- **13.4.114.21:** the 9 regression tests pass with `--maglev` (574660146 with its own `--no-maglev`); mjsunit
  **7556/7560** and, with `--extra-flags=--maglev`, `mjsunit/maglev/*` + `mjsunit/compiler/*` **1255/1258**. The failures
  are FlyWeb's own regression tests of fixes that live in `patches/v8`, not in a clean 13.4 (regress-420636529 and
  regress-430344952 are two of the 13.2-LTS-only fixes, flyweb-groupby-oom and regress-crbug-506689381): expected
  until the series sits on the level 134 `patches/v8`.
- **13.3.415.23** (same fixes on MOTOR-27's level 133 `patches/v8`): mjsunit 7464/7465 (the usual
  `maglev/extras-cped`, which asserts Maglev code) and 1213/1213 with `--maglev`.
