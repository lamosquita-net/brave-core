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

Notes:
- The two branch-heads/11.6 fixes landed after 11.6.189.20 and Chromium 116 never shipped them (Chrome 117 used V8
  11.7). They apply unchanged.
- CVE-2025-6554: TDZ elision is on by default in V8 11.6 (enabled by 5593d76d6), so the bug applies. The fix moves the
  `HoleCheckElisionScope` from `BuildOptionalChain()` into `OptionalChainNullLabelScope`; 116 has the same structure.
- CVE-2024-5274: the M120-LTS patch applies with an offset. It also turns a DCHECK in `Scope::MustAllocate` into a
  CHECK (crash instead of a type confusion if the invariant breaks again).
- Unit tests and golden files are left out (tests are not built). **Not compiled in the cloud**: LOCAL must build it.
- jitless does **not** mitigate these: they are in the parser, the bytecode generator and builtins.

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
