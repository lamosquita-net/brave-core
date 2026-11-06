# patches/ after the Flynk cut (FM.54 F1)

From FlyWeb 2.7.0 on, the Chromium and V8 changes no longer live here as `.patch` files. They are commits of
our own trees (softmac `FlyWeb/docs/fork-propio.md` §5–6):

- Chromium: `lamosquita-net` Chromium fork, branch `flynk` (2.7.0 = the conversion of this directory's
  history by `FlyWeb/tools/convertir.sh`).
- V8: `lamosquita-net/v8`, branch `flynk-v8` (replaces `FlyWeb/v8-revision`).

What is still here and applied by `npm run apply_patches`: the patches of the DEPS subrepositories
(`third_party/angle`, `skia`, `boringssl/src`, `catapult`, `dawn`, `devtools-frontend/src`, `emoji-segmenter/src`,
`net/third_party/quiche/src`). They move to forks or `DEPS` in F4.

The `FLYWEB-SECURITY*.md` files are kept as the record of the security ports made as patches; new ports are
commits with `Flynk-Class: security`.
