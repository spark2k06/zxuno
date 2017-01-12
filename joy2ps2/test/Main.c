/*
*    Joy2PS/2
*
*    PS/2 DATA: Pin A3
*    PS/2 CLK: Pin A2
*
*
*/


#include "keymaps.h"
#include <stdio.h>
#include <avr/io.h>
#include <inttypes.h>
#include <util/delay.h>

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

#define QUEUE_ELEMENTS 64
#define QUEUE_SIZE (QUEUE_ELEMENTS + 2)
unsigned char QueuePS2Command[QUEUE_SIZE];
double QueuePS2WaitMS[QUEUE_SIZE];

uint8_t QueueIn, QueueOut;

unsigned char sendcode;
double wait_ms;

unsigned char KeyMap[12];
uint8_t imap;

void my_delay_ms_10ms_steps(int ms)
{
	while (0 < ms)
	{
		_delay_ms(10);
		ms -= 10;
	}
}

void QueuePS2Init(void)
{
	QueueIn = QueueOut = 0;	
}

void QueuePS2Put(unsigned char sc, double ms)
{

	QueuePS2Command[QueueIn] = sc;
	QueuePS2WaitMS[QueueIn] = ms;

	QueueIn = (QueueIn + 2) % QUEUE_SIZE;

}

void QueuePS2Get(unsigned char *old_sc, double *old_ms)
{
	
	*old_sc = QueuePS2Command[QueueOut];
	*old_ms = QueuePS2WaitMS[QueueOut];

	QueueOut = (QueueOut + 2) % QUEUE_SIZE;		

}

void ps2Mode(uint8_t pin, uint8_t mode)
{
	if (mode) { //high
		PS2_DDR &= ~_BV(pin); //input (Truco DDR. Como input sin estado, se pone en modo Hi-Z)
	}
	else { //low
		PS2_DDR |= _BV(pin); //output (Truco DDR. Como output, se pone a 0v)
	}
}

//En us, reloj y semireloj, para los flancos
//zxuno v2 test15: CK1 = 240, CK2 = 480. Uso normal: CK1 = 20, CK2 = 40 microsegundos
//(revertir a normal cuando el core ps/2 del ZX-UNO se mejore)
#define CK1 4
#define CK2 8

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

// guardamos en un buffer las peticiones de envio de scancodes al puerto PS/2
void sendPS2(unsigned char code, double ms)
{
	QueuePS2Put(code, ms);
}


//envio de datos ps/2 simulando reloj con delays.
void sendPS2fromqueue(unsigned char code)
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

	

}

//codifica envio de caracteres ps/2 
void sendCodeMR(unsigned char key, uint16_t release, double ms)
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
		sendPS2(0xE0, 0);

	if (key && release)
		sendPS2(0xF0, 0);

	if (key)
		sendPS2(key, ms);


}

void PressKey(unsigned char key, double ms)
{
	sendCodeMR(key, 0, 100); //Make	
	sendCodeMR(key, 1, ms); //Release
}

void Cursors()
{
	keyup = KEY_UP;
	keydown = KEY_DOWN;
	keyleft = KEY_LEFT;
	keyright = KEY_RIGHT;
}

void LOAD128() // LOAD "" en BASIC 128
{

	PressKey(KEY_L, 0);
	PressKey(KEY_O, 0);
	PressKey(KEY_A, 0);
	PressKey(KEY_D, 0);
	sendPS2(KEY_RSHIFT, 0); // Mantenemos pulsado SHIFT
	PressKey(KEY_2, 100);
	PressKey(KEY_2, 0);
	sendPS2(0xF0, 0); // Liberamos SHIFT
	sendPS2(KEY_RSHIFT, 0);
	PressKey(KEY_ENTER, 0); // ENTER (13)

}

void LOAD48() // LOAD "" en BASIC 48
{

	PressKey(KEY_J, 0);
	sendPS2(KEY_RSHIFT, 0); // Mantenemos pulsado SHIFT
	PressKey(KEY_2, 100);
	PressKey(KEY_2, 0);
	sendPS2(0xF0, 0); // Liberamos SHIFT
	sendPS2(KEY_RSHIFT, 0);
	PressKey(KEY_ENTER, 0); // ENTER (13)

}

void NMI() // CTRL + ALT + F5 (NMI)
{

	sendPS2(KEY_LCTRL, 0); // Mantenemos pulsado LCTRL
	sendPS2(KEY_LALT, 0); // Mantenemos pulsado LALT
	PressKey(KEY_F5, 0); // F5 (116)

	sendPS2(0xF0, 0); // Liberamos LALT
	sendPS2(KEY_LALT, 0);
	sendPS2(0xF0, 0); // Liberamos LCTRL
	sendPS2(KEY_LCTRL, 0);
	Cursors();	

}

void Reset() // CTRL + ALT + Supr (Reset)
{

	sendPS2(KEY_LCTRL, 0); // Mantenemos pulsado LCTRL
	sendPS2(KEY_LALT, 0); // Mantenemos pulsado LALT
	PressKey(KEY_DEL, 0); // Supr

	sendPS2(0xF0, 0); // Liberamos LALT
	sendPS2(KEY_LALT, 0);
	sendPS2(0xF0, 0); // Liberamos LCTRL
	sendPS2(KEY_LCTRL, 0);
	Cursors();

}

void MasterReset() // CTRL + ALT + BackSpace (MasterReset)
{

	sendPS2(KEY_LCTRL, 0); // Mantenemos pulsado LCTRL
	sendPS2(KEY_LALT, 0); // Mantenemos pulsado LALT
	PressKey(KEY_DELETE, 0); // BackSpace

	sendPS2(0xF0, 0); // Liberamos LALT
	sendPS2(KEY_LALT, 0);
	sendPS2(0xF0, 0); // Liberamos LCTRL
	sendPS2(KEY_LCTRL, 0);
	Cursors();	

}

void ChangeKeys()
{
	if (mapper == 0) // El cambio entre cursores y teclas solo disponible para el mapa 0
	{
		KeyMap[0] = KeyMap[0] == KEY_UP ? Map0[0] : KEY_UP;
		KeyMap[1] = KeyMap[1] == KEY_DOWN ? Map0[1] : KEY_DOWN;
		KeyMap[2] = KeyMap[2] == KEY_LEFT ? Map0[2] : KEY_LEFT;
		KeyMap[3] = KeyMap[3] == KEY_RIGHT ? Map0[3] : KEY_RIGHT;
	}
}

int main()
{

	// Setup		
	CPU_PRESCALE(0);
	ps2Init();
	QueuePS2Init();
	mapper = 0; // Mapa por defecto al inicio	
	for (imap = 0; imap < 12; imap++) KeyMap[imap] = Map0[imap];
	Cursors(); // Direcciones del joystick como cursores por defecto al inicio

			   // Entrada desde DB15, activamos resistencias internas pullup en pines digitales 0 a 12
	DB15_PIN01 = 0xFF; // Ponemos en alto pines 0 - 7
	DB15_PIN02 = DB15_PIN02 | 0b00011111; // Ponemos en alto los pines 8 - 12, respetamos el contenido del resto ya que nos los utilzaremos
	DB15_PORT01 = 0; // Input pullup 0 - 7 
	DB15_PORT02 = DB15_PORT02 & 0b11100000; // Input pullup 8 - 12

	DB15_PINAux = 0xFFFF; // Estado inicial del joystick

	// Loop
	while (1) {

		while (QueueIn != QueueOut && !ps2Stat()) // Liberamos buffer de scancodes si las lineas estan en alto
		{

			// CLK debe encontrarse en alto durante al menos 50us
			_delay_us(50);
			if (!(PS2_PIN & (1 << PS2_CLK)))
			{
				break;
			}

			QueuePS2Get(&sendcode, &wait_ms);

			while ((sendcode == 0xE0 || sendcode == 0xF0))
			{
				sendPS2fromqueue(sendcode);
				if (QueueIn != QueueOut)
				{
					QueuePS2Get(&sendcode, &wait_ms);
				}
				else
				{
					goto exit;
				}

			}
			sendPS2fromqueue(sendcode);
			my_delay_ms_10ms_steps(wait_ms);
			break;

		}

	exit:

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
				switch (mapper)
				{
					case 0:
						for (imap = 0; imap < 12; imap++) KeyMap[imap] = Map0[imap];
						break;
					case 1:
						for (imap = 0; imap < 12; imap++) KeyMap[imap] = Map1[imap];
						break;
					case 2:
						for (imap = 0; imap < 12; imap++) KeyMap[imap] = Map2[imap];
						break;
					case 3:
						for (imap = 0; imap < 12; imap++) KeyMap[imap] = Map3[imap];
						break;
					default:
						break;
				}
				continue;
			}

			// Combinaciones para todos los mapeos
			if (DB15_PIN == 0b1110111111101111 && DB15PINPrev == 0b1110111111111111) { mapper = 0; _delay_ms(200); continue; } // Mapper 0 (KeyMapper + Boton 1) -> Inicial
			if (DB15_PIN == 0b1110111111011111 && DB15PINPrev == 0b1110111111111111) { mapper = 1; _delay_ms(200); continue; } // Mapper 1 (KeyMapper + Boton 2)
			if (DB15_PIN == 0b1110111110111111 && DB15PINPrev == 0b1110111111111111) { mapper = 2; _delay_ms(200); continue; } // Mapper 2 (KeyMapper + Boton 3)
			if (DB15_PIN == 0b1110111101111111 && DB15PINPrev == 0b1110111111111111) { mapper = 3; _delay_ms(200); continue; } // Mapper 3 (KeyMapper + Boton 4)

			if (DB15_PIN == 0b1111110111101111 && DB15PINPrev == 0b1111110111111111) { PressKey(KEY_1, 0); _delay_ms(200); continue; } // 1 (Select + Boton 1)
			if (DB15_PIN == 0b1111110111011111 && DB15PINPrev == 0b1111110111111111) { PressKey(KEY_2, 0); _delay_ms(200); continue; } // 2 (Select + Boton 2)
			if (DB15_PIN == 0b1111110110111111 && DB15PINPrev == 0b1111110111111111) { PressKey(KEY_3, 0); _delay_ms(200); continue; } // 3 (Select + Boton 3)
			if (DB15_PIN == 0b1111110101111111 && DB15PINPrev == 0b1111110111111111) { PressKey(KEY_4, 0); _delay_ms(200); continue; } // 4 (Select + Boton 4)
			if (DB15_PIN == 0b1111100111111111 && DB15PINPrev == 0b1111110111111111) { PressKey(KEY_5, 0); _delay_ms(200); continue; } // 5 (Select + Boton 5)
			if (DB15_PIN == 0b1111010111111111 && DB15PINPrev == 0b1111110111111111) { PressKey(KEY_6, 0); _delay_ms(200); continue; } // 6 (Select + Boton 6)

			if (DB15_PIN == 0b1111110011111111 && DB15PINPrev == 0b1111110111111111) { ChangeKeys(); _delay_ms(200); continue; } // (Select + Start) Cursor <-> Map0[0,1,2,3] desde keyup, keydown, keyleft y keyright en el mapa principal

			// Combinaciones para ZXUno
			if (DB15_PIN == 0b1111110111110111 && DB15PINPrev == 0b1111110111111111) { PressKey(KEY_F2, 0); _delay_ms(200); continue; } // F2 (Select + Derecha)
			if (DB15_PIN == 0b1111110111111011 && DB15PINPrev == 0b1111110111111111) { PressKey(KEY_CAPS, 0); _delay_ms(200); continue; } // BloqMayus (Select + Izquierda)
			if (DB15_PIN == 0b1111110111111110 && DB15PINPrev == 0b1111110111111111) { Reset(); _delay_ms(200); continue; } // Reset (Select + Arriba)
			if (DB15_PIN == 0b1111110111111101 && DB15PINPrev == 0b1111110111111111) { MasterReset(); _delay_ms(200); continue; } // MasterReset (Select + Abajo)

			if (DB15_PIN == 0b1111111011111011 && DB15PINPrev == 0b1111111011111111) { LOAD128(); _delay_ms(200); continue; } // Load 128K (Start + Izquierda)
			if (DB15_PIN == 0b1111111011110111 && DB15PINPrev == 0b1111111011111111) { LOAD48(); _delay_ms(200); continue; } // Load 48K (Start + Derecha)
			if (DB15_PIN == 0b1111111011101111 && DB15PINPrev == 0b1111111011111111) { NMI(); _delay_ms(200); continue; } // NMI (Start + Boton 1)              

			
			// Select y Start son especiales para una mejor integracion con las combinaciones, actuan al ser pulsados y despues soltados.
			if (DB15_PIN == 0b1111111111111111 && DB15PINPrev == 0b1111111011111111) { PressKey(KeyMap[5], 0); continue; } // Start
			if (DB15_PIN == 0b1111111111111111 && DB15PINPrev == 0b1111110111111111) { PressKey(KeyMap[4], 0); continue; } // Select

			if (CHECK_BIT(DB15_PIN, 8) && CHECK_BIT(DB15_PIN, 9) &&
				CHECK_BIT(DB15PINPrev, 8) && CHECK_BIT(DB15PINPrev, 9)) // Ignoramos si son pulsados o recien soltados los botones Select o Start
			{
				// Para el resto, se envia pulsacion o liberacion de tecla segun el estado de los mismos.
				if (CHECK_BIT(DB15_PINChanges, 0)) sendCodeMR(KeyMap[0], CHECK_BIT(DB15_PIN, 0), 0);
				if (CHECK_BIT(DB15_PINChanges, 1)) sendCodeMR(KeyMap[1], CHECK_BIT(DB15_PIN, 1), 0);
				if (CHECK_BIT(DB15_PINChanges, 2)) sendCodeMR(KeyMap[2], CHECK_BIT(DB15_PIN, 2), 0);
				if (CHECK_BIT(DB15_PINChanges, 3)) sendCodeMR(KeyMap[3], CHECK_BIT(DB15_PIN, 3), 0);
				if (CHECK_BIT(DB15_PINChanges, 4)) sendCodeMR(KeyMap[6], CHECK_BIT(DB15_PIN, 4), 0);
				if (CHECK_BIT(DB15_PINChanges, 5)) sendCodeMR(KeyMap[7], CHECK_BIT(DB15_PIN, 5), 0);
				if (CHECK_BIT(DB15_PINChanges, 6)) sendCodeMR(KeyMap[8], CHECK_BIT(DB15_PIN, 6), 0);
				if (CHECK_BIT(DB15_PINChanges, 7)) sendCodeMR(KeyMap[9], CHECK_BIT(DB15_PIN, 7), 0);
				if (CHECK_BIT(DB15_PINChanges, 10)) sendCodeMR(KeyMap[10], CHECK_BIT(DB15_PIN, 10), 0);
				if (CHECK_BIT(DB15_PINChanges, 11)) sendCodeMR(KeyMap[11], CHECK_BIT(DB15_PIN, 11), 0);
			}
				
		}

	}
}
