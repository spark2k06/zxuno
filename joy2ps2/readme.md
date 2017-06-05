# joy2ps2

Código C para microcontroladores Atmega168/328 que permite mapear a scancodes de teclado PS/2 varios tipos de joysticks.

Por defecto, se encuentra configurado para poder utilizar el conector DB15, o bien el conector principal DB9. Esta configuración se puede cambiar en caliente mediante una combinación especial, de manera que se habilitaría el conector auxiliar DB9 y deshabilitaria el conector DB15. Esto es útil cuando se necesita jugar a dos jugadores con un mismo addon.

El conector DB15 admite por ahora los siguientes gamepads:

-> Neogeo Oldstyle

Los conectores db9 admiten por ahora los siguientes gamepads:

-> Atari DB9 1 o 2 botones
-> Megadrive de 3 o 6 botones, con botones Start y Select (Mode)
-> NES clonico, version de Antonio adaptado a Atari DB9

CONTROLES
---------

Desde el primer gamepad se puede controlar el ZXUno, así como todo tipo de menús. Al comienzo, éste siempre se encuentra mapeados en modo cursores, primer boton Enter y segundo Escape (modo menú tradicional).

En cualquier momento podremos pasar a un estado especial, al que llamaremos SHIFT. Para acceder a este estado, se puede hacer de cualquiera de las siguientes formas:

1. Pulsando Start + Boton 1
2. Pulsado Select + Start.
3. Pulsando y soltando la tecla Keymapper.

Una vez en el modo SHIFT, parecerá que no funciona ningún boton, pero no es asi. A partir de ahora se puede tomar la siguiente acción, que se llevará a cabo al pulsar el boton 1 del gamepad como evento final (para cancelar, volver a pulsar la combinacion de SHIFT, o bien pulsar el boton 2 del gamepad):

Nota: Las combinaciones abajo expuestas son pulsando y soltando (no mantener botones pulsados), cualquier cambio de direccion resetea el contador de las otras:

Sólo boton 1: Cambio de cursores a OPQA y viceversa en el joystick 1 (el principal)
Sólo boton 2: Cancelar modo SHIFT (también se puede cancelar volviendo a pulsar la combinación de SHIFT)

RESETS
------

* 1 vez abajo y luego boton 1: Reset (Se cambia automaticamente a cursores)
* 2 veces abajo y luego boton 1: MasterReset (Se cambia automaticamente a cursores)
* 3 veces abajo y luego boton 1: MasterReset y entrada a ROMs (Se cambia automaticamente a cursores)
* 4 veces abajo y luego boton 1: MasterReset y entrada a cores (Se cambia automaticamente a cursores)
* 5 veces abajo y luego boton 1: MasterReset y entrada a BIOS (Se cambia automaticamente a cursores)
* 6 veces y siguientes... se mantiene en la opcion de las 5 veces.

MENUS
-----

* 1 vez arriba y luego boton 1: tecla 'R'
* 2 veces arriba y luego boton 1: tecla '0'
* ... asi sucesivamente hasta tecla '9', donde se mantiene.

CARGADOR
--------

* 1 vez derecha y luego boton 1: NMI
* 2 veces derecha y luego boton 1: teclas 'LOAD ""' (modo 128k)
* 3 veces derecha y luego boton 1: teclas 'J ""' (load modo 48k)
* 4 veces y siguientes... se mantiene en la opcion de las 3 veces.

EXTRA
-----

* 1 vez izquierda y luego boton 1: Modo teclado completo (ver abajo)
* 2 veces izquierda y luego boton 1: Escape (necesario si se usa joystick de un solo boton, junto como el boton de keymapper a modo de shift)
* 3 veces izquierda y luego boton 1 de jugador 1 o 2: Cambio de mapa del jugador correspondiente. (ver abajo mapas disponibles)
* 4 veces izquierda y luego boton 1: Cambio de Video (Scroll / Lock)
* 5 veces y siguientes... se mantiene en la opcion 4.

CAMBIO DE DB15/DB9 A DB9x2
--------------------------

* Estando en modo SHIFT pulsar el boton 1, y manteniendolo pulsado, pulsar boton 2.

MODO TECLADO (KEYSTROKES)
-------------------------

* arriba: cambio de tecla hacia adelante e impresion en pantalla.
* abajo: cambio de tecla hacia atras e impresion en pantalla.
* izquierda: borrar tecla.
* derecha: aceptar tecla o espacio.
* boton 1: cambio de tecla a correspodiente con tecla derecha del shift (para imprimir mayusculas o caracteres especiales)
* boton 2: Enter

* Para salir: Entrar al modo SHIFT y elegir otra opción, o salir del modo SHIFT una vez dentro.

MAPAS DE TECLADO
----------------
