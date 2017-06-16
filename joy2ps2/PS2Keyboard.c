#include <avr/io.h>
#include <util/delay.h>
#include "PS2Keyboard.h"

// Mapas del joystick

const unsigned charinimap[] = {

	'[','J', 'O', 'Y', '2', 'P', 'S', '2', ']', '0', '0', '0', '9', 'b'

};

const unsigned char P1Map0[] = { // Mapa 0 - Player 1 -> Por defecto al iniciar (OPQA)
	KEY_Q,       	// UP
	KEY_A,       	// DOWN
	KEY_O,       	// LEFT
	KEY_P,       	// RIGHT
	KEY_5,	        // SELECT
	KEY_1,		    // START
	KEY_SPACE,      // BUTTON 1
	KEY_E,       	// BUTTON 2
	KEY_R,       	// BUTTON 3
	KEY_D,       	// BUTTON 4
	KEY_F,       	// BUTTON 5
	KEY_C        	// BUTTON 6

};

const unsigned char P2Map0[] = { // Mapa 0 - Player 2 -> Por defecto al iniciar (IKJL)
	KEY_I,       	// UP
	KEY_K,       	// DOWN
	KEY_J,       	// LEFT
	KEY_L,       	// RIGHT
	KEY_6,	        // SELECT
	KEY_2,		    // START
	KEY_H,		    // BUTTON 1
	KEY_G,       	// BUTTON 2
	KEY_N,       	// BUTTON 3
	KEY_B,       	// BUTTON 4
	KEY_Y,       	// BUTTON 5
	KEY_T        	// BUTTON 6

};

const unsigned char P1Map1[] = { // Mapa 1 - Player 1 (WSAD)
	KEY_W,       	// UP
	KEY_S,       	// DOWN
	KEY_A,       	// LEFT
	KEY_D,       	// RIGHT
	KEY_5,	        // SELECT
	KEY_1,		    // START
	KEY_F,		    // BUTTON 1
	KEY_E,       	// BUTTON 2
	KEY_R,       	// BUTTON 3
	KEY_D,       	// BUTTON 4
	KEY_F,       	// BUTTON 5
	KEY_C        	// BUTTON 6

};

const unsigned char P2Map1[] = { // Mapa 1 - Player 2 (QAOP)
	KEY_I,       	// UP
	KEY_K,       	// DOWN
	KEY_J,       	// LEFT
	KEY_L,       	// RIGHT
	KEY_6,	        // SELECT
	KEY_2,		    // START
	KEY_H,		    // BUTTON 1
	KEY_G,       	// BUTTON 2
	KEY_N,       	// BUTTON 3
	KEY_B,       	// BUTTON 4
	KEY_Y,       	// BUTTON 5
	KEY_T        	// BUTTON 6

};

const unsigned char P1Map2[] = { // Mapa 2 - Player 1 (Default MAME)
	KEY_UP,       	// UP
	KEY_DOWN,      	// DOWN
	KEY_LEFT,      	// LEFT
	KEY_RIGHT,     	// RIGHT
	KEY_5,			// SELECT
	KEY_1,			// START
	KEY_LCTRL,		// BUTTON 1
	KEY_LALT,       // BUTTON 2
	KEY_SPACE,      // BUTTON 3
	KEY_LSHIFT,     // BUTTON 4
	KEY_Z,       	// BUTTON 5
	KEY_X        	// BUTTON 6

};

const unsigned char P2Map2[] = { // Mapa 2 - Player 2 (Default MAME)
	KEY_R,       	// UP
	KEY_F,       	// DOWN
	KEY_D,       	// LEFT
	KEY_G,       	// RIGHT
	KEY_6,	        // SELECT
	KEY_2,		    // START
	KEY_A,		    // BUTTON 1
	KEY_S,       	// BUTTON 2
	KEY_Q,       	// BUTTON 3
	KEY_W,       	// BUTTON 4
	KEY_E,       	// BUTTON 5 // Not set by default on MAME
	KEY_T        	// BUTTON 6 // Not set by default on MAME

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
	int i = 0;

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
		if (code & (1 << i))
		{
			ps2Mode(PS2_DAT, HI);
			parity = parity ^ 1;
		}
		else
			ps2Mode(PS2_DAT, LO);

		_delay_us(CK1);
		ps2Mode(PS2_CLK, LO);
		_delay_us(CK2);
		ps2Mode(PS2_CLK, HI);
		_delay_us(CK1);
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

void sendCodeMRE0(unsigned char key, uint16_t release, double ms)
{
	//secuencia  

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

void PressKeyWithE0(unsigned char key, double ms)
{
	sendCodeMRE0(key, 0, 100); //Make	
	sendCodeMRE0(key, 1, ms); //Release
}

void Cursors()
{	
	P1KeyMap[0] = KEY_UP;
	P1KeyMap[1] = KEY_DOWN;
	P1KeyMap[2] = KEY_LEFT;
	P1KeyMap[3] = KEY_RIGHT;
	P1KeyMap[6] = KEY_ENTER;
	P1KeyMap[7] = KEY_ESCAPE;
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
	if (p1map == 0) // El cambio entre cursores y teclas solo disponible para el mapa 0
	{
		unsigned char P1KeyMapAux[12];
		for (imap = 0; imap < 12; imap++) P1KeyMapAux[imap] = P1Map0[imap];

		P1KeyMap[0] = P1KeyMap[0] == KEY_UP ? P1KeyMapAux[0] : KEY_UP;
		P1KeyMap[1] = P1KeyMap[1] == KEY_DOWN ? P1KeyMapAux[1] : KEY_DOWN;
		P1KeyMap[2] = P1KeyMap[2] == KEY_LEFT ? P1KeyMapAux[2] : KEY_LEFT;
		P1KeyMap[3] = P1KeyMap[3] == KEY_RIGHT ? P1KeyMapAux[3] : KEY_RIGHT;
		P1KeyMap[6] = P1KeyMap[6] == KEY_ENTER ? P1KeyMapAux[6] : KEY_ENTER;
		P1KeyMap[7] = P1KeyMap[7] == KEY_ESCAPE ? P1KeyMapAux[7] : KEY_ESCAPE;
	}
}

void SetMapP1(int map)
{		
	switch (map)
	{
	case 0:
		for (imap = 0; imap < 12; imap++) P1KeyMap[imap] = P1Map0[imap];
		break;
	case 1:
		for (imap = 0; imap < 12; imap++) P1KeyMap[imap] = P1Map1[imap];
		break;
	case 2:
		for (imap = 0; imap < 12; imap++) P1KeyMap[imap] = P1Map2[imap];
		break;

	default:
		break;
	}
	
}

void SetMapP2(int map)
{
	switch (map)
	{
	case 0:
		for (imap = 0; imap < 12; imap++) P2KeyMap[imap] = P2Map0[imap];
		break;
	case 1:
		for (imap = 0; imap < 12; imap++) P2KeyMap[imap] = P2Map1[imap];
		break;		
		case 2:
		for (imap = 0; imap < 12; imap++) P2KeyMap[imap] = P2Map2[imap];
		break;
	default:
		break;
	}

}
