# FlyWeb: fixes from Chromium's 116 branch that never reached a macOS stable release

Chromium 116.0.5845.188 was the last 116 stable for macOS, but branch-heads/5845 kept receiving merges up to
116.0.5845.263 (31-10-2023). Triage: `FlyWeb/docs/cve-triage.md` in lamosquita-net/softmac. The V8 roll of that branch
(11.6.189.22) is covered by `patches/v8/FLYWEB-SECURITY.md`.

| Bug | Component | Branch commit | Files |
|---|---|---|---|
| CVE-2023-5217 (crbug 1486441, exploited) | libvpx VP8 encoder: thread count change after creation → heap overflow | libvpx 278d0acd3 (cherry-pick of 3fbd1dca6), rolled by chromium dbe6d1270 | `third_party/libvpx/source/libvpx/vp8-encoder-onyx_if.c.patch` (applied by `flywebRepos` in `build/commands/lib/util.js`) |
| crbug 1475798 | Extensions: `ExtensionLocalizationURLLoader` / DataPipeProducer UAF when CSS requests are cancelled | chromium 454452c4e (cherry-pick of b6e060e17) | `extensions-renderer-extension_localization_throttle.cc.patch` |
| crbug 1478889 | Password export: `PasswordManagerPorter` called back by an open file dialog after destruction (UAF) | chromium 9998ceadc (cherry-pick of 9dea1181c) | `chrome-browser-ui-passwords-settings-password_manager_porter.cc.patch` |

The three branch fixes apply unchanged: the files are identical at 116.0.5845.188 and at the parent of each branch commit.
Unit tests are left out. Not compiled in the cloud: LOCAL must build it.
Left out on purpose: chromium 51fe1f3 (Windows/Intel video swap chain), 7d0de2b (Android WebView), infra and XTB updates.

Also here, though it is not a branch-5845 merge (fixed in Chrome 120):

| Bug | Component | Upstream fix | Files |
|---|---|---|---|
| CVE-2023-7024 (crbug 1513170, exploited) | WebRTC audio sink: invalid audio parameters accepted in `OnSetFormat` → heap overflow | chromium 340b7e300 (main #1239233): `DCHECK` → `CHECK` | `third_party-blink-renderer-platform-peerconnection-webrtc_audio_sink.cc.patch` |

## V8 (also in this branch)

| Bug | Component | Upstream fix | Files |
|---|---|---|---|
| CVE-2024-0519 (crbug 1517354, exploited) | Runtime: fast deletion of an object's last property (undoing the map transition) interacts badly with other optimizations → OOB | v8 389ea9be7 removes that path; hand-ported (116 still uses raw `JSObject` instead of `Tagged<>`, same code otherwise) | `v8/src-runtime-runtime-object.cc.patch` |

Triaged and **not ported**: CVE-2025-10585, CVE-2025-13223, CVE-2026-3910, CVE-2026-85046 (TurboFan/Maglev: jitless
mitigates them; CVE-2026-3910 is Maglev, not enabled on desktop in 116), CVE-2024-4761 and CVE-2026-87491 (Wasm,
disabled by jitless), CVE-2026-11645 (`TryFastAddDataProperty` does not exist in V8 11.6).
