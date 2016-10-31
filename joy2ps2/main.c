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

#define DB9_PIN   PIND
#define PS2_PORT  PORTC
#define PS2_DDR   DDRC
#define PS2_PIN   PINC

#define PS2_DAT   PC3
#define PS2_CLK   PC2

#define CPU_PRESCALE(n) (CLKPR = 0x80, CLKPR = (n))

#define HI 1
#define LO 0
#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

unsigned char DB9_PINAux;
unsigned char DB9_PINPrev;
unsigned char DB9_PINChanges;
unsigned char temp;
uint8_t KeyStatus[255];

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

	// while (1) {} // Aqui vamos a comprobar con un polímetro que realmente están en alto.

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
void sendCodeMR(unsigned char key, uint8_t release)
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

	PressKey('L');
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

}

void ReleasePad()
{
	sendCodeMR(KEY_UP, 1);
	sendCodeMR(KEY_DOWN, 1);
	sendCodeMR(KEY_LEFT, 1);
	sendCodeMR(KEY_RIGHT, 1);
	sendCodeMR(KEY_Q, 1);
	sendCodeMR(KEY_A, 1);
	sendCodeMR(KEY_O, 1);
	sendCodeMR(KEY_P, 1);
	//temp = 0xFF;
	DB9_PINAux = 0xFF;
	DB9_PINPrev = 0;
	memset(KeyStatus, 0, sizeof(KeyStatus));
	_delay_ms(100);
	//temp = DB9_PIN;

}

void ChangeKeys()
{
	keymapKempston[0b00000001] = keymapKempston[0b00000001] == KEY_UP ? KEY_Q : KEY_UP;
	keymapKempston[0b00000010] = keymapKempston[0b00000010] == KEY_DOWN ? KEY_A : KEY_DOWN;
	keymapKempston[0b00000100] = keymapKempston[0b00000100] == KEY_LEFT ? KEY_O : KEY_LEFT;
	keymapKempston[0b00001000] = keymapKempston[0b00001000] == KEY_RIGHT ? KEY_P : KEY_RIGHT;
}

void Cursors()
{
	keymapKempston[0b00000001] = KEY_UP;
	keymapKempston[0b00000010] = KEY_DOWN;
	keymapKempston[0b00000100] = KEY_LEFT;
	keymapKempston[0b00001000] = KEY_RIGHT;
}

int main()
{		
	// Setup		
	CPU_PRESCALE(0);
	ps2Init();
	DB9_PINAux = 0xFF; // Estado inicial del joystick definido como ninguna entrada activada

	// Entrada desde DB9, activamos resistencias internas pullup en pines digitales 0 a 7
	PORTD = 0xFF;
	temp = DB9_PIN;
	DDRD = 0; // Input pullup 
	DB9_PINChanges = 0;

	// Loop
	while (1) {

		if (temp != DB9_PIN)
		{
			DB9_PINPrev = temp;
			temp = DB9_PIN;

			if (temp == 0b11100011 && DB9_PINPrev == 0b11110011) { PressKey('1'); ReleasePad(); } // 1 (Select + Boton 1) 
			if (temp == 0b11010011 && DB9_PINPrev == 0b11110011) { PressKey('2'); ReleasePad(); } // 2 (Select + Boton 2)
			if (temp == 0b10110011 && DB9_PINPrev == 0b11110011) { PressKey('3'); ReleasePad(); } // 3 (Select + Boton 3)
			if (temp == 0b11110000 && DB9_PINPrev == 0b11110011) { PressKey('4'); ReleasePad(); } // 4 (Select + Boton 4)

			if (temp == 0b11111111 && DB9_PINPrev == 0b11110011) { PressKey(27); ReleasePad(); } // ESC (Select)
			if (temp == 0b11111111 && DB9_PINPrev == 0b11111100) { PressKey(13); ReleasePad(); } // Intro (Start)
			if (temp == 0b10111100 && DB9_PINPrev == 0b11111100) { PressKey(113); ReleasePad(); } // F2 (Start + Boton 3)
			if (temp == 0b11111111 && DB9_PINPrev == 0b11110000) { ChangeKeys(); ReleasePad(); } // Cambio de teclado (Boton 4)

			if (temp == 0b11101100 && DB9_PINPrev == 0b11111100) { NMI(); ReleasePad(); } // NMI (Start + Boton 1)
			if (temp == 0b11111000 && DB9_PINPrev == 0b11111100) { LOAD128(); ReleasePad(); } // Load 128K (Start + Izquierda)
			if (temp == 0b11110100 && DB9_PINPrev == 0b11111100) { LOAD48(); ReleasePad(); } // Load 48K (Start + Derecha)
			if (temp == 0b11110010 && DB9_PINPrev == 0b11110011) { Reset(); ReleasePad(); } // Reset (Select + Arriba)
			if (temp == 0b11110001 && DB9_PINPrev == 0b11110011) { MasterReset(); ReleasePad(); } // MasterReset (Select + Abajo)

			if ((DB9_PINChanges = temp ^ DB9_PINAux) != 0 && !(!CHECK_BIT(temp, 0) & !CHECK_BIT(temp, 1)) && !(!CHECK_BIT(temp, 2) & !CHECK_BIT(temp, 3))) // Esperamos a que se produzca un cambio en el estado del joystick, y evitamos combinaciones imposibles.
			{

				DB9_PINAux = temp; // Guardamos el estado actual del joystick.      
				KeyStatus[DB9_PINChanges] = !KeyStatus[DB9_PINChanges]; // Guardamos el estado del evento anterior al cambio.
				sendCodeMR(keymapKempston[DB9_PINChanges], !KeyStatus[DB9_PINChanges]);

			}

		}
		
	}
		
	
}

