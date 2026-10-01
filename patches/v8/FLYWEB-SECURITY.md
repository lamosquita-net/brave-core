
## Added with step 18 (branch `nube/cve-rama-5845`)

| Bug | Component | Upstream fix | Files |
|---|---|---|---|
| CVE-2024-0519 (crbug 1517354, exploited) | Runtime: fast deletion of an object's last property (undoing the map transition) interacts badly with other optimizations → OOB | v8 389ea9be7 removes that path; hand-ported (116 still uses raw `JSObject` instead of `Tagged<>`, same code otherwise) | `src-runtime-runtime-object.cc.patch` |

Triaged and **not ported**: CVE-2025-10585, CVE-2025-13223, CVE-2026-3910, CVE-2026-85046 (TurboFan/Maglev: jitless
mitigates them; CVE-2026-3910 is Maglev, not enabled on desktop in 116), CVE-2024-4761 and CVE-2026-87491 (Wasm,
disabled by jitless), CVE-2026-11645 (`TryFastAddDataProperty` does not exist in V8 11.6).
