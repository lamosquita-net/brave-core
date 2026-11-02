# FS.39 propio — revisión de seguridad del nivel 138 (regla 17)

Rama `seg/fs39-propio` sobre `nube/motor-138` 06d351d39b (MOTOR-42/43/44). Alcance: lo portado o tocado en
480f73623798..06d351d39b (147 ficheros fuera de V8; la V8 13.8 va aparte). Método: los 8030 cherry-picks de las
ramas 138 (LTS, hasta 138.0.7204.310) y 139–149 (hasta sus últimas etiquetas) cruzados por fichero con los portados,
más los commits de main desde el 20-05-2025 hasta 8577dee868 (02-11-2026) cuyo mensaje nombra el área y habla de
caída, CHECK, nulo, desbordamiento o seguridad; además, diff 138.0.7204.185 → main de cada fichero nuevo del porte.
Los parches se regeneran completos contra 128.0.6613.138 y se comprueban con `git apply` sobre el original.

## Arreglos aplicados

| Commit | Fichero(s) | Upstream | Gravedad (deducida) |
|---|---|---|---|
| FS.39: Integrity-Policy parser does not crash on non-token items | `services/network/public/cpp/integrity_policy_parser.cc` | d71f062d656 (crbug 542994311, solo main) | **Media**: una cabecera `Integrity-Policy: blocked-destinations=(1)` llamaba a `Item::GetString()` sobre un entero (QUICHE_CHECK fatal) en el proceso que analiza las cabeceras (servicio de red): cualquier web lo tumbaba. Adaptado: la 128 no tiene `GetIfToken()`; el mensaje sale de `SerializeItem()` |
| FS.39: progress() with NaN skips std::clamp, checked calc multiply | `css_math_expression_node.cc`, `calculation_expression_node.cc` | f27c40425db + 2c28ee75bf8 (crbug 516759906); 61d51b2d398 (crbug 422982562) | Baja (endurecimiento): `std::clamp(NaN)` del recorte de `progress()` portado; la multiplicación ya no hace un `To<>` sin comprobar |
| FS.39: WebCodecs readback callbacks hold cross-thread handles | `video_frame.cc`, `video_encoder.cc`, `background_readback.cc` | 9cad4244e46, en M144-LTS como d6e8a8cb223 (crbug 517129549) | **Media-alta** (fusionado a LTS): callbacks con `WrapPersistent` que se pueden destruir fuera del hilo principal. El código de la 128 es el mismo |

## Revisado sin cambios

| Fichero | Arreglos posteriores | Veredicto |
|---|---|---|
| `reporting_context.cc` | 101ab4886e6 / LTS 53e1827796 (no hacer downcast de informes Hash/Integrity) | Ya dentro (se trata antes del `LocationReportBody`); la 128 no tiene `kCSPHash` |
| blink `integrity_policy.cc` | ae476b94e09 (exención de `about:`, alineación con la especificación) | Ya trae el retorno sin policy container de 138.0.7204.185; lo de `about:` no es de seguridad (139) |
| `service_worker_version.cc` (`SetPolicyContainerHost`) | — | Igual que 138.0.7204.185 y main (comprueba el host nulo) |
| `navigation_policy_container_builder.*`, `policy_container_host.*` | 68478a66451 (herencia de políticas del iniciador si cambia la StoragePartition) | Reestructura de main ligada a `NavigationStateKeepAlive` y LNA; no aplica a la 128 |
| `styled_markup_serializer.cc`, `dom_selection.cc`, `system_clipboard.cc` | solo se quitan banderas y MathML | Sin arreglos |
| `chunk_to_layer_mapper.cc`, `raster_invalidator.cc` | 9581ab6ed67 (transformaciones singulares) | Corrección visual, no de seguridad |
| `shape_result_spacing.cc`, `simple_font_data.cc` | refactorizaciones, caché de monoespaciado | Sin arreglos |
| `video_decoder.cc`, `decoder_template.*`, `media/base/video_transformation.*` | fdfbc2680c3 (revierte un cambio que FlyWeb no tiene), 11685a78351 (ayudante) | Sin arreglos; `rotation` es `double` en el IDL (NaN/Inf dan TypeError) y se acota con `fmod` |
| `css_math_expression_node.cc` | 8c1ab242bf2 (`abs()`, M145), d668e9a0b09, c7dfb0ad8a3 (aritmética tipada), 93b4adaa137 (`clamp(none)`), 2c52271bc93 (ángulos sin resolver con `sibling-index()`) | No aplican: la 128 no tiene aritmética tipada, ni `clamp()` con `none`, ni `sibling-index()` (aplazado). Revisar al portarlos |
| `mixed_content_checker.cc` | 03b9f5da120, 9a0010ab658 (LNA) | LNA no está en la 128 |
| `quota_exceeded_error.*`, `exception_state.*`, `dom_exception.cc` | 400a46080e1 (ya portado) | Tabla de nombres por búsqueda lineal: el corrimiento del enum no indexa nada |
| `dom_viewport.cc`, `svg_graphics_element.cc` | — / contador de uso | Sin arreglos |
| `svg_a_element.cc` | 17a3040239e (noopener de Blob URL en `<a>` SVG), 72a3631366f | El primero depende de `EnforceNoopenerOnBlobURLNavigation` (no está en la 128); FlyWeb ya no resuelve Blob URL de otra partición (anulación de Brave). El segundo arregla una regresión posterior |
| `third_party/simdutf` 7.0.0, `dragonbox` | subidas a 7.3/7.7 y rollos sin marca de seguridad | Sin arreglos marcados |

## Fuera del alcance de lo portado (código previo de la 128 en ficheros que tocó el porte)

Arreglos de seguridad fusionados a ramas estables/LTS sobre código que ya estaba en la 128. No se aplicaron en
`seg/fs39-propio`; COORDINACIÓN los metió en la 2.7.0 como rama `seg/fs39-lts` (sección siguiente):

- `render_frame_host_impl.cc`: cf3b685c0d5 [M138-LTS] (ficheros de PageState no validados, crbug 487383169),
  eb3896190a2 [M144-LTS] (ResourceRequestBody en CreateNewWindowParams; los dos primeros ya están en 138.0.7204.310), 9971d3de459 [M144-LTS] + 0dcb0580163
  (DidCommitNavigation desde un RFH en bfcache), 176da1f0c7b [M149] (salto de Site Isolation con subtramas MHTML),
  b3727a40d28 [M149] (propiedad de widgets emergentes), 8ed77b6a0d7 [M148] (RFH borrado en un bucle anidado).
- `navigation_request.cc`: 1e6b1e116cc / e40949404db (MiraclePtr de `frame_tree_node_`), b2b579bb87c [M149] (CHECK de
  `CanAccessOrigin()`).
- `service_worker_version.cc`: ee4d43b9004 [M140] (carrera en `OnTimeoutTimer`, también en la LTS 138.0.7204.310).
- `element.cc`: b619f72c646 [M148] (EventDispatchForbiddenScope en la notificación por lotes de atributos).
- `frame_selection.cc`: bae0c060694 [M144] (comprobaciones tras `CompositionRange()`).
- `anchor_position_scroll_data.cc`: 95c7adc412f [M145] (doble desplazamiento en hit testing de anclas).

## seg/fs39-lts

Rama sobre `seg/fs39-propio` 17f009dfda (cadena blink138 → v8 → gpu → brotli → propio → lts, lineal). Un commit por
arreglo; cada `.patch` se regenera completo contra 128.0.6613.138 desde el de la cadena (conserva lo de FS.39 Blink y
del porte). Comprobado: los 11 `.patch` tocados aplican sobre el original de la 128; `fuera.py --ref` main: 0 avisos.
Sin compilar ni pruebas (las de upstream no se portan).

| Commit | Upstream | Qué arregla (128) |
|---|---|---|
| FS.39 LTS: every file in a PageState must be in its referenced files | cf3b685c0d5 [M138-LTS], crbug 487383169 | El navegador solo validaba los ficheros que lista `GetReferencedFiles()`; el renderer podía esconder más en el árbol de marcos. `GetAllFilesInPageState()` nuevo en `page_state_serialization.{h,cc}` |
| FS.39 LTS: validate the form body of CreateNewWindow | eb3896190a2 [M144-LTS] (también en la .310), crbug 487768779 | `form_submission_post_data` de `window.open()` se cargaba sin `CanReadRequestBody()` (`web_contents_impl.cc` la usa en la 128). `VerifyCreateNewWindowParams()` nuevo en `ipc_utils` |
| FS.39 LTS: no DidCommitNavigation from a frame in the back-forward cache | 9971d3de459 [M144-LTS] + 0dcb0580163 [M149], crbug 517148260 | En la 128 era un `DCHECK`. Mata al renderer si hubo ACK de bfcache; si no, expulsa la entrada. Valores de enum como upstream (`RFH_DID_COMMIT_NAVIGATION_WHILE_BFCACHED = 354`, `kDidCommitNavigation = 44`); sin `enums.xml` |
| FS.39 LTS: MHTML subframes may only commit opaque origins | 176da1f0c7b [M149], crbug 498783665 | `CanCommitOriginAndUrl()` salía antes sin mirar el origen en subtramas MHTML: salto de Site Isolation |
| FS.39 LTS: stop after DidNavigate if a nested loop deleted the frame | 8ed77b6a0d7 [M148], crbug 516608438 | `WeakPtr` tras `DidNavigate()` y tras `DidCommitNavigationInternal()` en la activación |
| FS.39 LTS: NavigationRequest::frame_tree_node_ is a raw_ptr | d5c6640b6e7 (1e6b1e116cc [M148] / e40949404db [M149]), crbug 516608438 | `RAW_PTR_EXCLUSION` → `raw_ptr<FrameTreeNode> const`; la 128 ya tiene `raw_ptr.h` y el resto de usos convierten solos. Solo el de tracing pasa por `frame_tree_node()` |
| FS.39 LTS: service worker request timeouts without dangling iterators | ee4d43b9004 [M140] (también en la .310), crbug 440454442; **más 405dcf07fd1**, crbug 499449324 [M144-LTS] | Carrera en `OnTimeoutTimer()`. ee4d43b9004 solo deja `InflightRequest::timeout_iter` apuntando a una entrada borrada (UAF en `FinishRequestWithFetchCount`), así que va con su arreglo posterior (iterador opcional; upstream con interruptor, aquí sin él) |

Fuera, con motivo:

- **b3727a40d28** (propiedad de widgets emergentes): el único consumidor es la extracción de contenido de página de
  `optimization_guide` (APC, IA de Chrome), que no está en la 128; el resto es fontanería de 15 ficheros.
- **b2b579bb87c** (restaurar el `CHECK` de `CanAccessOrigin()`): en la 128 ya es `CHECK`; lo rebajó un cambio de 2026.
- **b619f72c646** (`element.cc` + `html_select_element.cc`), **bae0c060694** (`frame_selection.cc` + `widget_base.cc`):
  ya en la cadena por FS.39 Blink (6aa38f2d02 y 9fb841bf28). En la 128, `RootEditableElementOrDocumentElement()` no
  fuerza layout, así que basta la comprobación inicial.
- **95c7adc412f** (anclas): solo corrige la rama de `CSSAnchorUpdate`, que no existe en la 128 (allí no hay doble suma).

**KEV (CISA, catálogo 2026.10.30, espejo cisagov/kev-data): ninguno.** Las entradas de Chrome desde junio de 2025 son de
V8, Dawn, Skia, ANGLE, CSS (CVE-2026-2441) y Brotli (CVE-2026-111497, ya en `seg/fs39-gpu`); ninguna es de navegación,
PageState, bfcache, MHTML ni service workers. No he podido cruzar crbug → CVE en Chrome Releases (crbug cerrado); la
deducción es por componente.

## Privacidad (regla 20)

- **Integrity-Policy, informes:** van solo a los endpoints que la propia web declara en `Reporting-Endpoints` (pueden
  ser de terceros, como en CSP), con `documentURL` y `blockedURL` recortados por `StripURLForUseInReport`, el destino
  y `reportOnly`. No añaden datos que la web no tenga ya. `ReportingObserver` sigue sin entregar nada (anulación de
  Brave) y el Reporting API de red sigue como en Chrome y Brave 1.80. Sin cambios.
- **VideoFrame rotation/flip:** metadatos del propio fotograma o de la configuración del decodificador; en Mac las
  cámaras no rotan, no revela orientación del equipo. Sin cambios.

## Anotado

- **`scheduler.yield` hereda señales entre orígenes** (falta 1988acd89, Chrome 136): aplazado al 139, seguridad baja.
  FlyWeb no tiene `SchedulerTaskContext` (1f5acb16b48e tampoco se portó).

## Límites

- Sin compilar. Los commits de main sin palabras clave reconocibles en el mensaje no se han mirado; los de las ramas
  estables y LTS sí, todos.
- crbug está cerrado: la gravedad se deduce de las fusiones a estable/LTS.
