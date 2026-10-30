# flyweb_os_crypt

Proveedor `fw1` de `os_crypt_async` para Mac (FS.25, FM.51): AES-256-GCM con una clave aleatoria de 256 bits en el
llavero clásico («FlyWeb Safe Storage» / «FlyWeb fw1»), sin KDF, sin T2 ni Secure Enclave. Diseño y receta en softmac:
`FlyWeb/docs/cifrado.md` y `FlyWeb/docs/fw1-receta.md`; migración y pruebas, `FlyWeb/docs/fw1-migracion.md`.

- Registro: `chromium_src/chrome/browser/browser_process_impl.cc` (precedencia 20).
- `patches/components-os_crypt-async-common-encryptor.cc.patch`: un dato `fw1` sin su clave falla al descifrar en vez
  de pasar a OSCrypt síncrono, que lo devolvería como «texto en claro».
- `patches/components-password_manager-core-browser-features-password_features.cc.patch`: contraseñas por
  `os_crypt_async` (`kUseAsyncOsCryptInLoginDatabase`) y sin borrado de contraseñas indescifrables al sincronizar.

Para cambiar de algoritmo (agilidad): proveedor nuevo con etiqueta nueva (`fw2`) y precedencia mayor; `fw1` se queda
registrado para leer lo antiguo, igual que `v10`.
