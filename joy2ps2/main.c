/*
*    Joy2PS/2
*
*    PS/2 DATA: Pin A3
*    PS/2 CLK: Pin A2
*
*
*/


#include <stdio.h>
#include <avr/io.h>
#include <inttypes.h>
#include <util/delay.h>
#include "keymaps.h"

#define DB15_PIN01   PIND
#define DB15_PIN02   PINB
#define DB15_PORT01   DDRD
#define DB15_PORT02   DDRB
#define PS2_PORT  PORTC
#define PS2_DDR   DDRC
#define PS2_PIN   PINC

#define PS2_DAT   PC3
#define PS2_CLK   PC2

#define CPU_PRESCALE(n) (CLKPR = 0x80, CLKPR = (n))

#define HI 1
#define LO 0
#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

uint16_t DB15_PIN, DB15_PINAux;
uint16_t DB15_PINChanges, DB15PINPrev;

uint8_t keyup, keydown, keyleft, keyright;
uint8_t mapper;

void ps2Mode(uint8_t pin, uint8_t mode)
{
	if (mode) { //high
		PS2_DDR &= ~_BV(pin); //input (Truco DDR. Como input sin estado, se pone en modo Hi-Z)
	}
	else { //low
		PS2_DDR |= _BV(pin); //output (Truco DDR. Como output, se pone a 0v)
	}
}

void ps2Init()
{
	//ponemos en alto ambas señales
	PS2_PORT &= ~_BV(PS2_DAT); //A 0
	PS2_PORT &= ~_BV(PS2_CLK); //A 0
	ps2Mode(PS2_DAT, HI);
	ps2Mode(PS2_CLK, HI);
}

uint8_t ps2Stat()
{
	if (!(PS2_PIN & (1 << PS2_CLK)))
		return 1;
	if (!(PS2_PIN & (1 << PS2_DAT)))
		return 1;

	return 0;
}

//En us, reloj y semireloj, para los flancos
//zxuno v2 test15: CK1 = 240, CK2 = 480. Uso normal: CK1 = 20, CK2 = 40 microsegundos
//(revertir a normal cuando el core ps/2 del ZX-UNO se mejore)
#define CK1 240 
#define CK2 480

//envio de datos ps/2 simulando reloj con delays.
void sendPS2(unsigned char code)
{

	//Para continuar las líneas deben estar en alto
	if (ps2Stat())
		return;

	unsigned char parity = 1;
	unsigned char i = 0;

	//iniciamos transmisión
	ps2Mode(PS2_DAT, LO);
	_delay_us(CK1);

	ps2Mode(PS2_CLK, LO); //bit de comienzo
	_delay_us(CK2);
	ps2Mode(PS2_CLK, HI);
	_delay_us(CK1);
	//enviamos datos
	for (i = 0; i < 8; ++i)
	{
		if ((0b00000001 & code))
			ps2Mode(PS2_DAT, HI);
		else
			ps2Mode(PS2_DAT, LO);

		_delay_us(CK1);
		ps2Mode(PS2_CLK, LO);
		_delay_us(CK2);
		ps2Mode(PS2_CLK, HI);
		_delay_us(CK1);

		//paridad
		if ((0b00000001 & code) == 0b00000001)
		{
			if (!parity)
				parity = 1;
			else
				parity = 0;
		}
		code = code >> 1;
	}

	// Enviamos bit de paridad
	if (parity)
		ps2Mode(PS2_DAT, HI);
	else
		ps2Mode(PS2_DAT, LO);

	_delay_us(CK1);
	ps2Mode(PS2_CLK, LO);
	_delay_us(CK2);
	ps2Mode(PS2_CLK, HI);
	_delay_us(CK1);

	//Bit de parada
	ps2Mode(PS2_DAT, HI);
	_delay_us(CK1);
	ps2Mode(PS2_CLK, LO);
	_delay_us(CK2);
	ps2Mode(PS2_CLK, HI);
	_delay_us(CK1);

	_delay_us(CK2 * 3); //fin


}

//codifica envio de caracteres ps/2 
void sendCodeMR(unsigned char key, uint16_t release)
{
	uint8_t extn = 0;

	//checkeamos si es una tecla con scancode extendido (E0)
	switch (key) {
	case KEY_LEFT:
	case KEY_DOWN:
	case KEY_RIGHT:
	case KEY_UP:
	case KEY_HOME:
	case KEY_END:
	case KEY_PDN:
	case KEY_PUP:
		extn = 1;
		break;
	default:
		extn = 0;
		break;
	}
	//secuencia  

	if (extn)
		sendPS2(0xE0);

	if (key && release)
		sendPS2(0xF0);

	if (key)
		sendPS2(key);

	//fin secuencia
}

void PressKey(unsigned char key)
{
	sendCodeMR(keymapVB[key], 0); //Make
	_delay_ms(100);
	sendCodeMR(keymapVB[key], 1); //Release
}

void LOAD128() // LOAD "" en BASIC 128
{

	PressKey('L');
	PressKey('O');
	PressKey('A');
	PressKey('D');
	sendPS2(KEY_RSHIFT); // Mantenemos pulsado SHIFT
	PressKey('2');
	_delay_ms(100);
	PressKey('2');
	sendPS2(0xF0); // Liberamos SHIFT
	sendPS2(KEY_RSHIFT);
	PressKey(0x0D); // ENTER (13)

}

void LOAD48() // LOAD "" en BASIC 48
{

	PressKey('J');
	sendPS2(KEY_RSHIFT); // Mantenemos pulsado SHIFT
	PressKey('2');
	_delay_ms(100);
	PressKey('2');
	sendPS2(0xF0); // Liberamos SHIFT
	sendPS2(KEY_RSHIFT);
	PressKey(0x0D); // ENTER (13)

}

void NMI() // CTRL + ALT + F5 (NMI)
{

	sendPS2(KEY_LCTRL); // Mantenemos pulsado LCTRL
	sendPS2(KEY_LALT); // Mantenemos pulsado LALT
	PressKey(0x74); // F5 (116)

	sendPS2(0xF0); // Liberamos LALT
	sendPS2(KEY_LALT);
	sendPS2(0xF0); // Liberamos LCTRL
	sendPS2(KEY_LCTRL);
	Cursors();
	_delay_ms(200); // Retardo para evitar pulsacion INTRO por equivocacion

}

void Reset() // CTRL + ALT + Supr (Reset)
{

	sendPS2(KEY_LCTRL); // Mantenemos pulsado LCTRL
	sendPS2(KEY_LALT); // Mantenemos pulsado LALT
	PressKey(46); // Supr

	sendPS2(0xF0); // Liberamos LALT
	sendPS2(KEY_LALT);
	sendPS2(0xF0); // Liberamos LCTRL
	sendPS2(KEY_LCTRL);
	Cursors();

}

void MasterReset() // CTRL + ALT + BackSpace (MasterReset)
{

	sendPS2(KEY_LCTRL); // Mantenemos pulsado LCTRL
	sendPS2(KEY_LALT); // Mantenemos pulsado LALT
	PressKey(8); // BackSpace

	sendPS2(0xF0); // Liberamos LALT
	sendPS2(KEY_LALT);
	sendPS2(0xF0); // Liberamos LCTRL
	sendPS2(KEY_LCTRL);
	Cursors();
	_delay_ms(200); // Retardo para evitar pulsacion ESC por equivocacion

}

void ChangeKeys()
{
	keyup = keyup == KEY_UP ? KEY_Q : KEY_UP;
	keydown = keydown == KEY_DOWN ? KEY_A : KEY_DOWN;
	keyleft = keyleft == KEY_LEFT ? KEY_O : KEY_LEFT;
	keyright = keyright == KEY_RIGHT ? KEY_P : KEY_RIGHT;
}

void Cursors()
{
	keyup = KEY_UP;
	keydown = KEY_DOWN;
	keyleft = KEY_LEFT;
	keyright = KEY_RIGHT;
}

int main()
{

	// Setup		
	CPU_PRESCALE(0);
	ps2Init();
	mapper = 0; // Mapa por defecto al inicio
	Cursors(); // Direcciones del joystick como cursores por defecto al inicio

			   // Entrada desde DB15, activamos resistencias internas pullup en pines digitales 0 a 12
	DB15_PIN01 = 0xFF; // Ponemos en alto pines 0 - 7
	DB15_PIN02 = DB15_PIN02 | 0b00011111; // Ponemos en alto los pines 8 - 12, respetamos el contenido del resto ya que nos los utilzaremos
	DB15_PORT01 = 0; // Input pullup 0 - 7 
	DB15_PORT02 = DB15_PORT02 & 0b11100000; // Input pullup 8 - 12

	DB15_PINAux = 0xFFFF; // Estado inicial del joystick
	
	// Loop
	while (1) {

		DB15_PIN = (((uint16_t)DB15_PIN02 << 8) + DB15_PIN01) | 0b1110000000000000; // Organizamos los botones en 16 bits (pines digitales 0 a 12, ignorando el resto)

		if ((DB15_PINChanges = DB15_PIN ^ DB15_PINAux) != 0) // Esperamos a un cambio de estado del joystick
		{

			DB15PINPrev = DB15_PINAux; // Almacenamos estado anterior del joystick
			DB15_PINAux = DB15_PIN; // Almacenamos en auxiliar el estado actual para esperar despues un cambio del estado del joystick

			// Cambio de mapeo en bucle
			if (DB15_PIN == 0b1111111111111111 && DB15PINPrev == 0b1110111111111111) // Se ha pulsado y soltado KeyMapper
			{
				if (mapper > 2) // Disponemos de cuatro mapeos
				{
					mapper = 0;
				}
				else
				{
					mapper = mapper + 1;
				}
				return;
			}

			// Combinaciones para todos los mapeos
			if (DB15_PIN == 0b1110111111101111 && DB15PINPrev == 0b1110111111111111) { mapper = 0; _delay_ms(200); return; } // Mapper 0 (KeyMapper + Boton 1) -> Inicial
			if (DB15_PIN == 0b1110111111011111 && DB15PINPrev == 0b1110111111111111) { mapper = 1; _delay_ms(200); return; } // Mapper 1 (KeyMapper + Boton 2)
			if (DB15_PIN == 0b1110111110111111 && DB15PINPrev == 0b1110111111111111) { mapper = 2; _delay_ms(200); return; } // Mapper 2 (KeyMapper + Boton 3)
			if (DB15_PIN == 0b1110111101111111 && DB15PINPrev == 0b1110111111111111) { mapper = 3; _delay_ms(200); return; } // Mapper 3 (KeyMapper + Boton 4)

			if (DB15_PIN == 0b1111101111101111 && DB15PINPrev == 0b1111101111111111) { PressKey('1'); _delay_ms(200); return; } // 1 (Select + Boton 1)
			if (DB15_PIN == 0b1111101111011111 && DB15PINPrev == 0b1111101111111111) { PressKey('2'); _delay_ms(200); return; } // 2 (Select + Boton 2)
			if (DB15_PIN == 0b1111101110111111 && DB15PINPrev == 0b1111101111111111) { PressKey('3'); _delay_ms(200); return; } // 3 (Select + Boton 3)
			if (DB15_PIN == 0b1111101101111111 && DB15PINPrev == 0b1111101111111111) { PressKey('4'); _delay_ms(200); return; } // 4 (Select + Boton 4)
			if (DB15_PIN == 0b1111001111111111 && DB15PINPrev == 0b1111101111111111) { ChangeKeys(); _delay_ms(200); return; } // (Select + Start) Cursor <-> OQPA desde keyup, keydown, keyleft y keyright en los mapeos que lo utilicen

			if (DB15_PIN == 0b1111101111110111 && DB15PINPrev == 0b1111101111111111) { PressKey(113); _delay_ms(200); return; } // F2 (Select + Derecha)
			if (DB15_PIN == 0b1111101111111011 && DB15PINPrev == 0b1111101111111111) { PressKey(20); _delay_ms(200); return; } // BloqMayus (Select + Izquierda)
			if (DB15_PIN == 0b1111101111111110 && DB15PINPrev == 0b1111101111111111) { Reset(); _delay_ms(200); return; } // Reset (Select + Arriba)
			if (DB15_PIN == 0b1111101111111101 && DB15PINPrev == 0b1111101111111111) { MasterReset(); _delay_ms(200); return; } // MasterReset (Select + Abajo)

			if (DB15_PIN == 0b1111011111111011 && DB15PINPrev == 0b1111011111111111) { LOAD128(); _delay_ms(200); return; } // Load 128K (Start + Izquierda)
			if (DB15_PIN == 0b1111011111110111 && DB15PINPrev == 0b1111011111111111) { LOAD48(); _delay_ms(200); return; } // Load 48K (Start + Derecha)
			if (DB15_PIN == 0b1111011111101111 && DB15PINPrev == 0b1111011111111111) { NMI(); _delay_ms(200); return; } // NMI (Start + Boton 1)              

			if (mapper == 0) // Mapa 0 (inicial) -> Cursores/OPQA y botones Espacio, V, B, N, G, H. Select -> ESC, Start -> Intro
			{
				// Select y Start son especiales para una mejor integracion con las combinaciones, actuan al ser pulsados y despues soltados.
				if (DB15_PIN == 0b1111111111111111 && DB15PINPrev == 0b1111011111111111) { PressKey(13); return; } // Intro (Start)
				if (DB15_PIN == 0b1111111111111111 && DB15PINPrev == 0b1111101111111111) { PressKey(27); return; } // ESC (Select)

				if (CHECK_BIT(DB15_PIN, 10) && CHECK_BIT(DB15_PIN, 11) &&
					CHECK_BIT(DB15PINPrev, 10) && CHECK_BIT(DB15PINPrev, 11)) // Ignoramos si son pulsados o recien soltados los botones Select o Start
				{
					// Para el resto, se envia pulsacion o liberacion de tecla segun el estado de los mismos.
					if (CHECK_BIT(DB15_PINChanges, 0)) sendCodeMR(keyup, CHECK_BIT(DB15_PIN, 0));
					if (CHECK_BIT(DB15_PINChanges, 1)) sendCodeMR(keydown, CHECK_BIT(DB15_PIN, 1));
					if (CHECK_BIT(DB15_PINChanges, 2)) sendCodeMR(keyleft, CHECK_BIT(DB15_PIN, 2));
					if (CHECK_BIT(DB15_PINChanges, 3)) sendCodeMR(keyright, CHECK_BIT(DB15_PIN, 3));
					if (CHECK_BIT(DB15_PINChanges, 4)) sendCodeMR(KEY_SPACE, CHECK_BIT(DB15_PIN, 4));
					if (CHECK_BIT(DB15_PINChanges, 5)) sendCodeMR(KEY_V, CHECK_BIT(DB15_PIN, 5));
					if (CHECK_BIT(DB15_PINChanges, 6)) sendCodeMR(KEY_B, CHECK_BIT(DB15_PIN, 6));
					if (CHECK_BIT(DB15_PINChanges, 7)) sendCodeMR(KEY_N, CHECK_BIT(DB15_PIN, 7));
					if (CHECK_BIT(DB15_PINChanges, 8)) sendCodeMR(KEY_B, CHECK_BIT(DB15_PIN, 8));
					if (CHECK_BIT(DB15_PINChanges, 9)) sendCodeMR(KEY_N, CHECK_BIT(DB15_PIN, 9));
				}

			}

			/*
			if (mapper == 1) // Mapa 1 ->
			{
			}
			if (mapper == 2) // Mapa 2 ->
			{
			}
			if (mapper == 3) // Mapa 3 ->
			{
			}
			*/
		}

	}

}
