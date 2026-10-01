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
