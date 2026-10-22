# Parches de Mojave de la base 128 (LOCAL-6, LOCAL-7, LOCAL-8)

Diffs que LOCAL aplica sobre el árbol de Chromium 128, después de `npm run apply_patches`, para que la 2.0 funcione en macOS 10.14.
Van a convertirse en `patches/*.patch` de brave-core (MOTOR-16; Dawn y ANGLE por util.js). Base: `nube/base-128-motor-p6` 02c9d0ca4b.

- `src.diff` (`src/`, 20 ficheros): GPU de la Fase 0/1 (`color_space_util.mm`, `hdr_copier_layer.mm`, `screen_mac.mm`, `font_matcher_mac.mm`,
  `system_services.cc`, `launch_application.mm` + `launch_services_spi.h`) y el repaso de los avisos `-Wunguarded-availability` de 10.15
  (LOCAL-8, 22-10): Bluetooth de WebAuthn (`CBCentralManager.authorization`, caída al abrir accounts.google.com), `LAContext` de Ajustes,
  `NSWorkspaceOpenConfiguration` (abrir descargas y lanzar apps), `posix_spawn_file_actions_addchdir_np`, `productCategory` (mandos),
  `requestWhenInUseAuthorization` (geolocalización), `fontWithSize:` (consulta de diccionario), `kSecUseDataProtectionKeychain`, lista de
  cámaras (`AVCaptureDeviceDiscoverySession`), `takePhoto`, cuentagotas (`NSColorSampler`) y `SFUniversalLink`. Patrón: el `@available` de la 116.
- `angle.diff` (`src/third_party/angle`) y `dawn.diff` (`src/third_party/dawn`): GPU de la Fase 1 (LOCAL-7).

Sin tocar a propósito (no se alcanzan en 10.14): `chrome/browser/mac/auth_session_request.*` y el registro de
`ASWebAuthenticationSessionWebBrowserSessionManager` en `app_controller_mac.mm` (clase nil en 10.14; el navegador arranca bien).
