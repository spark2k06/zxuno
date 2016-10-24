#include <stdio.h>
#include <stdlib.h>

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

unsigned char image[0x2401F], temp[0x15f];
unsigned short af, pos;
FILE *fi, *fo;

int main(int argc, char *argv[]) {
	if (argc == 1)
		printf("\n"
			"128sna2rom, generates ZXUno ROM from 128K SNA file. Aitor Gomez Garcia (spark2k06) 2016-10-04\n\n"
			"  128sna2rom <input_file> <output_file> <48Krom_slot>\n\n"
			"  <input_file>  SNA input file\n"
			"  <output_file> ROM output file\n"
			"  <48Krom_slot> 48K ROM slot\n\n"),

		exit(0);

	
	if (argc != 4)
		printf("\nInvalid number of parameters\n"),
		exit(-1);		

	fi = fopen("loader128k", "rb");
	if (!fi)
		printf("\nFile loader128k not found"),
		exit(-1);
	fread(temp, 1, 0x15f, fi);
	fclose(fi);
	fi = fopen(argv[1], "rb");
	if (!fi)
		printf("\nInput file not found: %s\n", argv[1]),
		exit(-1);
	fo = fopen(argv[2], "wb+");
	if (!fo)
		printf("\nCannot create output file: %s\n", argv[2]),
		exit(-1);
		
	unsigned int rom48k_offset1 = 0x00c0; // Offset de primeros 19 slots
	unsigned int rom48k_offset2 = 0x34c0; // Offset de los 45 slots restantes

	unsigned int rom48k_slot = atoi(argv[3]);
	unsigned char banks[8][16384];	
	
	unsigned int rom48k_offset = rom48k_slot > 18 ? rom48k_offset2 + 0x40 * (rom48k_slot - 19) : rom48k_offset1 + 0x40 * rom48k_slot;

	fread(image, 1, 131103, fi); // En principio leemos un snapshot con 8 bancos de 16Kb
	temp[0x68] = rom48k_offset; // Parcheamos offset de slot de 48K ROM
	memcpy(temp + 0x144, image, 0x27); // Parcheamos los 27 bytes iniciales del Snapshot
	memcpy(temp + 0xe0, image + 0xc01b, 2); // Parcheamos PC	
	unsigned char port0x7ffd = image[0xc01d];
	temp[0x80] = port0x7ffd; // Parcheamos puerto 0x7ffd	
	temp[0x64] = CHECK_BIT(port0x7ffd, 4) ? 0x09 : 0x08; // Parcheamos lectura de banco en SPI Flash en funcion de la ROM paginada
	unsigned char page = (unsigned char)(port0x7ffd << 5) >> 5;


	memcpy(image + 0x1b, temp, 0x15f); // Pegamos el loader128k ya parcheado en el banco correspondiente a la VRAM del snapshot
    
	memcpy(banks[5], image + 0x1b, 16384); // Banco 5
	memcpy(banks[2], image + 0x401b, 16384); // Banco 2

	int offsetotherbanks = 0xc01f;
	for (unsigned char i = 0; i < 8; i++)
	{
		if (offsetotherbanks == 131103) // Banco 2 o 5 fueron paginados, necesitamos leer 16Kb adicionales
		{
			fread(banks[i], 1, 16384, fi);
			break;
		}

		if (i == 2 || i == 5)
		{
			continue;
		}

		if (i == page)
		{
			memcpy(banks[i], image + 0x801b, 16384); // Banco paginado
			continue;
		}
		else
		{
			memcpy(banks[i], image + offsetotherbanks, 16384); // Resto de bancos
			offsetotherbanks += 16384;
		}

	}

	fwrite(banks[5], 1, 16384, fo);
	fwrite(banks[0], 1, 16384, fo);
	fwrite(banks[1], 1, 16384, fo);
	fwrite(banks[2], 1, 16384, fo);
	fwrite(banks[3], 1, 16384, fo);
	fwrite(banks[4], 1, 16384, fo);
	fwrite(banks[6], 1, 16384, fo);
	fwrite(banks[7], 1, 16384, fo);	
	
	fclose(fo);
}
