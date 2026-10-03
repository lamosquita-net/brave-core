# FlyWeb: recursos de la pestaña nueva y del primer inicio

Entregados por el HUMANO (03/10/2026); preparados por LOCAL. Los monta NUBE en `nube/ntp`.
Referencia de comportamiento: `FlyWeb/docs/prototipo-ntp/` del repo softmac.

- `fondos/fondo-1.jpg` … `fondo-7.jpg`: fondos de la pestaña nueva (uno por modelo de maqueta), reducidos a 2560 px
  de ancho, JPEG calidad 80. `fondo-6.jpg` es de 1551 px (el original no es mayor). `fondo-boot.jpg`: primer inicio.
  Fotografías de @lamosquita (crédito en pantalla: "© fotografía @lamosquita").
- `fuentes/D-DIN.woff2`, `D-DIN-Bold.woff2`: D-DIN de Datto (no "D-DIN PRO"), convertida a WOFF2 sin otros cambios
  desde los OTF originales. Licencia SIL OFL 1.1 en `OFL.txt` (nombre reservado "D-DIN").
- `moscas/mosca-newtab.svg`: la que vuela en la pestaña nueva. Alas = `.cls-2`; el contorno negro de alas y cuerpo
  es un único trazado (las alas no se pueden mover por separado).
- `moscas/mosca-primer-inicio.svg`: la del primer inicio. Alas = `.cls-1`; los 7 trazados negros después del
  principal son los arcos del zumbido; el `<rect class="cls-3">` sobra.
