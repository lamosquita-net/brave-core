# FlyWeb: security fixes in the Chromium patches (Blink, GPU, ANGLE)

V8 has its own register in `patches/v8/FLYWEB-SECURITY.md`. Earlier Blink rounds (FS.38 and before) are only described
in their commit messages (`seg/fs38-blink137` and the like); this file starts with FS.39. Base: Chromium
128.0.6613.138 plus the patches of the level. Crash bugs are restricted, so severities are inferred from the stable
and LTS branches the fix was merged to.

## FS.39 (level 138, SEGURIDAD-PORTES-16)

02-11-2026, brave-core `seg/fs39-blink138` (Blink) and `seg/fs39-gpu` (GPU, media, ANGLE) on `nube/motor-138`
ae7e11c1. Not compiled yet.

### Blink (fixes outside the level left over from FS.38)

All of them are adapted by hand: none applied as is on 128 plus the existing patches.

- **9fb841bf28** (crbug 523292588; merged to M144, M150 and M151). `WidgetBase::UpdateCompositionInfo()` checks a
  WeakPtr after `GetCompositionRange()` and `GetCompositionCharacterBounds()`. In 128,
  `WebInputMethodControllerImpl::CompositionRange()` forces a style and layout update, as it does upstream.
  `FrameSelection::RootEditableElementOrDocumentElement()` only gets the check on entry, because in 128 it does not
  update layout itself.
- **dffb572d0b** (crbug 515155946; merged to M148 and M149). `Document::ProcessBaseElement()` copies `href` and
  `target` instead of keeping pointers into the attribute storage. 128's `UpdateBaseURL()` does not fire the
  speculation rules error event, so on 128 this is hardening.
- **155b785d0e, 6aa38f2d02 and 926d3d2597** (crbug 496280532; merged to M144-LTS, M148 and M149).
  `ReplaceNodeWithSpanCommand` no longer calls `CloneAttributesFrom()` on a span that is already connected. It now
  removes the span's attributes (`Element::RemoveAllAttributes`) and sets a copy of the replaced element's attributes
  one at a time.
  - `CloneAttributesFrom()` and `ParserSetAttributes()` now require a newly created element. The slider thumb's id
    moves from the `SliderThumbElement` constructor to `RangeInputType::CreateShadowSubtree()`.
  - `HTMLSelectElement` (`size`) skips `ResetToDefaultSelection()` when the element has no children.
  - 128 has no attribute bloom filter, so the upstream hunks for it are left out.
  - **Watch:** the new `EventDispatchForbiddenScope` in `ParserSetAttributes()` and `CloneAttributesFrom()` is a DCHECK
    only. In Release it does nothing; in a DCHECK build it can fire if some 128 `ParseAttribute()` dispatches events
    while the parser sets attributes.
  - **Watch:** the `CHECK(!hasAttributes())` and `CHECK(!GetAttrNodeList())` in `CloneAttributesFrom()` are release
    CHECKs. Reviewed against 128:
    - Callers: `CloneWithChildren` and `CloneWithoutChildren` (fresh element from
      `CloneWithoutAttributesAndChildren`), the inspector's `setNodeName` (fresh element from
      `CreateElementForBinding`) and `SVGUseElement`'s `<symbol>` → `<svg>` (fresh `SVGSVGElement`).
    - Overrides of `CloneWithoutAttributesAndChildren`: `SliderThumbElement` (fixed here), `HTMLScriptElement`,
      `SVGScriptElement` and `VTTElement`, whose constructors set no attributes.
    - Other user-agent shadow elements that set an id in their constructor are cloned by tag name, so their
      constructor does not run.
- **70c9c1f16b** (crbug 433196586 and 40753334; a crash fix, not merged to any branch). A custom property cycle reached
  through an animation resolves to `unset` in `CSSDefaultInterpolationType` and in
  `CSSInterpolationType::MaybeConvertCustomPropertyDeclaration()`. `CSSVarCycleInterpolationType` returns nullptr for
  neutral keyframes.
- **a16cd5c0ad and 34258641a0** (crbug 442619489; 34258641a0 merged to M147). `AXNodeObject::CanHaveChildren()`
  returns false on a detached object. `AXTreeSerializer::SerializeChanges()` aborts when the LCA is null. Only reached
  with accessibility on.
- **60f004b926** (M138-LTS; upstream c1433740f3, crbug 485935305; merged to M144-LTS and M146). A WebGL context
  restore bumps the `WebGLContextGroup` counter, as a context loss does, so objects created while the context was lost
  are invalid after the restore. 128 has no `WebGLContextObjectSupport`. The counter stays `uint32_t`.
