#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include "PS2Keyboard.h"

// Mapas del joystick

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

const unsigned char ScanCodes1[] = { 0x00, 0x48, 0x50, 0x4B, 0x4D, 0x52, 0x47, 0x49, 0x4B, 0x4F, 0x51,   // UP, DOWN, LEFT, RIGHT, INS, HOME, PUP, DEL, END, PDOWN
									 0x1E, 0x30, 0x2E, 0x20, 0x12, 0x21, 0x22, 0x23, 0x17, 0x24,   // A -> J
									 0x25, 0x26, 0x32, 0x31, 0x18, 0x19, 0x10, 0x13, 0x1F, 0x14,   // K -> T
									 0x16, 0x2F, 0x11, 0x2D, 0x15, 0x2C,						   // U -> Z
									 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,   // 1 -> 9, 0
									 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x41, 0x42, 0x43, 0x44, 0x57, 0x58, // F1 -> F12
									 0x4F, 0x50, 0x51, 0x4B, 0x4C, 0x4D, 0x47, 0x48, 0x49, 0x52, 0x35, 0x37, 0x4A, 0x4E, 0x1C, 0x53, 0x45, // Keypad Buttons
									 0x01, 0x0E, 0x34, 0x1D, 0x38, 0x46, 0x1C, 0x39, 0x2A, 0x36, 0x3A, 0x29, 0x0F, 0x0D, 0x33, 0x27 }; // Others

const unsigned char ScanCodes2[] = { 0x00, 0x75, 0x72, 0x6B, 0x74, 0x70, 0x6C, 0x7D, 0x71, 0x69, 0x7A,   // UP, DOWN, LEFT, RIGHT, INS, HOME, PUP, DEL, END, PDOWN
									 0x1C, 0x32, 0x21, 0x23, 0x24, 0x2B, 0x34, 0x33, 0x43, 0x3B,   // A -> J
									 0x42, 0x4B, 0x3A, 0x31, 0x44, 0x4D, 0x15, 0x2D, 0x1B, 0x2C,   // K -> T
									 0x3C, 0x2A, 0x1D, 0x22, 0x35, 0x1A,						   // U -> Z
									 0x16, 0x1E, 0x26, 0x25, 0x2E, 0x36, 0x3D, 0x3E, 0x46, 0x45,   // 1 -> 9, 0
									 0x05, 0x06, 0x04, 0x0C, 0x03, 0x0B, 0x83, 0x0A, 0x01, 0x09, 0x78, 0x07, // F1 -> F12
									 0x69, 0x72, 0x7A, 0x6B, 0x73, 0x74, 0x6C, 0x75, 0x7D, 0x70, 0x4A, 0x7C, 0x7B, 0x79, 0x5A, 0x71, 0x77, // Keypad Buttons
									 0x76, 0x66, 0x49, 0x14, 0x11, 0x7E, 0x5A, 0x29, 0x12, 0x59, 0x58, 0x0E, 0x0D, 0x55, 0x41, 0x4C }; // Others


void my_delay_ms_10ms_steps(int ms)
{
	while (0 < ms)
	{
		_delay_ms(10);
		ms -= 10;
	}
}

void my_delay_us_4us_steps(int us)
{
	while (0 < us)
	{
		_delay_us(4);
		us -= 4;
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

int checkState(int t) {
	int count = 0;

	while (count++ < t) {
		if (!(PS2_PIN & (1 << PS2_DAT)))
			return 1;
		_delay_us(5);
	}

	return 0;
}

int getPS2(unsigned char *ret)
{
	unsigned char data = 0x00;
	unsigned char p = 0x01;
	
	// discard the start bit
	while ((PS2_PIN & (1 << PS2_DAT)));
	while (!(PS2_PIN & (1 << PS2_CLK)));

	// Bit de comienzo
	my_delay_us_4us_steps(CK1 * ckt);
	ps2Mode(PS2_CLK, LO);
	my_delay_us_4us_steps(CK2 * ckt);
	ps2Mode(PS2_CLK, HI);
	my_delay_us_4us_steps(CK1 * ckt);

	// read each data bit
	for (int i = 0; i<8; i++) {		
		if ((PS2_PIN & (1 << PS2_DAT))) {
			data = data | (1 << i);
			p = p ^ 1;
		}
		my_delay_us_4us_steps(CK1 * ckt);
		ps2Mode(PS2_CLK, LO);
		my_delay_us_4us_steps(CK2 * ckt);
		ps2Mode(PS2_CLK, HI);
		my_delay_us_4us_steps(CK1 * ckt);
	}
	
	// read the parity bit	
	
	if (((PS2_PIN & (1 << PS2_DAT)) != 0) != p) {
				
		return -1;
	}
	
	my_delay_us_4us_steps(CK1 * ckt);
	ps2Mode(PS2_CLK, LO);
	my_delay_us_4us_steps(CK2 * ckt);
	ps2Mode(PS2_CLK, HI);
	my_delay_us_4us_steps(CK1 * ckt);

	// send 'ack' bit
	ps2Mode(PS2_DAT, LO);
	my_delay_us_4us_steps(CK1 * ckt);
	ps2Mode(PS2_CLK, LO);
	my_delay_us_4us_steps(CK2 * ckt);
	ps2Mode(PS2_CLK, HI);
	ps2Mode(PS2_DAT, HI);

	_delay_us(100);
	*ret = data;

	return 0;
}

//envio de datos ps/2 simulando reloj con delays.

void sendPS2fromqueue(unsigned char code)
{
	//Para continuar las líneas deben estar en alto  
	while (ps2Stat());

	unsigned char parity = 1;
	int i = 0;

	//iniciamos transmisión
	ps2Mode(PS2_DAT, LO);
	my_delay_us_4us_steps(CK1 * ckt);

	ps2Mode(PS2_CLK, LO); //bit de comienzo
	my_delay_us_4us_steps(CK2 * ckt);
	ps2Mode(PS2_CLK, HI);
	my_delay_us_4us_steps(CK1 * ckt);
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

		my_delay_us_4us_steps(CK1 * ckt);
		ps2Mode(PS2_CLK, LO);
		my_delay_us_4us_steps(CK2 * ckt);
		ps2Mode(PS2_CLK, HI);
		my_delay_us_4us_steps(CK1 * ckt);
	}

	// Enviamos bit de paridad
	if (parity)
		ps2Mode(PS2_DAT, HI);
	else
		ps2Mode(PS2_DAT, LO);

	my_delay_us_4us_steps(CK1 * ckt);
	ps2Mode(PS2_CLK, LO);
	my_delay_us_4us_steps(CK2 * ckt);
	ps2Mode(PS2_CLK, HI);
	my_delay_us_4us_steps(CK1 * ckt);

	//Bit de parada
	ps2Mode(PS2_DAT, HI);
	my_delay_us_4us_steps(CK1 * ckt);
	ps2Mode(PS2_CLK, LO);
	my_delay_us_4us_steps(CK2 * ckt);
	ps2Mode(PS2_CLK, HI);
	my_delay_us_4us_steps(CK1 * ckt);

	_delay_us(50);
	
}

//codifica envio de caracteres ps/2 
void sendCodeMR(unsigned char key, uint16_t release, double ms, unsigned char scancodeset)
{
	uint8_t extn = 0;

	//checkeamos si es una tecla con scancode extendido (E0)
	switch (key) {	
	case KEY_UP:
	case KEY_DOWN:
	case KEY_LEFT:
	case KEY_RIGHT:	
	case KEY_INS:
	case KEY_HOME:
	case KEY_PUP:
	case KEY_DEL:
	case KEY_END:
	case KEY_PDN:
	case KEYPAD_BAR:
	case KEYPAD_ENT:
	
		extn = 1;
		break;
	default:
		extn = 0;
		break;
	}
	//secuencia  

	if (extn)
		sendPS2(0xE0, 0);

	if (key && release && scancodeset == 1) // Set 1 release
		sendPS2(ScanCodes1[key] + 0x80, 0);

	if (key && !release && scancodeset == 1) // Set 1 make
		sendPS2(ScanCodes1[key], 0);

	if (key && release && scancodeset == 2) // Set 2 release
		sendPS2(0xF0, 0);
		
	if (key && scancodeset == 2) // Set 2 make or release
		sendPS2(ScanCodes2[key], ms);
	
}

void PressKey(unsigned char key, double ms, unsigned char scancodeset)
{
	sendCodeMR(key, 0, 100, scancodeset); //Make	
	sendCodeMR(key, 1, ms, scancodeset); //Release
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

void LOAD128(unsigned char scancodeset) // LOAD "" en BASIC 128
{

	PressKey(KEY_L, 0, scancodeset);
	PressKey(KEY_O, 0, scancodeset);
	PressKey(KEY_A, 0, scancodeset);
	PressKey(KEY_D, 0, scancodeset);
	if (scancodeset == 1)
	{
		sendPS2(ScanCodes1[KEY_RSHIFT], 0); // Mantenemos pulsado SHIFT
	}
	if (scancodeset == 2)
	{
		sendPS2(ScanCodes2[KEY_RSHIFT], 0); // Mantenemos pulsado SHIFT
	}
	PressKey(KEY_2, 100, scancodeset);
	PressKey(KEY_2, 0, scancodeset);
	if (scancodeset == 1)
	{
		sendPS2(ScanCodes2[KEY_RSHIFT] + 0x80, 0); // Liberamos SHIFT
	}
	if (scancodeset == 2)
	{
		sendPS2(0xF0, 0); // Liberamos SHIFT
		sendPS2(ScanCodes2[KEY_RSHIFT], 0);
	}	
	PressKey(KEY_ENTER, 0, scancodeset); // ENTER (13)

}

void LOAD48(unsigned char scancodeset) // LOAD "" en BASIC 48
{

	PressKey(KEY_J, 0, scancodeset);
	if (scancodeset == 1)
	{
		sendPS2(ScanCodes1[KEY_RSHIFT], 0); // Mantenemos pulsado SHIFT
	}
	if (scancodeset == 2)
	{
		sendPS2(ScanCodes2[KEY_RSHIFT], 0); // Mantenemos pulsado SHIFT
	}
	PressKey(KEY_2, 100, scancodeset);
	PressKey(KEY_2, 0, scancodeset);
	if (scancodeset == 1)
	{
		sendPS2(ScanCodes2[KEY_RSHIFT] + 0x80, 0); // Liberamos SHIFT
	}
	if (scancodeset == 2)
	{
		sendPS2(0xF0, 0); // Liberamos SHIFT
		sendPS2(ScanCodes2[KEY_RSHIFT], 0);
	}	
	PressKey(KEY_ENTER, 0, scancodeset); // ENTER (13)

}

void NMI(unsigned char scancodeset) // CTRL + ALT + F5 (NMI)
{

	if (scancodeset == 1)
	{
		sendPS2(ScanCodes1[KEY_LCTRL], 0); // Mantenemos pulsado LCTRL
	}
	if (scancodeset == 2)
	{
		sendPS2(ScanCodes2[KEY_LCTRL], 0); // Mantenemos pulsado LCTRL
	}
	if (scancodeset == 1)
	{
		sendPS2(ScanCodes1[KEY_LALT], 0); // Mantenemos pulsado LALT
	}
	if (scancodeset == 2)
	{
		sendPS2(ScanCodes2[KEY_LALT], 0); // Mantenemos pulsado LALT
	}
	PressKey(KEY_F5, 0, scancodeset); // F5 (116)

	if (scancodeset == 1)
	{
		sendPS2(ScanCodes1[KEY_LALT] + 0x80, 0); // Liberamos LALT
	}
	if (scancodeset == 2)
	{
		sendPS2(0xF0, 0); // Liberamos LCTRL
		sendPS2(ScanCodes2[KEY_LALT], 0);
	}
	if (scancodeset == 1)
	{
		sendPS2(ScanCodes1[KEY_LCTRL] + 0x80, 0); // Liberamos LCTRL
	}
	if (scancodeset == 2)
	{
		sendPS2(0xF0, 0); // Liberamos LCTRL
		sendPS2(ScanCodes2[KEY_LCTRL], 0);
	}
	Cursors();


}

void Reset(unsigned char scancodeset) // CTRL + ALT + Supr (Reset)
{
	if (scancodeset == 1)
	{
		sendPS2(ScanCodes1[KEY_LCTRL], 0); // Mantenemos pulsado LCTRL
	}
	if (scancodeset == 2)
	{
		sendPS2(ScanCodes2[KEY_LCTRL], 0); // Mantenemos pulsado LCTRL
	}
	if (scancodeset == 1)
	{
		sendPS2(ScanCodes1[KEY_LALT], 0); // Mantenemos pulsado LALT
	}
	if (scancodeset == 2)
	{
		sendPS2(ScanCodes2[KEY_LALT], 0); // Mantenemos pulsado LALT
	}
	PressKey(KEY_DEL, 0, scancodeset); // Supr

	if (scancodeset == 1)
	{
		sendPS2(ScanCodes1[KEY_LALT] + 0x80, 0); // Liberamos LALT
	}
	if (scancodeset == 2)
	{
		sendPS2(0xF0, 0); // Liberamos LCTRL
		sendPS2(ScanCodes2[KEY_LALT], 0);
	}
	if (scancodeset == 1)
	{
		sendPS2(ScanCodes1[KEY_LCTRL] + 0x80, 0); // Liberamos LCTRL
	}
	if (scancodeset == 2)
	{
		sendPS2(0xF0, 0); // Liberamos LCTRL
		sendPS2(ScanCodes2[KEY_LCTRL], 0);
	}
	Cursors();

}

void MasterReset(int extra, unsigned char scancodeset) // CTRL + ALT + BackSpace (MasterReset)
{

	if (scancodeset == 1)
	{
		sendPS2(ScanCodes1[KEY_LCTRL], 0); // Mantenemos pulsado LCTRL
	}
	if (scancodeset == 2)
	{
		sendPS2(ScanCodes2[KEY_LCTRL], 0); // Mantenemos pulsado LCTRL
	}
	if (scancodeset == 1)
	{
		sendPS2(ScanCodes1[KEY_LALT], 0); // Mantenemos pulsado LALT
	}
	if (scancodeset == 2)
	{
		sendPS2(ScanCodes2[KEY_LALT], 0); // Mantenemos pulsado LALT
	}
	PressKey(KEY_DELETE, 500, scancodeset); // BackSpace

	if (scancodeset == 1)
	{
		sendPS2(ScanCodes1[KEY_LALT] + 0x80, 0); // Liberamos LALT
	}
	if (scancodeset == 2)
	{
		sendPS2(0xF0, 0); // Liberamos LCTRL
		sendPS2(ScanCodes2[KEY_LALT], 0);
	}
	if (scancodeset == 1)
	{
		sendPS2(ScanCodes1[KEY_LCTRL] + 0x80, 0); // Liberamos LCTRL
	}
	if (scancodeset == 2)
	{
		sendPS2(0xF0, 0); // Liberamos LCTRL
		sendPS2(ScanCodes2[KEY_LCTRL], 0);
	}
	Cursors();

	switch (extra)
	{
	case 1: // Entrada a ROMs
		PressKey(KEY_ESCAPE, 200, scancodeset);
		break;
	case 2: // Entrada a cores
		PressKey(KEY_CAPS, 200, scancodeset);
		break;
	case 3: // Entrada a BIOS
		PressKey(KEY_F2, 200, scancodeset);
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
