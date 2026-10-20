# FlyWeb: security backports

Backports onto the Chromium 116.0.5845.188 base for in-the-wild bugs fixed upstream later.
Triage: `FlyWeb/docs/cve-triage.md` in lamosquita-net/softmac. Applied by `build/commands/lib/util.js`
(`flywebRepos`) with the same GitPatcher as Brave's own patches.

| CVE | Component | Upstream fix | Backport base | Files |
|---|---|---|---|---|
| CVE-2025-6558 | ANGLE (WebGL transform feedback validation, GPU sandbox escape) | angle 2f8193ecf (M132-LTS: f0a78b0de), crbug 427162086 | ANGLE b48983ab8 (Chromium 116) | `angle/src-libANGLE-TransformFeedback.{cpp,h}.patch`, `angle/src-libANGLE-validationES2.cpp.patch` |
| CVE-2023-6345 | Skia (Ganesh mesh op integer overflow, sandbox escape) | skia 6169a1fab / M114-LTS 0ff62cff01 + f46cc1d256, crbug 1505053 | Skia bb40886d4 (Chromium 116) | `skia/src-gpu-ganesh-ops-DrawMeshOp.cpp.patch` |
| CVE-2024-4671 | viz (FrameSinkBundleImpl UAF, sandbox escape) | chromium b2cc7b7ac (M124 072039f0), crbug 339266700 | Chromium 116.0.5845.188 | `../components-viz-service-frame_sinks-frame_sink_bundle_impl.cc.patch` (chromium patch dir) |
| CVE-2026-3909 | Skia (Ganesh/Graphite text atlas: glyph entry reused across mask formats, OOB write, in the wild) | skia 0cab3e4ee, crbug/b 491421267 (Chrome 146.0.7680.80) | Skia bb40886d4 (Chromium 116) | `skia/src-text-gpu-{Glyph.h,StrikeCache.h,StrikeCache.cpp,GlyphVector.h,GlyphVector.cpp,SubRunContainer.cpp}.patch`, `skia/src-gpu-ganesh-text-GrAtlasManager.cpp.patch`, `skia/src-gpu-graphite-text-AtlasManager.cpp.patch` |
| CVE-2023-5218 | Site Isolation (UAF of `RenderFrameHostImpl` resuming a `pending_navigate_` after an early RFH swap; critical) | chromium 093daae65d5 (+ 0763aad8eee, guest views), crbug 1487110 (Chrome 118.0.5993.70) | Chromium 116.0.5845.188 | `../content-browser-renderer_host-render_frame_host_impl.cc.patch` (with the existing FlyWeb hunk), `../content-browser-renderer_host-render_widget_host_impl.cc.patch`, `../components-guest_view-browser-guest_view_base.{cc,h}.patch`, `../extensions-browser-guest_view-*.patch` |
| CVE-2024-2883 | ANGLE, Vulkan backend (inactive vertex attributes not marked as accessed: UAF; critical) | angle e5cb7f1f5ca, crbug 327807820 (Chrome 123.0.6312.86) | ANGLE b48983ab8 (Chromium 116) | `angle/src-libANGLE-renderer-vulkan-ContextVk.cpp.patch` |
| CVE-2024-4058 | ANGLE translator (constant folding after `RemoveArrayLengthMethod`; type confusion in SPIR-V output; critical) | angle 0a67bbaf83f, crbug 332546345 (Chrome 124.0.6367.78) | ANGLE b48983ab8 (Chromium 116) | `angle/src-compiler-translator-{Compiler.cpp,OutputSPIRV.cpp}.patch` |
| CVE-2024-7532 | ANGLE translator (WebGL shaders with infinite loops, constant `switch` with no matching case; OOB; critical) | angle 568caaa0482, 451d78d9963, 67d8d8c88ad, crbug 350528343 (Chrome 127.0.6533.99) | ANGLE b48983ab8 (Chromium 116) | `angle/src-compiler-translator-tree_ops-{PruneNoOps,PruneInfiniteLoops}.{cpp,h}.patch`, `angle/src-compiler-translator-{Common.h,Compiler.cpp}.patch`, `angle/src-compiler.gni.patch`, `angle/include-GLSLANG-ShaderLang.h.patch`, `angle/src-libANGLE-Shader.cpp.patch` |

Notes per backport:
- **CVE-2025-6558**: the M132-LTS patch applies except `validationES2.cpp`, where ANGLE 116 still uses
  `context->validationError(entryPoint, …)` instead of `ANGLE_VALIDATION_ERROR`. Logic is unchanged. The
  WebGLCompatibilityTest addition is omitted (tests are not built). Syntax-checked with clang against the 116 tree.
- **CVE-2023-6345**: M114-LTS patches (fix plus the SkToInt follow-up) apply cleanly to Skia 116. Syntax-checked with clang.
- **CVE-2024-4671**: hand-adapted. Chromium 116's `Submit()` has only `affected_groups` (the `groups`/`DidFinishFrame` path came later), so only that set is converted to weak references; `SinkGroup*` is used as map key instead of `raw_ptr<>` to avoid depending on 116's raw_ptr comparison operators. **Not compiled in the cloud** (needs the full tree): LOCAL must build it.
- **CVE-2026-3909**: hand-adapted. Upstream (2026) keys the backend-specific `GlyphEntry` by
  `(SkPackedGlyphID, MaskFormat)` and passes the sub run's mask format through `initBackendData`. Skia 116 has
  the same flaw in the shared `sktext::gpu::TextStrike` (glyphs cached by packed ID only, and
  `addGlyphToAtlas` choosing the atlas from the rasterized glyph's format), so the port keys `Glyph` by
  `Glyph::Key{packedID, maskFormat}`, passes the sub run's format to `GlyphVector::packedGlyphIDToGlyph`, and
  makes both atlas managers use `glyph->fMaskFormat`. The new AtlasOobTest is omitted (tests are not built).
  Syntax-checked with clang against the 116 tree; patches apply cleanly to bb40886d4. **Not compiled in the
  cloud**: LOCAL must build it and check text rendering (plain, LCD/subpixel, colour emoji).
- **CVE-2023-5218** (FS.22): 093daae65d5 applies to 116 with offsets, on top of FlyWeb's existing hunk in
  `render_frame_host_impl.cc` (116 has `pending_navigate_` with the same resume path in `Init()`). 0763aad8eee (same
  bug: only `<webview>` delays resuming a created window) needed two context adaptations (the 116 comment in
  `guest_view_base.cc` says "WillAttach"; `WebViewGuest::ShouldResumeRequestsForCreatedWindow` goes before
  `AddNewContents`). The browser test is omitted. **Not compiled in the cloud**: LOCAL must build it.
- **CVE-2024-2883 / CVE-2024-4058** (FS.22): ANGLE 116 builds the Vulkan backend and SwiftShader on Mac
  (`gni/angle.gni`); they are used when WebGL falls back to software. e5cb7f1f5ca applies cleanly; 0a67bbaf83f only
  changes path (`OutputSPIRV.cpp` was not yet under `spirv/`). Its `FoldExpressions` part runs for every backend.
- **CVE-2024-7532** (FS.22): 568caaa0482 applies cleanly; 451d78d9963 by hand (the new pass goes right after
  `SeparateDeclarations`, as upstream; 116 has no `rescopeGlobalVariables`); 67d8d8c88ad without the
  `rejectWebglShadersWithUndefinedBehavior` frontend feature: the compile option is set for every WebGL shader in
  `Shader::compile`, which is upstream's default. Not ported: 65f08d08515 (Vulkan workaround, Linux/Intel only),
  8469b249b (2025 refactor into the parser) and 386bb0e11 (MSL `EnsureLoopForwardProgress`, not in 116).
  The ANGLE patches apply cleanly to b48983ab8; syntax-checked with clang against the 116 tree (`Shader.cpp` with a
  stub `xxhash.h`). Tests are not built. **Not compiled in the cloud**: LOCAL must build it and load WebGL pages
  (shaders with loops still compile; a shader with `for(;;){}` and no break is now rejected, as in Chrome 127+).
