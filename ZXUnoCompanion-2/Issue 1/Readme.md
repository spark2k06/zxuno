Propuestas para una hipotética Issue 2, para quién se atreva con ella :-):

- LLevar todos los pines disponibles de Atmega128 o Arduino Mega 2560 a un único conector, a modo de expansión.

- Corregir pinout ISP del atmega128 para evitar tener que hacer fix si se quiere usar éste. Se trata de los pines MOSI y MISO, que actualmente no están donde corresponden.

- Añadir chip CH340 para poder programar el atmega128 por serie como la versión del arduino mega, en lugar de sólo por ISP, y llevar pos pistas las señales del CH340 a los pines D+ y D- del conector de arduino, para que también pueda ser programado desde el conector microUSB sin necesidad de abrir la carcasa.

- ¿Otras?
