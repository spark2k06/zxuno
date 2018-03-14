Alpha 3

-> Se incluye una versión simplificada del proyecto Joy2PS2 para el segundo puerto de Joystick de Sugarless +2, sin posibilidad de redefinición de teclas por el momento:

	KEY_Q,        // UP
	KEY_A,        // DOWN
	KEY_O,        // LEFT
	KEY_P,        // RIGHT
	KEY_5,	      // SELECT
	KEY_1,	      // START
	KEY_SPACE,    // BUTTON 1
	KEY_X,        // BUTTON 2
	KEY_M,        // BUTTON 3
	KEY_D,        // BUTTON 4
	KEY_F,        // BUTTON 5
	KEY_C         // BUTTON 6

-> Nuevos modos de teclado (.PC, .PCXT, .KBEXT)

-> Código configurable en tiempo de compilación para poder intercambiar tecla break-borrar y cursores por "." y "," (proximamente se podrá hacer mediante atajo, quedando guardada la configuración en la EEPROM de forma permanente)

-> Mejora de la gestión de combos para evitar la repetición de sus teclas asociadas a la función seleccionada.

Alpha 2

Mejora en la gestión del encendido/apagado + reset físico. Ahora para apagar se requiere que esté pulsado 3 segundos el boton de reset, de lo contrario se producirá un reset físico. 

Alpha 1

-> Se incluye el proyecto original ZXUnoPS2, adaptado a los AVR atmega128 y atmega2560.

-> Gestión de encendido/apagado del ZX-Uno conectado a Sugarless +2.

-> Gestión de bidireccionalidad del puerto PS/2 externo de Sugarless +2.
