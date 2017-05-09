#include <avr/io.h>
#include <util/delay.h>
#include "PS2Keyboard.h"

// Mapas del joystick

const unsigned charinimap[] = {

	'[','J', 'O', 'Y', '2', 'P', 'S', '2', ']', '0', '0', '0', '6', 'b'

};

const unsigned char db15 = 0;

const unsigned char Map0[] = { // Mapa 0 -> Por defecto al iniciar
	KEY_Q,       	//DB15 00  UP		DB9_1 00  UP
	KEY_A,       	//DB15 01  DOWN		DB9_1 01  DOWN
	KEY_O,       	//DB15 02  LEFT		DB9_1 02  LEFT
	KEY_P,       	//DB15 03  RIGHT	DB9_1 03  RIGHT
	KEY_5,	        //DB15 04  SELECT	DB9_1 04  BUTTON 1
	KEY_1,		    //DB15 05  START	DB9_1 05  BUTTON 2
	KEY_SPACE,      //DB15 06  BUTTON 1	DB9_2 06  UP
	KEY_V,       	//DB15 07  BUTTON 2	DB9_2 07  DOWN
	KEY_B,       	//DB15 08  BUTTON 3	DB9_2 08  LEFT
	KEY_N,       	//DB15 09  BUTTON 4	DB9_2 09  RIGHT
	KEY_G,       	//DB15 10  BUTTON 5	DB9_2 10  BUTTON 1
	KEY_H        	//DB15 11  BUTTON 6	DB9_2 11  BUTTON 2

};

const unsigned char Map1[] = { // Mapa 1
	0,       //DB15 00  UP			DB9_1 00  UP
	0,       //DB15 01  DOWN		DB9_1 01  DOWN
	0,       //DB15 02  LEFT		DB9_1 02  LEFT
	0,       //DB15 03  RIGHT		DB9_1 03  RIGHT
	0,       //DB15 04  SELECT		DB9_1 04  BUTTON 1
	0,       //DB15 05  START		DB9_1 05  BUTTON 2
	0,       //DB15 06  BUTTON 1	DB9_2 06  UP
	0,       //DB15 07  BUTTON 2	DB9_2 07  DOWN
	0,       //DB15 08  BUTTON 3	DB9_2 08  LEFT
	0,       //DB15 09  BUTTON 4	DB9_2 09  RIGHT
	0,       //DB15 10  BUTTON 5	DB9_2 10  BUTTON 1
	0        //DB15 11  BUTTON 6	DB9_2 11  BUTTON 2

};

const unsigned char Map2[] = { // Mapa 2
	0,       //DB15 00  UP			DB9_1 00  UP
	0,       //DB15 01  DOWN		DB9_1 01  DOWN
	0,       //DB15 02  LEFT		DB9_1 02  LEFT
	0,       //DB15 03  RIGHT		DB9_1 03  RIGHT
	0,       //DB15 04  SELECT		DB9_1 04  BUTTON 1
	0,       //DB15 05  START		DB9_1 05  BUTTON 2
	0,       //DB15 06  BUTTON 1	DB9_2 06  UP
	0,       //DB15 07  BUTTON 2	DB9_2 07  DOWN
	0,       //DB15 08  BUTTON 3	DB9_2 08  LEFT
	0,       //DB15 09  BUTTON 4	DB9_2 09  RIGHT
	0,       //DB15 10  BUTTON 5	DB9_2 10  BUTTON 1
	0        //DB15 11  BUTTON 6	DB9_2 11  BUTTON 2

};

const unsigned char Map3[] = { // Mapa 3
	0,       //DB15 00  UP			DB9_1 00  UP
	0,       //DB15 01  DOWN		DB9_1 01  DOWN
	0,       //DB15 02  LEFT		DB9_1 02  LEFT
	0,       //DB15 03  RIGHT		DB9_1 03  RIGHT
	0,       //DB15 04  SELECT		DB9_1 04  BUTTON 1
	0,       //DB15 05  START		DB9_1 05  BUTTON 2
	0,       //DB15 06  BUTTON 1	DB9_2 06  UP
	0,       //DB15 07  BUTTON 2	DB9_2 07  DOWN
	0,       //DB15 08  BUTTON 3	DB9_2 08  LEFT
	0,       //DB15 09  BUTTON 4	DB9_2 09  RIGHT
	0,       //DB15 10  BUTTON 5	DB9_2 10  BUTTON 1
	0        //DB15 11  BUTTON 6	DB9_2 11  BUTTON 2

};

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

void FreeKBBuffer()
{
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
				return;
			}

		}
		sendPS2fromqueue(sendcode);
		my_delay_ms_10ms_steps(wait_ms);
		break;

	}
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
	KeyMap[0] = KEY_UP;
	KeyMap[1] = KEY_DOWN;
	KeyMap[2] = KEY_LEFT;
	KeyMap[3] = KEY_RIGHT;
	KeyMap[6] = KEY_ENTER;
	KeyMap[7] = KEY_ESCAPE;
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

void MasterReset(int extra) // CTRL + ALT + BackSpace (MasterReset)
{

	sendPS2(KEY_LCTRL, 0); // Mantenemos pulsado LCTRL
	sendPS2(KEY_LALT, 0); // Mantenemos pulsado LALT
	PressKey(KEY_DELETE, 500); // BackSpace

	sendPS2(0xF0, 0); // Liberamos LALT
	sendPS2(KEY_LALT, 0);
	sendPS2(0xF0, 0); // Liberamos LCTRL
	sendPS2(KEY_LCTRL, 0);
	Cursors();

	switch (extra)
	{
	case 1: // Entrada a ROMs
		PressKey(KEY_ESCAPE, 200);
		break;
	case 2: // Entrada a cores
		PressKey(KEY_CAPS, 200);
		break;
	case 3: // Entrada a BIOS
		PressKey(KEY_F2, 200);
		break;
	default:

		break;
	}

}

void ChangeKeys()
{
	if (mapper == 0) // El cambio entre cursores y teclas solo disponible para el mapa 0
	{
		unsigned char KeyMapAux[12];
		for (imap = 0; imap < 12; imap++) KeyMapAux[imap] = Map0[imap];

		KeyMap[0] = KeyMap[0] == KEY_UP ? KeyMapAux[0] : KEY_UP;
		KeyMap[1] = KeyMap[1] == KEY_DOWN ? KeyMapAux[1] : KEY_DOWN;
		KeyMap[2] = KeyMap[2] == KEY_LEFT ? KeyMapAux[2] : KEY_LEFT;
		KeyMap[3] = KeyMap[3] == KEY_RIGHT ? KeyMapAux[3] : KEY_RIGHT;
		KeyMap[6] = KeyMap[6] == KEY_ENTER ? KeyMapAux[6] : KEY_ENTER;
		KeyMap[7] = KeyMap[7] == KEY_ESCAPE ? KeyMapAux[7] : KEY_ESCAPE;
	}
}

void SetMap(int map)
{		
	switch (map)
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
	

}
unsigned char CheckDB15()
{
	return db15;
}