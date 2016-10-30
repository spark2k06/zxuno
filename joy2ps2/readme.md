# joy2ps2

Código arduino para el interface de Joystick de 6 botones basado en Kempston de conector DB9, los botones adicionales serían los siguientes:

Boton 4 -> Arriba + Abajo
Boton Select -> Izquierda + Derecha
Boton Start -> Arriba + Abajo + Izquierda + Derecha

El mapeo al teclado del ZXUno es el siguiente:

* Espacio (Boton 1)
* V (Boton 2)
* B (Boton 3)
* Cambio de mapeo del PAD -> OPQA - Cursores (Boton 4)
  * Al inicio, se mapea en Cursores por defecto

* 1 (Select + Boton 1) 
* 2 (Select + Boton 2)
* 3 (Select + Boton 3)
* 4 (Select + Boton 4)
  
* ESC (Select)
* Intro (Start)
* F2 (Start + Boton 3)
  
* NMI (Start + Boton 1) -> se cambia el mapeo del pad a Cursores
* Load 128K (Start + Boton 2)
* Load 48K (Start + Boton 3)
* Reset (Select + Arriba) -> se cambia el mapeo del pad a Cursores
* MasterReset (Select + Abajo) -> se cambia el mapeo del pad a Cursores

Idea desarrollada a partir del siguiente hilo, utilizando como base el código uart2ps2 de @Quest, y con la ayuda de otros miembros del foro:

http://www.zxuno.com/forum/viewtopic.php?f=35&p=9840

