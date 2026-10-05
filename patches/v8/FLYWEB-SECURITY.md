# FlyWeb: V8 security backports

Backports onto V8 11.6.189.20 (65d8fbecd, the V8 of Chromium 116.0.5845.188). Triage:
`FlyWeb/docs/cve-triage.md` in lamosquita-net/softmac. Applied by Brave's own V8 GitPatcher
(`build/commands/lib/util.js`), next to Brave's V8 patches in this directory.

| Bug | Component | Upstream fix | Files |
|---|---|---|---|
| chromium:1479104 | Promise builtins (`Promise.any` reject closure) | v8 b0ad701a6, merged to branch-heads/11.6 #36 after 11.6.189.20 | `src-builtins-promise-any.tq.patch` |
| chromium:1477588 (v8:13723) | Ignition TDZ hole-check elision in `do`-`while` tests | v8 cf1d4d3c0, merged to branch-heads/11.6 #38 | `src-interpreter-bytecode-generator.cc.patch` |
| CVE-2025-6554 (crbug 427663123, exploited) | Ignition TDZ hole-check elision across optional chains | v8 22e9d9621 (M132-LTS f39628536, same diff) | `src-interpreter-bytecode-generator.cc.patch` |
| CVE-2024-5274 (crbug 341663589, exploited) | Parser: class static blocks parsed with the outer ExpressionScope | v8 3e037e195 (M120-LTS 8f1c780b3) | `src-parsing-parser-base.h.patch`, `src-ast-scopes.cc.patch` |
| CVE-2025-13223 (crbug 460017370, exploited) | TurboFan: property backing store extension copied slots with the wrong machine type | v8 4cf9311 (TurboFan part) + 9b5250b9 (crbug 475479135: field representation dependency, a bug introduced by 4cf9311) | `src-compiler-access-builder.cc.patch`, `src-compiler-access-builder.h.patch`, `src-compiler-js-native-context-specialization.cc.patch` |
| CVE-2024-7971 (crbug 360700873, exploited) | Liftoff (Wasm baseline): loop inputs left in registers were mixed up when merging on the back edge (f64 bits read as a reference) | v8 9797576 (functional part only) | `src-wasm-baseline-liftoff-assembler.cc.patch`, `src-wasm-baseline-liftoff-assembler.h.patch`, `src-wasm-baseline-liftoff-compiler.cc.patch` |

Notes:
- The two branch-heads/11.6 fixes landed after 11.6.189.20 and Chromium 116 never shipped them (Chrome 117 used V8
  11.7). They apply unchanged.
- CVE-2025-6554: TDZ elision is on by default in V8 11.6 (enabled by 5593d76d6), so the bug applies. The fix moves the
  `HoleCheckElisionScope` from `BuildOptionalChain()` into `OptionalChainNullLabelScope`; 116 has the same structure.
- CVE-2024-5274: the M120-LTS patch applies with an offset. It also turns a DCHECK in `Scope::MustAllocate` into a
  CHECK (crash instead of a type confusion if the invariant breaks again).
- Unit tests and golden files are left out (tests are not built). **Not compiled in the cloud**: LOCAL must build it.
- jitless does **not** mitigate these: they are in the parser, the bytecode generator and builtins.

CVE-2025-13223 (SEGURIDAD, 05-10-2026):
- `JSNativeContextSpecialization::BuildExtendPropertiesBackingStore()` is identical in 11.6 and in V8 before the fix,
  and TurboFan is the top tier in Chromium 116, so the bug applies to the sites that keep the JIT (claude.ai,
  Google). Only the TurboFan part of 4cf9311 is ported: the Maglev/Turbolev parts touch code that 11.6 lacks or that
  is off (Maglev is disabled in 116; Turbolev does not exist).
- 4cf9311 alone is unsafe: it reads the field representation without depending on it, so an in-place generalization
  (HeapObject -> Tagged) left a Smi loaded as a pointer. Upstream fixed that in 9b5250b9 (crbug 475479135); ported
  with the existing `FieldRepresentationDependencyOffTheRecord()` + `RecordDependency()` (11.6 has no
  `DependOnFieldRepresentation()`).
- Not ported: c3b80811/9b3e50ea (Smi/write-barrier tuning, partly reverted upstream for TurboFan). Final upstream
  TurboFan semantics are kept: Smi stays AnyTagged; the store keeps `kFullWriteBarrier` (safer than upstream's
  pointer barrier).
- **Compiled and tested in the cloud** with a Linux x64 d8 of 11.6.189.20 (Chromium 116's build/ and clang,
  `dcheck_always_on`): no warnings; regress-475479135-1/-2 and a FlyWeb test with Smi/Double/HeapObject/Tagged
  fields interleaved with accessors pass (the TurboFan graph shows the expected per-slot representation and the
  descriptor-walk DCHECK holds); mjsunit compiler/wasm/regress/es6-9/harmony: 4891 passed, 0 failed.

CVE-2024-7971 (SEGURIDAD, 05-10-2026):
- 11.6 has the same `PrepareLoopArgs()`. The upstream regression test (regress-360700873.js, from 4ddcbf2)
  **crashes the unpatched 11.6 d8** (`(address & kHeapObjectTagMask) == 0`: the f64 bit pattern 0x3 is used as a
  heap pointer) and passes with the patch. Wasm is off with jitless, so it only matters on the sites that keep the
  JIT.
- Ported: `PrepareLoopArgs()` -> `SpillLoopArgs()` (spill every loop input before the loop). Left out: the range-for
  cleanups of `SpillLocals()`/`SpillAllRegisters()` and the DCHECKs of 4ddcbf2 (in `parallel-move.cc`, which 11.6
  does not have). Cost: Liftoff code only (baseline tier), one spill per loop input.
Object.groupBy / Map.groupBy crash (SEGURIDAD, 05-10-2026, security review of the engine-level-117 port, FS.3):
- With FlyWeb's groupBy port (steps 46-50), any page can crash the renderer, with or without JIT:
  `Object.groupBy(iterable, x => x)` with ~17M distinct keys makes the groups OrderedHashMap exceed its maximum
  capacity, and `AddValueToKeyedGroup()` calls `Runtime::kOrderedHashMapGrow` with **no context**; throwing the
  RangeError then dereferences a null context. Reproduced on a release (no-DCHECK) d8 with all of FlyWeb's V8 patches
  and the default heap limit: `SEGV_MAPERR ffffffffffffffff`, both `Object.groupBy` and `Map.groupBy`.
- Ported v8 77df647d (crbug 438364208): pass the caller's context to the grow runtime call. Also ported the runtime part
  of v8 92aba703 (crbug 405910175): the four grow runtime functions clear the RangeError already thrown by
  `OrderedHashTable::Allocate()` before throwing their own (11.6 threw a second exception on top of a pending one;
  a DCHECK failure in debug, harmless in release for plain `Map`/`Set`). Left out: 92aba703's message changes in
  `ordered-hash-table.cc` and `keys.cc` (cosmetic) and its lower-limits test folder.
- The four `.patch` files are LOCAL's (steps 49-50) regenerated from `flyweb` 837babd7 with these changes on top.
- Code cache: no builtin, runtime function or flag is added or renamed, so caches stay compatible; still follow
  LOCAL's rule (raise `kFlyWebCacheEpoch` once per release that changes V8 patches).
