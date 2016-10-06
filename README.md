# loader128k y 128sna2rom

Cargador de "ROMs" para ZXUno a partir snapshots de 128K, para utilizar desde el menu del firmware. 

Por un lado tenemos el cargador loader128k, que se autocopia en la VRAM para despues pasar el control a la misma. 

Desde la VRAM se mueven todos los bloques de 16Kb al lugar que les corresponde, se restauran los registros y por último se 

pasa el control al PC del Snapshot.

128sna2rom por su parte se utiliza para crear la "ROM" parcheando datos de loader128k.


Idea desarrollada a partir del siguiente hilo:

http://www.zxuno.com/forum/viewtopic.php?f=12&t=530
