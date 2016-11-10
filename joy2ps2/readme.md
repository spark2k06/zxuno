# joy2ps2

Código arduino para el interface de Joystick de 6 botones + select + start, basado en la norma Atari Extendida de conector DB15 Neogeo.

El mapeo al teclado del ZXUno es el siguiente:

* Cambio de mapeo especial de las direcciones del PAD -> OPQA - Cursores (Boton KeyMapper + Boton 4)
  * Al inicio, se mapea en Cursores por defecto
* Espacio (Boton 1)
* V (Boton 2)
* B (Boton 3)
* N (Boton 4)
* G (Boton 5)
* H (Boton 6)

* Cambio de mapeo de todos los botones (Boton KeyMapper) -> Hasta 4 mapeos diferentes, de momento solo definido el principal para usar en el ZXUno.

* 1 (Select + Boton 1) 
* 2 (Select + Boton 2)
* 3 (Select + Boton 3)
* 4 (Select + Boton 4)
  
* ESC (Select)
* Intro (Start)
* F2 (Select + Derecha)
  
* NMI (Start + Boton 1)
* Load 128K (Start + Izquierda)
* Load 48K (Start + Derecha)
* Reset (Select + Arriba) -> se cambia el mapeo del pad a Cursores
* MasterReset (Select + Abajo) -> se cambia el mapeo del pad a Cursores

Idea desarrollada a partir del siguiente hilo, utilizando como base el código uart2ps2 de @Quest, y con la ayuda de otros miembros del foro:

http://www.zxuno.com/forum/viewtopic.php?f=35&p=9840

