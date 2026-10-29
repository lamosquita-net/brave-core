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



