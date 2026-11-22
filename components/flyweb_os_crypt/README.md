# flyweb_os_crypt

Proveedor `fw1` de `os_crypt_async` para Mac (FS.25, FM.51): AES-256-GCM con una clave aleatoria de 256 bits en el
llavero clásico («FlyWeb Safe Storage» / «FlyWeb fw1», en Base64), sin KDF, sin T2 ni Secure Enclave. Diseño y receta
en softmac: `FlyWeb/docs/cifrado.md` y `FlyWeb/docs/fw1-receta.md`; migración y pruebas, `FlyWeb/docs/fw1-migracion.md`;
porte a Chromium 156, `FlyWeb/docs/brave-propio.md` §11.8.2 y §12.

- Chromium 156: lee el ítem con `crypto::apple::KeychainV2` (mismo ítem y formato que la 2.x, que usaba
  `crypto::AppleKeychain`). Cualquier fallo (llavero denegado o bloqueado, ítem corrupto, alta fallida, llavero de
  prueba) es `kTemporarilyUnavailable`, nunca permanente: el llamante podría borrar datos. Entonces cifra `v10`.
- Registro: `chromium_src/chrome/browser/browser_process_impl.cc` (precedencia 20, por encima del `v10` de
  `KeychainKeyProvider`, 10): con su clave, `fw1` cifra; los `v10` se leen y salen con `should_reencrypt`.
- Clave `v10`: `KeychainKeyProvider` de Chromium con el nombre «FlyWeb Safe Storage» / «FlyWeb» de
  `chromium_src/components/os_crypt/common/keychain_password_mac.mm`.
- `chromium_src/components/os_crypt/async/common/encryptor.cc` (+ parche de una línea), Mac y Linux: un dato sin
  prefijo (lo que OSCrypt de la 2.x devolvía tal cual) se devuelve en claro con `should_reencrypt`; uno con prefijo de
  proveedor (`vNN`, `fwN`) nunca sale en claro. Pruebas en `encryptor_legacy_unittest.cc`.
- Contraseñas: `patches/...login_database.cc.patch`: una fila `fw1` o `v10` que no se descifra nunca se borra
  (`IsEncryptionAvailable()` solo mira el proveedor de cifrado; el otro puede faltar).
- Cookies: `patches/net-extras-sqlite-sqlite_persistent_cookie_store.cc.patch`:
  - **Clave solo ausente en ese arranque** (llavero o Secret Service bloqueado o denegado, cualquier formato): la fila no se
    carga ni se borra, y no tira su eTLD+1. Se vuelve a leer en un arranque con clave. Si en la sesión hay una escritura
    para su `host_key`, antes se borran las filas guardadas de ese `host_key`: así no choca con el índice único y no reaparece
    un valor viejo. Sin escritura para ese `host_key`, un cierre de sesión que solo caduque esa cookie puede no verse
    hasta el siguiente arranque con clave. El delegado lo sabe por
    `CookieCryptoDelegate::DecryptStringWithTemporaryFailure` (`patches/net-extras-sqlite-cookie_crypto_delegate.h.patch`,
    `patches/services-network-network_context.cc.patch`, con `Encryptor::DecryptFlags::temporarily_unavailable`).
  - **Valor `fw1` ilegible con la clave presente** (valor corrupto, o proveedor no registrado): se pierde esa cookie sola
    (se borra su fila con el mecanismo de las no canónicas de la 156), no todo su eTLD+1.

Para cambiar de algoritmo (agilidad): proveedor nuevo con etiqueta nueva (`fw2`) y precedencia mayor; `fw1` se queda
registrado para leer lo antiguo, igual que `v10`.
