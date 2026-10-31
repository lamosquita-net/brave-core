# FlyWeb: FS.38 (rule 17 of level 137), post-137 V8 fixes on 13.7.152.17

SEGURIDAD-PORTES-15, 30-10-2026 (brave-core `seg/fs38-v8` on `nube/motor-137` 78237a70). Base: 13.7.152.17 + the 240
`patches/v8` of level 137 (all apply; the .18/.19 merges 484172fda and 4278d6d37 are already in them). Branch 13.7 has
nothing else after .19; the newest branch head is still 15.8. Reviewed every merge on the branch heads 13.7 (after .17)
to 15.8 (657 commits, 341 originals), each `git apply`-checked against 13.7 + `patches/v8`: 67 already in, 36 apply,
238 conflict (most are already ported by hand in FS.29-FS.37, need code 13.7 does not have, or are Maglev). The FS.36
and FS.37 verdicts were re-read against 13.7. **New in 13.7: JSPI and Wasm deopt are on by default** (`jspi` is a
shipped Wasm feature, `wasm_deopt` defaults to true on x64; 13.5 and 13.6 had both off), and neither gin 116 nor
brave-core touches them (no `--no-experimental-wasm-jspi`/`--no-wasm-deopt`, no `SetWasmJSPIEnabledCallback`). So the
JSPI and Wasm deopt merges that FS.36/FS.37 left as "off" were redone. **Ported, 6 (cache epoch 27 -> 28; none changes
bytecode):**

- **Clean:** 9d9ccafe3b (code cache source hash checked before the background merge; was a risk since FS.36).
- **Almost clean:** c4a1766500 (565328105, Wasm deopt data checks and no cloning of blocks with `DeoptimizeIf` in
  Turboshaft branch elimination; the deoptimizer CHECKs on 13.7's code, which has no `is_simd128_reg` case).
- **By hand, JSPI (x64 only; FlyWeb ships no other architecture):** 49a1dbeb1b (561655173, `WasmResume`/`WasmReject`
  abort unless called with exactly one argument), 3137a74400 (561655173, a returning or rejecting JSPI stack jumps to
  the parent's own return address, kept in `StackSwitchFrameConstants::kParentReturnAddressOffset` = the wrapper
  buffer slot, so `WasmResume`'s two slots are popped), 54cf5fa964 (485784597, `return_switch` clears the active
  suspender's stack EPT entry before the stack goes to the pool; 13.7 has no `return_jspi_stack`), 13c7629466
  (501147587, the same when an exception, e.g. a termination, unwinds through the stack: 13.7's unwind loop does not
  track the suspender, so the chain from `kActiveSuspender` is searched for the owner). All four are sandbox-escape
  hardenings (they need an in-sandbox corruption first). The upstream tests need `--sandbox-testing` and the
  `sandbox/wasm-jspi.js` helper 13.7 lacks; our own `wasm/regress-flyweb-jspi-return.js` checks the normal
  completion and rejection paths instead.

**JSPI / Wasm deopt merges after 13.7.152.17:** ported the five above; already in 13.7 + `patches/v8`: 25e22b601c
(SBXCHECK), 07398289d9, d49d5148c9. Not ported: 7f6e1abfad and 752405a70a (WasmResume formal parameter count /
JSDispatchTable arity metadata: 13.7 has no `GetJSBuiltinState`/JS-trampoline allowlist, and changing the SFI to
`kAdapt` with a fixed-arity builtin could trip the JDT checks; 49a1dbeb1b already blocks over-application),
fd5c378f2a (Windows TEB only), 90e0d729f8/a905bbec03/51d117fbc0/03ac65ce51/23c6caade2 (other architectures),
fast-call deopt 268068728c/32a6c8d4eb/189386bca7 (exceptions thrown from fast API calls: 13.7 differs, and
Blink 116 never throws from a fast call). **None of these is exploited in the wild**, so
nothing has to be turned off from gin. The in-the-wild V8 bugs since 2025 are all covered: CVE-2025-5419 (7bc0a67ebf,
in 13.7), -6554 (22e9d9621d, .19), -10585 (ec6c184783), -13223 (4cf9311810), CVE-2026-3910 (Maglev untagged phis
off), -11645 (3c869652b0), -85046 (e0562d87ad: 13.7 does not inline `Array.prototype.sort`), -87491 (36079c3628,
already in `patches/v8`).

**Not needed on 13.7:** 8118921e3f (no `JS_DETACHED_TYPED_ARRAY_TYPE`), fba00590cb and ab2cad06c8
(`additive_safe_int_feedback` is off in 13.7), e0562d87ad (see above), f56988e020 (13.7's shuffle reducer has no
demanded-lanes halving), 59121a5f05 (no `sfis_without_scope_info_`), c5977fcc75 (13.7 already reads `next_` before
removing the node), 1a00890534 (no lightweight `Summarize`), 3ab87be544 (no embedded feedback), 1868e5fb96 and
32fd863235/bdded6d679 (fix regressions from later changes), e2ffaa3479/a638de4ac0/80c2fa2080/55496daf90 (features 13.7
does not have), 65acd1f043/562b9d1272 and the other ARM/32-bit merges, inspector-only merges (47a3dfaccd,
907d1fa76c, 3c3aab471d: DevTools), GC heuristics and build/test/infra merges. The FS.36/FS.37 "not in 13.5/13.6" list
was rechecked: still not in 13.7.

**Maglev (still off):** 74 originals not ported (the FS.37 debt list is unchanged; a54bf5cd45 is CVE-2026-3910, covered
by `maglev_untagged_phis` off).

**Risks:** b01d779c20/19d60f6362 (stale buffer when a shared Wasm memory is posted while it grows; 13.7 still uses
the old `WasmMemoryObject::New(buffer)` path, a smaller view only, not ported); the FS.37 left list (714b5478d3,
56c28da60f/327251377d, db1a465144, 28fc9a12ce, now with Wasm inlining and deopt on); FS.36 risks 17f229f3ea/
e1ac1f641a, 93fdd4a2bc, 3a6df68901 (JSPI is on for every context, so the feature key matters less); the sandbox
hardenings (cfaa590b51, 3c03b1d9e8, e9f61f1bbc, 58bdae122c, bb8724fd6c, 9c0c301876, dc31719c94, 493ea0b937,
d4374eaec8, 1a4a2b03b1, b5db50f99b), now more relevant with JSPI on.

Not compiled in the cloud: `d8` 13.7 and mjsunit by D8-LINUX (normal, `--always-osr`, `wasm/stack-switching*`,
`wasm/regress-flyweb-jspi-return.js`, `wasm/deopt/*` with and without `--no-wasm-deopt`, and
`--stress-wasm-stack-switching`).

---

# FlyWeb: own crash fix, Float16Array in Blink 128 (MOTOR-39, 30-10-2026; enters 2.5.0)

Not a V8 change, noted here because it comes from a V8 flag. Since 2.4.0, gin turns on `--js-float16array`; a
`Float16Array` passed to a Blink API (e.g. `crypto.getRandomValues(new Float16Array(4))`) reached the
`NOTREACHED_IN_MIGRATION()` in `ToDOMArrayBufferView`, which is fatal in release in 128 (`kNotReachedIsFatal`): any page
could crash its renderer (DoS only; no memory corruption found). Fix: brave-core `nube/float16-abv` f7a89181a (part of
Chromium 130: `DOMFloat16Array`, `kTypeFloat16`, `ABVTrait`, `ReadableByteStreamController`), merged into
`nube/motor-137` (d7ad12b4b). The other gin flags were checked for the same pattern (MOTOR-40, FM.52): no risk.
`--js-explicit-resource-management` adds no public V8 type (DisposableStack is internal; structured clone throws
`DataCloneError`, SuppressedError serializes as a plain `Error`; the use counters 164-166 hit the `default:` of
`use_counter_callback.cc`); `--js-regexp-escape` only adds a static function and is already shipping in 13.7.
Not yet tested in a renderer: `structuredClone(new DisposableStack())`, `postMessage(new SuppressedError(1, 2))`.

# FlyWeb: FS.37 (rule 17 of level 136), post-136 V8 fixes on 13.6.233.10

SEGURIDAD-PORTES-14, 30-10-2026 (brave-core `seg/fs37-v8` on `nube/motor-136` 1747f79f). Base: 13.6.233.10 + the 235
`patches/v8` of level 136 (all apply). Reviewed again every merge on the branch heads 13.6 (after .10) to 15.8 (672
merges, 346 originals), `git apply`-checked against 13.6 + `patches/v8`; the FS.36 verdicts "not in 13.5" and the risks
were re-read against 13.6. **Ported, 11 (cache epoch 25 -> 26):**

- **Clean:** 7e802b909a (574339635, `JSGetTemplateObject` is not `kNoWrite`), 74b6f1e095 (560121552, Promise.race
  catch prediction), 5472defe56 (511290038, `CodeEntry` refcount race), 848d7f4122 (436181695, `await using` in C-style
  for loops; **changes bytecode**), e16e6ceda4 (503869362, the 14.8 form of eb3fc08220: a shared Wasm module with other
  denormal flags is recompiled on import), 81ed440d87 (522126182, FastArray limit in `lower_limits_mode`).
- **By hand:** 7587a107c1 (577318603, normalize non-extensible root maps in MapUpdater; fuzz), d4dbd1dc45 (424905890,
  no code space compaction with stack; "not in 13.5" in FS.36), ebf970d8a3 (515431687, Sparkplug size estimate;
  13.6 has no `base::CheckedNumeric`, so 64-bit arithmetic saturated at `kMaxInt` and an OOM check in `AllocateBuffer`,
  which in 13.6 had no size check at all), 695bea76a6 (560406548, Wasm source map under
  `allocation_mutex_`), d87667efce (559893859, cyclic module SCC initialized before any module becomes `kLinked`; core
  only: `MaybeTransitionComponent`, `GetSharedFunctionInfo`, `Reset`, verifier; `CheckedCast` does not exist in 13.6, so
  a CHECK in `RunInitializationCode`). The first push (179e9dbd) did not compile on 13.6 (8118921e3f, b04613a419,
ebf970d8a3 as upstream); D8-LINUX caught it. One upstream test dropped (`fast-api-calls-nested.js`: needs `call_to_number`).

**Not needed on 13.6:** 8118921e3f (needs `JS_DETACHED_TYPED_ARRAY_TYPE` from `--track-array-buffer-views`, which
13.6 does not have: detaching does not change the map), b04613a419 (needs `IsEmptyNodeType`; still test-only, as in
FS.36), 9236a84b32 (`WellKnown_FastApi` has no `check_for_exception` in 13.6, single caller), e814d52038
(13.6 already installs the dispatch handle by offset), 4cf9311810 (already in `patches/v8`, with 9b5250b9), 7893ac23bc
(the counter overflow is 32-bit only; x64), c055ccbcde/faa4a23b32 (cppgc ordering on weak memory models; x86 is TSO),
3d3e6a10b5 and c2eab5ee18 (13.6 does not have the bug), 97e76199e8 (ICU 77 only). **Left (hardening or low value):**
714b5478d3 (CHECK in `FeedbackVector::SizeFor`, Torque-generated in 13.6), 56c28da60f/327251377d (no allocation in OOM
stack traces), db1a465144 (non-nestable Wasm compile tasks), c4a1766500 (code generator CHECKs; Wasm deopt is off),
28fc9a12ce (WKI inlining exceptions, large). **Still risks from FS.36:** 9d9ccafe3b, 17f229f3ea/e1ac1f641a, 93fdd4a2bc,
3a6df68901 and the sandbox hardenings (cfaa590b51, 3c03b1d9e8, e9f61f1bbc, 58bdae122c, bb8724fd6c, 9c0c301876,
dc31719c94, 493ea0b937, d4374eaec8). **Maglev debt (still off), for when it is turned on:** ffc18dfff9 (changes
bytecode), e321b042d4, 6c83fb0eda, 5c719bd690, 604e1bec59, d05f2fc478, bcefebe849, 4000d6244b, a2b0edcc8c; with
`maglev_untagged_phis` only: e91f547823, 473f721634, e9252d44a8.

Not compiled in the cloud (directive of 30-10): `d8` 13.6 and mjsunit by D8-LINUX (normal, `--always-osr`,
`maglev`/`compiler` with `--maglev --stress-maglev`).

---

# FlyWeb: FS.36 (rule 17 of level 135), post-135 V8 fixes on 13.5.212.10

SEGURIDAD-PORTES-13, 29-10-2026 (brave-core `seg/fs36-v8` on `nube/motor-135` ebd8c17f). Branch 13.5 has nothing after
.10 but a whitespace change (.11). Reviewed: every merge on the 13.6–15.8 branch heads (13.8 = M138-LTS, 14.4 = M144-LTS,
15.0 = M150-LTS; 690 merges, 340 originals), each `git apply`-checked and read against 13.5 + `patches/v8`. Main-only
fixes are out, except the two prerequisites below. **70 fixes ported** (one brave-core commit per area):

- **TurboFan/Turboshaft:** 8378f79d34 (413800519), 1926a0d990 (558734727), 036e5e8f69 (495273999), ec136c5038 (559780376), 057dcc408d (560536731,562243479), be842e713c (546670199), 0cc5dcf121 (522125255), 006d1038ec (430265109,419503126), a10d231ead (577327383), 07398289d9 (497404188), af9e5c36d8 (569659194), dd0f8f20b6 (560233248), a90e80ff48 (527385397).
- **Builtins, runtime, objects, API:** 50a6c0c69c (552342545), de11d56041 (532595489), 615d074ab2 (502830119), 6f81b563fa (545767601), 383120c3ac (535000102), e7f117bdb2 (473851441), 3d445698df (562038980), 5a4d7a78db (568894515), 26b9a1e79b (574351803), 9cfa2ca3fe (531319201,512450179), a9509c5e81 (487746373), 0197c35257 (436140742), 5d91cd2961 (513289241), d49d5148c9 (469878492), fbf1d9e1fd (554236352), ebf22f9a92 (429703123), e48ce362eb (428451956).
- **IC:** 62af07e961 (450328966), 7ce954c3ae (571853016), 243b68bea4 (566091009), 1ea0d1c531 (509312809).
- **Parser and interpreter:** 2a21c75603 (526380803), 58ab897331 (545412300), 9180d91eb6 (511228272), 4615af981a (563716534), 82455926cf (460166688), b371b4f8ba (449760249), 1f5fbf6824 (450618029), b80a7539b7 (536271629), d7d46a7be (421933369), 89ba284081 (506499280), b07860d015 (485267831), 119e64dc34 (490642836), c29f676270 (490642836), d643369e98 (568831100).
- **RegExp and JSON:** ddf1b5dac0 (451663011), ce4bc856e0 (486084137), 50eba5e6c2 (452296415).
- **Wasm and asm.js:** daf86cef6b (408253898), c0a41078e6 (485152421), 1eda9300e2 (433533359), c250ecfd8b (427918760), c82024befb (430960844), c3ea7757b1 (538378084), 11a31ce52c (574223268), a970eed899 (422313191), df3874776c (421403261), dec3cffe63 (487213150), 00250c2d2c (541448581,540430406), dec5f6bdb (490980546), d8801666fd (560536735).
- **Heap, cppgc, code cache:** 5a113ffdf2 (508082779), e19035b316 (575143870), 7fd662da8a (541837151), 81b2acf3bb (541837151), 7e9d59456d (425703809).
- **Maglev (still off):** dd7e9b5ef6 (550360762).

By hand (13.5 differs): af9e5c36d8 also gives the `CreateJSGeneratorObject` intrinsic a FrameState (`linkage.cc`, as
upstream since a1fa446ef); 89ba284081 needs d7d46a7be (main-only `kTooManyEvals`: without it the 20-bit eval index
wraps and the bytecode generator writes a ScopeInfo into another function's `Script::infos()` slot); d8801666fd needs
dec5f6bdb (main-only, `WasmLiftoffSetupFrame::CallingPC()`; its regression test segfaulted on 13.5); 0197c35257 only
its `CopyElementsFromTypedArray` part; 5d91cd2961 keeps the status check next to the new flag; 7e9d59456d only the
flag hash (13.5 has no `FlushDenormalsScope`); ddf1b5dac0 keeps upstream's symmetric bound; 4615af981a, c82024befb,
dec3cffe63, 7fd662da8a/81b2acf3bb, 26b9a1e79b, 7ce954c3ae, 243b68bea4, 1ea0d1c531, a9509c5e81, 50eba5e6c2, 3d445698df,
5a4d7a78db, e19035b316, 5a113ffdf2 (the old `ClearValueIfKeyIsDead` stays), d643369e98, 00250c2d2c (only the lookup) and
dd7e9b5ef6 rewritten for 13.5's code. Four upstream tests dropped (they need d8/runtime helpers 13.5 lacks).

**Maglev debt (still off):** dd7e9b5ef6 now needs its code (13.5 copies the inlinee's for-in state back; its test,
already here, failed with `--maglev`). b04613a419 still test-only (no empty node type in 13.5). The rest of the Maglev
merges fix code 13.5 does not have (KNA processor, range analysis, hole tracking, Turbolev, later reductions) or are
already in 13.5 (4940f770b4); ffc18dfff9 (unreachable catch blocks; changes bytecode) and e321b042d4 (retain constant
TypedArray buffers, defense in depth) are left for when Maglev is turned on. Untagged-phi fixes: not needed
(`maglev_untagged_phis` off).

**Not ported:** not in 13.5 (363347672a, c69bace374, fba00590cb, 56501aaabb, ddee520d8c, 8118921e3f, 7fd7eec11e,
55dcff77d1, a81b3eb9de, 95fa109f9b, 12cc665311/c30fe0dc44/f29abbb8e0, 6fa92847a9/71a8c6aed7, c2eab5ee18, 746dc18862,
fast-call deopt 268068728c/32a6c8d4eb/8a46942d6b, d4dbd1dc45, 3d3e6a10b5, c4a1766500, CVE-2026-85046's e0562d87ad);
already in 13.5 (279d280296) or in `patches/v8` (19c0bf473e, 04d57e1a86, 55cc1df038, bb38f8914d); ARM/32-bit only;
DevTools/inspector only (4ae4e70abd and the inspector merges). **Left as risks:** 9d9ccafe3b (code cache source hash
before the background merge), 17f229f3ea + e1ac1f641a (reparse of `Function()` top-level; needs `Script::CompilationKind`),
93fdd4a2bc and 3a6df68901 (Wasm async compile race, NativeModule cache key), and the sandbox/JSPI hardenings
(9c0c301876, dc31719c94, d4374eaec8, bb8724fd6c, 493ea0b937, 1a4a2b03b1, JSPI ones; JSPI is off in 13.5).

**Cache epoch 24.** Cloud `d8` 13.5 with all 239 `patches/v8` (Linux x64, `dcheck_always_on`, no Intl): mjsunit
7656/7657, with `--always-osr` 7656/7657 (both: the usual `maglev/extras-cped`), `maglev`+`stress_maglev` 15238/15238,
`maglev/*`+`compiler/*` with `--maglev --stress-maglev` (untagged phis off) 1288/1288. The upstream regression tests
of the ported fixes pass (four dropped, see above).

---



