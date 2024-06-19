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
#include <util/delay.h>
#include <avr/eeprom.h>
#include "PS2Keyboard.h"
#include "report.h"
#include "direct.h"
#include "famicom.h"

#define LED_CONFIG	DDRB |= (1<<5)
#define LED_ON	PORTB |= (1<<5)		
#define LED_OFF	PORTB &= ~(1<<5)



const unsigned char MenuOptions[] = { KEY_R, KEY_0, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9 };
const unsigned char Keys[] = { KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, KEY_J, KEY_K, KEY_L,
                               KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X,
                               KEY_Y, KEY_Z, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0 };

static	report_t		p1, p1prev;
static	report_t		p2, p2prev;
static  uchar			p1selectnesclon, p1startnesclon;
static  uchar			p2selectnesclon, p2startnesclon;
static  int				keystrokes_supr;
unsigned char			keybinit;
unsigned char			db15;
unsigned char			hostdata;
unsigned char			prevhostdata;
unsigned char			scancodeset;

void CheckP1SelectStartNesClon()
{
	if (p1.up && p1.down)
	{
		p1.select = 1;
		p1selectnesclon = 1;
		p1.up = 0;
		p1.down = 0;
	}
	else
	{
		if (p1selectnesclon)
		{
			p1.select = 0;
			p1selectnesclon = 0;
		}
		p1.up = p1.up && !p1prev.select;
		p1.down = p1.down && !p1prev.select;
	}

	if (p1.left && p1.right)
	{
		p1.start = 1;
		p1startnesclon = 1;
		p1.left = 0;
		p1.right = 0;
	}
	else
	{
		if (p1startnesclon)
		{
			p1.start = 0;
			p1startnesclon = 0;
		}
		p1.left = p1.left && !p1prev.start;
		p1.right = p1.right && !p1prev.start;
	}
}

void CheckP2SelectStartNesClon()
{
	if (p2.up && p2.down)
	{
		p2.select = 1;
		p2selectnesclon = 1;
		p2.up = 0;
		p2.down = 0;
	}
	else
	{
		if (p2selectnesclon)
		{
			p2.select = 0;
			p2selectnesclon = 0;
		}
		p2.up = p2.up && !p2prev.select;
		p2.down = p2.down && !p2prev.select;
	}

	if (p2.left && p2.right)
	{
		p2.start = 1;
		p2startnesclon = 1;
		p2.left = 0;
		p2.right = 0;
	}
	else
	{
		if (p2startnesclon)
		{
			p2.start = 0;
			p2startnesclon = 0;
		}
		p2.left = p2.left && !p2prev.start;
		p2.right = p2.right && !p2prev.start;
	}
}

void keystroke_press(unsigned char key)
{
	if (!keystrokes_supr)
	{
		PressKey(key, 0, scancodeset);
		keystrokes_supr = 1;
	}
	else
	{
		PressKey(KEY_DELETE, 0, scancodeset);
		PressKey(key, 0, scancodeset);
	}

}

int main()
{	
	
	uchar shiftmode = 0;
	int menuoption = -1, resetoption = -1, combioption = -1, extraoptions = -1;	
	int keystrokes_idx = 35;
	int rshift = 0;
	uchar famicom_p1 = 0, famicom_p2 = 0; 
	db15 = 0; // Modo DB9 por defecto.
	keybinit = 0; // Por defecto, escucha deshabilitada para evitar conflictos con teclados reales
	p1map = 0, p2map = 0;
	keystrokes_supr = 0;	
	ckt = 4; // Por defecto, tiempos de 16/32 us para semireloj y reloj
		
	ps2Init();
	QueuePS2Init();	
	
	p1selectnesclon = 0;
	p1startnesclon = 0;
	p2selectnesclon = 0;
	p2startnesclon = 0;		

	// Entrada desde DB15, activamos resistencias internas pullup en pines digitales 0 a 12
	DDRC &= ~(1 << 4);	//set UP_BUTTON		as input -> Analogic Pin 4 <-> Digital 0
	DDRC &= ~(1 << 5);	//set DOWN_BUTTON	as input -> Analogic Pin 5 <-> Digital 1
	PORTC |= (1 << 4);	//set UP_BUTTON 	pull up on -> Analogic Pin 4 <-> Digital 0
	PORTC |= (1 << 5);	//set DOWN_BUTTON 	pull up on -> Analogic Pin 5 <-> Digital 1
	
	DB15_PIN01 = 0b11111111; // Ponemos en alto pines 0 - 7
	DB15_PIN02 = DB15_PIN02 | 0b00011111; // Ponemos en alto los pines 8 - 12, respetamos el contenido del resto ya que nos los utilzaremos
	DB15_PORT01 = 0; // Input pullup 0 - 7 
	DB15_PORT02 = DB15_PORT02 & 0b11100000; // Input pullup 8 - 12

	DDRC |= (1 << 0);		// Select as output (Pin A0 -> Player 1)
	DDRC |= (1 << 1);		// Select as output (Pin A1 -> Player 2)
	
	PORTC |= (1 << 0);		// Select Player 1 high
	PORTC |= (1 << 1);		// Select Player 2 high		

	hostdata = 0;
	scancodeset = 2;

	LED_CONFIG;
	LED_ON;

	SetMapP1(p1map);
	SetMapP2(p2map);

	Cursors(); // Direcciones del joystick como cursores y ENTER por defecto en el inicio
		
	if (!db15)
	{
		if ((DB15_PIN01 & (1 << 2)) && (DB15_PIN01 & (1 << 3))) // Nos aseguramos que Left y Right en norma Atari no se encuentran pulsados
			famicom_p1 = CheckFamicomP1(); // Famicom se considera detectado si se encuentra pulsado Select
		
		if ((DB15_PIN01 & (1 << 7)) && (DB15_PIN02 & (1 << 2))) // Nos aseguramos que Left y Right en norma Atari no se encuentran pulsados
			famicom_p2 = CheckFamicomP2(); // Famicom se considera detectado si se encuentra pulsado Select

		while (CheckFamicomP1());
		while (CheckFamicomP2());
	}

	// Loop
	while (1) {		

		if ((keybinit | shiftmode) && ps2Stat()) // Lineas CLK y/o DATA a 0
		{
				
			// wait for response
			while (checkState(1000)) // tramos de 5 us (5000 us)
			{

				prevhostdata = hostdata;
				if (getPS2(&hostdata) == 0)
				{

					if (hostdata == 0xEE)
					{						
						sendPS2fromqueue(0xEE); // Echo
					}
					else
					{
						sendPS2fromqueue(0xFA); // Ack
					}
					switch (hostdata) {
					case 0x00: // second bit of 0xED or 0xF3 (or get scancode set)			
						if (prevhostdata == 0xF0)
						{
							sendPS2fromqueue(scancodeset);
						}
						break;
					case 0x01: // set scancode 1					
						if (prevhostdata == 0xF0)
						{
							scancodeset = 1;
							shiftmode = 0;
						}
						break;
					case 0x02: // set scancode 2					
						if (prevhostdata == 0xF0)
						{
							scancodeset = 2;
							shiftmode = 0;
						}
						break;
					case 0xED: // set/reset LEDs						
						break;
					case 0xF2: // ID
						sendPS2fromqueue(0xAB);
						sendPS2fromqueue(0x83);
						break;
					case 0xF0: // get/set scancode set					
						break;
					case 0xF3: // set/reset typematic delay						
						break;
					case 0xF4: // keyboard is enabled, break loop						
						break;
					case 0xF5: // keyboard is disabled, break loop						
						break;
					case 0xFF:
						// tell host we are ready to connect
						sendPS2fromqueue(0xAA);
						break;
					default:
						break;
					}
					
				}

			}
			

		}
		else // Lineas CLK/DATA libres, liberamos buffer de eventos.

		{
			FreeKBBuffer();
		}

		p1prev.up = p1.up; p2prev.up = p2.up;
		p1prev.down = p1.down; p2prev.down = p2.down;
		p1prev.left = p1.left; p2prev.left = p2.left;
		p1prev.right = p1.right; p2prev.right = p2.right;
		p1prev.select = p1.select; p2prev.select = p2.select; p1.select = 0;
		p1prev.start = p1.start; p2prev.start = p2.start; p1.start = 0;
		p1prev.button1 = p1.button1; p2prev.button1 = p2.button1;
		p1prev.button2 = p1.button2; p2prev.button2 = p2.button2;
		p1prev.button3 = p1.button3; p2prev.button3 = p2.button3; p1.button3 = 0;
		p1prev.button4 = p1.button4; p2prev.button4 = p2.button4; p1.button4 = 0;
		p1prev.button5 = p1.button5; p2prev.button5 = p2.button5; p1.button5 = 0;
		p1prev.button6 = p1.button6; p2prev.button6 = p2.button6; p1.button6 = 0;
		p1prev.keymapper = p1.keymapper;

		_delay_ms(10); // Para evitar efecto rebote en la lectura de pulsaciones de los gamepads

		if (db15)
		{
			// Player 1
			ReadDB9P1(&p1);
			ReadDB15(&p1);			
		}
		else
		{
			// Player 1
			if (famicom_p1) ReadFamicomP1(&p1);
			else
			{
				ReadDB9P1(&p1);
				CheckP1SelectStartNesClon();
			}
			// Player 2
			if (famicom_p2) ReadFamicomP2(&p2);
			else
			{
				ReadDB9P2(&p2);
				CheckP2SelectStartNesClon();
			}
			
		}
		p1.keymapper = !(DB15_PIN02 & (1 << 4));  // Keymapper
		
		if			
			((p1.start && p1.button1) ||				// Start + Button 1 (SHIFT MODE)
			(p1.select && p1.start) ||				// Select + Start (SHIFT MODE)
			(p1prev.keymapper && !p1.keymapper)      // Keymapper (SHIFT MODE)	
			)				
			
		{			
			shiftmode = !shiftmode;
			if (!shiftmode)
			{
				p1prev.select = 0; p1prev.start = 0; p1prev.button1 = 0;
				if (!db15 && p1prev.keymapper && !p1.keymapper)
				{
					if ((DB15_PIN01 & (1 << 2)) && (DB15_PIN01 & (1 << 3))) // Nos aseguramos que Left y Right en norma Atari no se encuentran pulsados
						famicom_p1 = CheckFamicomP1(); // Famicom se considera detectado si se encuentra pulsado Select

					if ((DB15_PIN01 & (1 << 7)) && (DB15_PIN02 & (1 << 2))) // Nos aseguramos que Left y Right en norma Atari no se encuentran pulsados
						famicom_p2 = CheckFamicomP2(); // Famicom se considera detectado si se encuentra pulsado Select

					while (CheckFamicomP1());
					while (CheckFamicomP2());
				}
			}

			while (p1.select || p1.start || p1.button1 || p1.up || p1.down || p1.left || p1.right)
			{

				p1.button3 = 0; p1.button4 = 0; p1.button5 = 0; p1.button6 = 0; p1.select = 0; p1.start = 0;
				_delay_ms(10); // Para evitar efecto rebote en la lectura de pulsaciones de los gamepads

				if (db15)
				{
					// Player 1
					ReadDB9P1(&p1);
					ReadDB15(&p1);					
				}
				else
				{

					// Player 1
					if (famicom_p1) ReadFamicomP1(&p1);
					else
					{
						ReadDB9P1(&p1);
						CheckP1SelectStartNesClon();
					}
					// Player 2
					if (famicom_p2) ReadFamicomP2(&p2);
					else
					{
						ReadDB9P2(&p2);
						CheckP2SelectStartNesClon();
					}
				}				
				p1.keymapper = !(DB15_PIN02 & (1 << 4));  // Keymapper
				continue;
			}

			// Reset counters
			menuoption = -1;
			resetoption = -1;
			combioption = -1;
			extraoptions = -1;

			_delay_ms(200);

		}
		
		if (shiftmode)
		{
						
			if (p1.start && p1prev.up && !p1.up) // Reducimos CK1 y CK2 hasta 4 / 8 minimo en tramos de 4 / 8
			{
				if (ckt > 1)
				{
					ckt--;
				}
				p1prev.up = 0;
				_delay_ms(200);
				continue;
			}

			if (p1.start && p1prev.down && !p1.down) // Aumentamos CK1 y CK2 hasta 16 / 32 maximo en tramos de 4 / 8
			{

				if (ckt < 4)
				{
					ckt++;
				}
				p1prev.down = 0;
				_delay_ms(200);
				continue;
			}

			// Up -> add menuoption counter
			if (p1prev.up && !p1.up)			
			{
				menuoption += (int)(menuoption < 10);
				combioption = -1;
				resetoption = -1;
				extraoptions = -1;

			}

			// Down -> add resetoption counter
			if (p1prev.down && !p1.down)
			{
				resetoption += (int)(resetoption < 4);
				combioption = -1;
				menuoption = -1;
				extraoptions = -1;
			}

			// Right -> add combioption counter
			if (p1prev.right && !p1.right)
			{
				combioption += (int)(combioption < 2);
				menuoption = -1;
				resetoption = -1;
				extraoptions = -1;
			}

			// Left -> Extraoptions
			if (p1prev.left && !p1.left)
			{
				extraoptions += (int)(extraoptions < 3);
				menuoption = -1;
				resetoption = -1;
				combioption = -1;
			}

			if (p2prev.button1 && !p2.button1) // Final combo P2
			{
				_delay_ms(200);
				shiftmode = 0;
				p2.button1 = 0;
				p2prev.button1 = 0;

				if (extraoptions >= 0)
				{
					if (extraoptions == 2) // Keymap P2
					{
						if (p2map < 3) p2map++; else p2map = 0;
						SetMapP2(p2map);
					}
				}
			}

			if (p1.button1 && p1.button2) // db15 / db9
			{
				_delay_ms(500);
				shiftmode = 0;
				p1.button1 = 0; p1.button2 = 0;
				p1prev.button1 = 0; p1prev.button2 = 0;
				db15 = !db15;
			}

			if (p1.button1 && p1.left) // Desactivacion de escucha del Host y cambio de set de scancodes de (1 o 2)
			{
				_delay_ms(500);
				if (scancodeset == 2) scancodeset = 1; else scancodeset = 2;
				shiftmode = 0;
				p1.button1 = 0; p1.left = 0;
				p1prev.button1 = 0; p1prev.left = 0;
				keybinit = 0;
			}

			if (p1.button1 && p1.right) // Activacion de escucha del Host
			{
				_delay_ms(500);
				shiftmode = 0;
				p1.button1 = 0; p1.right = 0;
				p1prev.button1 = 0; p1prev.right = 0;
				keybinit = 1;
			}

			if (p1prev.button1 && !p1.button1) // Final combo
			{
				_delay_ms(200);
				shiftmode = 0;
				p1.button1 = 0;
				p1prev.button1 = 0;

				// ChangeKeys
				if (menuoption == -1 && resetoption == -1 && combioption == -1 && extraoptions == -1)
				{
					ChangeKeys();
				}

				// Right counts -> Combination options: NMI, LOAD128, LOAD48
				if (combioption >= 0)
				{
					if (combioption == 0)
					{
						NMI(scancodeset);
					}
					if (combioption == 1)
					{
						LOAD128(scancodeset);
					}
					if (combioption == 2)
					{
						LOAD48(scancodeset);
					}
					continue;
				}

				// Up counts -> Menu options: R, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
				if (menuoption >= 0)
				{
					PressKey(MenuOptions[(int)menuoption], 200, scancodeset);
					continue;
				}

				// Down counts -> Reset options: Reset, MasterReset, ROMs, cores, BIOS
				if (resetoption >= 0)
				{
					p1map = 0, p2map = 0;
					SetMapP1(p1map);
					SetMapP2(p2map);					
					ckt = 4; // Por defecto, tiempos de 16/32 us para semireloj y reloj
					keybinit = 0; // Por defecto, escucha deshabilitada para evitar conflictos con teclados reales

					if (resetoption == 0)
					{
						Reset(scancodeset);
					}
					if (resetoption == 1)
					{
						MasterReset(0, scancodeset);
					}
					if (resetoption == 2)
					{
						MasterReset(1, scancodeset);
					}
					if (resetoption == 3)
					{
						MasterReset(2, scancodeset);
					}
					if (resetoption == 4)
					{
						MasterReset(3, scancodeset);
					}
					continue;
				}

				if (extraoptions >= 0)
				{
					if (extraoptions == 0) // -> extraoptions == 0 sera KEYSTROKES en modo noshift (!shiftmode)
					{
						keystrokes_supr = 0;
					}
					if (extraoptions == 1)
					{
						PressKey(KEY_ESCAPE, 0, scancodeset);
					}
					if (extraoptions == 2) // Keymap P1
					{
						if (p1map < 3) p1map++; else p1map = 0;
						SetMapP1(p1map);
					}
					if (extraoptions == 3) // Cambio de Video (Scroll / Lock)
					{
						PressKey(KEY_SCROLL, 0, scancodeset);
					}

				}

			}

		}
		else
		{						

			if (extraoptions == 0) // modo KEYSTROKES
			{

				if (p1prev.button1 && !p1.button1 && keystrokes_supr) // Button 1 -> Hará de KEY_RSHIFT para acceder a mayusculas y a caractéres especiales.									
				{
					rshift = !rshift;
					PressKey(KEY_DELETE, 0, scancodeset);
					if (rshift)
					{
						sendCodeMR(KEY_RSHIFT, 0, 0, scancodeset);
						PressKey(Keys[keystrokes_idx], 0, scancodeset);
						sendCodeMR(KEY_RSHIFT, 1, 0, scancodeset);
					}
					else
					{
						PressKey(Keys[keystrokes_idx], 0, scancodeset);
					}
					FreeKBBuffer();
				}

				if (p1.up && !p1.button1)
				{
					keystrokes_idx = keystrokes_idx < 35 ? keystrokes_idx + 1 : 0;
					keystroke_press(Keys[keystrokes_idx]);
					rshift = 0;
					FreeKBBuffer();

				}
				if (p1.down && !p1.button1)
				{
					keystrokes_idx = keystrokes_idx > 0 ? keystrokes_idx - 1 : 35;
					keystroke_press(Keys[keystrokes_idx]);
					rshift = 0;
					FreeKBBuffer();
				}
				if (p1.right && !p1prev.right)
				{
					if (p1.button1)
					{
						sendCodeMR(KEY_RSHIFT, 1, 0, scancodeset); // Liberamos SHIFT
					}
					if (keystrokes_supr)
					{
						keystrokes_supr = 0;
					}
					else
					{
						PressKey(KEY_SPACE, 0, scancodeset);
					}
					rshift = 0;
					FreeKBBuffer();
				}

				if (p1.left && !p1prev.left && !p1.button1)
				{
					if (p1.button1)
					{
						sendCodeMR(KEY_RSHIFT, 1, 0, scancodeset); // Liberamos SHIFT
					}
					keystrokes_supr = 0;
					PressKey(KEY_DELETE, 0, scancodeset);
					rshift = 0;
					FreeKBBuffer();
				}

				if (p1.button2 && !p1prev.button2 && !p1.button1)
				{
					if (p1.button1)
					{
						sendCodeMR(KEY_RSHIFT, 1, 0, scancodeset); // Liberamos SHIFT
					}
					keystrokes_supr = 0;
					PressKey(KEY_ENTER, 0, scancodeset);
					rshift = 0;
					FreeKBBuffer();

				}


			}
			else
			{
				if (p1.up != p1prev.up) sendCodeMR(P1KeyMap[0], !p1.up, 0, scancodeset);
				if (p1.down != p1prev.down) sendCodeMR(P1KeyMap[1], !p1.down, 0, scancodeset);
				if (p1.left != p1prev.left) sendCodeMR(P1KeyMap[2], !p1.left, 0, scancodeset);
				if (p1.right != p1prev.right) sendCodeMR(P1KeyMap[3], !p1.right, 0, scancodeset);

				if ((p1.select != p1prev.select) && !p1.select) PressKey(P1KeyMap[4], 200, scancodeset);
				if ((p1.start != p1prev.start) && !p1.start) PressKey(P1KeyMap[5], 200, scancodeset);				

				if (p1.button1 != p1prev.button1) sendCodeMR(P1KeyMap[6], !p1.button1, 0, scancodeset);
				if (p1.button2 != p1prev.button2) sendCodeMR(P1KeyMap[7], !p1.button2, 0, scancodeset);
				if (p1.button3 != p1prev.button3) sendCodeMR(P1KeyMap[8], !p1.button3, 0, scancodeset);
				if (p1.button4 != p1prev.button4) sendCodeMR(P1KeyMap[9], !p1.button4, 0, scancodeset);
				if (p1.button5 != p1prev.button5) sendCodeMR(P1KeyMap[10], !p1.button5, 0, scancodeset);
				if (p1.button6 != p1prev.button6) sendCodeMR(P1KeyMap[11], !p1.button6, 0, scancodeset);
			}
		}

		if (p2.up != p2prev.up) sendCodeMR(P2KeyMap[0], !p2.up, 0, scancodeset);
		if (p2.down != p2prev.down) sendCodeMR(P2KeyMap[1], !p2.down, 0, scancodeset);
		if (p2.left != p2prev.left) sendCodeMR(P2KeyMap[2], !p2.left, 0, scancodeset);
		if (p2.right != p2prev.right) sendCodeMR(P2KeyMap[3], !p2.right, 0, scancodeset);

		if ((p2.select != p2prev.select) && !p2.select) PressKey(P2KeyMap[4], 200, scancodeset);
		if ((p2.start != p2prev.start) && !p2.start) PressKey(P2KeyMap[5], 200, scancodeset);

		if (p2.button1 != p2prev.button1) sendCodeMR(P2KeyMap[6], !p2.button1, 0, scancodeset);
		if (p2.button2 != p2prev.button2) sendCodeMR(P2KeyMap[7], !p2.button2, 0, scancodeset);
		if (p2.button3 != p2prev.button3) sendCodeMR(P2KeyMap[8], !p2.button3, 0, scancodeset);
		if (p2.button4 != p2prev.button4) sendCodeMR(P2KeyMap[9], !p2.button4, 0, scancodeset);
		if (p2.button5 != p2prev.button5) sendCodeMR(P2KeyMap[10], !p2.button5, 0, scancodeset);
		if (p2.button6 != p2prev.button6) sendCodeMR(P2KeyMap[11], !p2.button6, 0, scancodeset);
	}
}

