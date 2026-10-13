# FlyWeb: V8 12.8.374.38 (Chrome 128) and its security backports

Since engine level 118 each level carries the V8 of Chrome N (FM.5). This branch uses V8 **12.8.374.38** (d213b3bb,
`FlyWeb/v8-revision`: the head of the 12.8 branch, merges up to 30-09-2024), on top of Chromium 116.0.5845.188 and
MOTOR-4's `nube/v8-12.8-fase4` 493c8998 (FM.17, `Object::Wrap`). SEGURIDAD-PORTES, 12-10-2026 (task FS.10), for
**FlyWeb 1.11.2**.

**Rule (COORDINACIÓN): 1.11.2 does not ship with fewer V8 fixes than 1.11** (`seg/v8-12.7` 94914682). Everything below
was compared on a clean 12.8.374.38 (`git apply`, `-R`, by hand).

## 1. FlyWeb 1.11's patches (`seg/v8-12.7`), carried over

- The 1.10 set (CVE-2025-6554 — MOTOR-4's clean version —, CVE-2025-13223 + 9b5250b9, CVE-2025-5419 + Turboshaft
  store-store, `IsOnlyUserOf`, CVE-2026-87491, `groupBy`, 5cf18d03, the M132-LTS ports, the options) applies to 12.8.
- Of the M126-LTS and 12.8–13.2 fixes 1.11 added: already in 12.8.374.38 (12.8 branch merges): c34adce, 4f4cd3f,
  4b30831, 96a23b2, bcb83bc (0acf7a6), e75055b, 7929e3e, f58e00f, 22e3faf, 309f157, 62a670a. Carried over: 96932a9,
  7c53644, fd73f2e, e379c53, 6e1cc25, 77a4fa1, 2b72d95, 74d8224, 5c4b4ba, d120b8c, 917b1e2, 19301b9, e1497b2, 5a7e02c,
  2e96808. Left out: 14d323f (Maglev regalloc, Maglev is off; 12.8's code differs).

## 2. New fixes for code that 12.8 has and 12.7 had not

| Fix | What |
|---|---|
| e8d92cc (crbug 355745234) | `ValueDeserializer`: check bounds and tag before searching the expected transition (12.8's new `ExpectedTransition(key_chars)` path; reachable with `postMessage`) |
| bc67262 (crbug 364422411), by hand | `Object.assign` / clone fast path across native contexts: `Map::BelongsToSameNativeContextAs` (a remote object's meta map is contextless) |
| 357d0dd4 (crbug 383647255), by hand | `Object.assign` fast path must not overwrite the target's identity hash |
| 8834c16 (crbug 392521083) | `FastCloneJSObject` skipped the write barrier only checking the young generation: now `TrySkipWriteBarrier` (marking included). Upstream's `CSA_DCHECK` on a test-only runtime function left out |
| 9b3e3bf (crbug 371237564) | Parser: no `this` var management in reparsed scopes |
| 3349cb9fd (crbug 350350588; found by MOTOR-6, 13-10) | `MicrotaskQueue`: a completed callback that removes itself while the callbacks run was not removed when the copy-on-write vector became empty (12.8's b8235a6), so Blink 116's `EventLoop` (which removes itself and releases) was called after being freed: **use-after-free reachable from any page**, and the likely cause of the YouTube hang. 12.6/12.7 do not have b8235a6 |

Checked and **not applicable** to 12.8: 91343bb (no `TrustedHeapConstant`), the big-WeakMaps revert (12.8 does not have
them), Turboshaft-Wasm fixes (`turboshaft_wasm` off), Turboshaft ISel (off), the generic wasm-to-js wrapper tier-up
(153d4e8; the wrapper is off), JSPI and central-stack switching (experimental), `call_indirect` inlining
(experimental), the 13.x type canonicalizer fixes, the global regexp result cache (absent), Maglev, ARM/LoongArch/MIPS.

## 3. Options

As in 1.7.1–1.11: `maglev`, `maglev_untagged_phis`, `turboshaft_instruction_selection` and `wasm_to_js_generic_wrapper`
off; load elimination and loop unrolling on; Wasm inlining on (COORDINACIÓN 09-10). 12.8 adds
`idle_gc_on_context_disposal` = true (GC only). `turboshaft_wasm` is still off by default.

## 4. Cache epoch

`kFlyWebCacheEpoch` **11** (10 before 3349cb9fd; 9 is `nube/v8-12.8-api`/`-fase4`, 8 reserved for `seg/v8-12.7`).

## 5. Tests (cloud `d8`, Linux x64, V8 only; `FlyWeb/scripts/v8-d8.sh`)

- Options as above by default (`--print-flag-values`).
- FlyWeb's regression tests (`FlyWeb/tools/v8-pruebas/`) and upstream ones (371237564, 377971725, 363538434, 366323452,
  364422411, 372819446): **14/14**.
- **mjsunit 7114/7122** (with Google's `kMaxArguments` test changes for f6961c40). The 8: 7
  `turboshaft/maglev-frontend/*` (experimental `--turboshaft-from-maglev`, needs Turboshaft ISel) and
  `maglev/extras-cped` (asserts Maglev code): with `--turboshaft-instruction-selection --maglev` all 171 `turboshaft/*`
  and that test pass, so they come from FlyWeb's options, not from the patches.
- Not covered by the `d8`: Intl (no ICU) and Chromium 116's libc++. **Not compiled in FlyWeb**: LOCAL builds it.
