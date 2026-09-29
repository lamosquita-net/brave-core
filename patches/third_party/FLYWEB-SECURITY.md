# FlyWeb: security backports

Backports onto the Chromium 116.0.5845.188 base for in-the-wild bugs fixed upstream later.
Triage: `FlyWeb/docs/cve-triage.md` in lamosquita-net/softmac. Applied by `build/commands/lib/util.js`
(`flywebRepos`) with the same GitPatcher as Brave's own patches.

| CVE | Component | Upstream fix | Backport base | Files |
|---|---|---|---|---|
| CVE-2025-6558 | ANGLE (WebGL transform feedback validation, GPU sandbox escape) | angle 2f8193ecf (M132-LTS: f0a78b0de), crbug 427162086 | ANGLE b48983ab8 (Chromium 116) | `angle/src-libANGLE-TransformFeedback.{cpp,h}.patch`, `angle/src-libANGLE-validationES2.cpp.patch` |
| CVE-2023-6345 | Skia (Ganesh mesh op integer overflow, sandbox escape) | skia 6169a1fab / M114-LTS 0ff62cff01 + f46cc1d256, crbug 1505053 | Skia bb40886d4 (Chromium 116) | `skia/src-gpu-ganesh-ops-DrawMeshOp.cpp.patch` |
| CVE-2024-4671 | viz (FrameSinkBundleImpl UAF, sandbox escape) | chromium b2cc7b7ac (M124 072039f0), crbug 339266700 | Chromium 116.0.5845.188 | `../components-viz-service-frame_sinks-frame_sink_bundle_impl.cc.patch` (chromium patch dir) |

Notes per backport:
- **CVE-2025-6558**: the M132-LTS patch applies except `validationES2.cpp`, where ANGLE 116 still uses
  `context->validationError(entryPoint, …)` instead of `ANGLE_VALIDATION_ERROR`. Logic is unchanged. The
  WebGLCompatibilityTest addition is omitted (tests are not built). Syntax-checked with clang against the 116 tree.
- **CVE-2023-6345**: M114-LTS patches (fix plus the SkToInt follow-up) apply cleanly to Skia 116. Syntax-checked with clang.
- **CVE-2024-4671**: hand-adapted. Chromium 116's `Submit()` has only `affected_groups` (the `groups`/`DidFinishFrame` path came later), so only that set is converted to weak references; `SinkGroup*` is used as map key instead of `raw_ptr<>` to avoid depending on 116's raw_ptr comparison operators. **Not compiled in the cloud** (needs the full tree): LOCAL must build it.
