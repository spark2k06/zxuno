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
#include "PS2Keyboard.h"
#include "report.h"
#include "direct.h"

const unsigned char MenuOptions[] = { KEY_R, KEY_0, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9 };
const unsigned char Keys[] = { KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, KEY_J, KEY_K, KEY_L,
                               KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X,
                               KEY_Y, KEY_Z, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0 };

static	report_t		p1, p1prev;
static	report_t		p2, p2prev;
static  uchar			p1selectnesclon, p1startnesclon;
static  uchar			p2selectnesclon, p2startnesclon;
static  int				keystrokes_supr;
unsigned char			db15;
//millis_t				milisecs;

void CheckP1SelectStartNesClon()
{
	if (p1.up & p1.down)
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
		p1.up = p1.up & !p1prev.select;
		p1.down = p1.down & !p1prev.select;
	}

	if (p1.left & p1.right)
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
		p1.left = p1.left & !p1prev.start;
		p1.right = p1.right & !p1prev.start;
	}
}

void CheckP2SelectStartNesClon()
{
	if (p2.up & p2.down)
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
		p2.up = p2.up & !p2prev.select;
		p2.down = p2.down & !p2prev.select;
	}

	if (p2.left & p2.right)
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
		p2.left = p2.left & !p2prev.start;
		p2.right = p2.right & !p2prev.start;
	}
}

void keystroke_press(unsigned char key)
{
	if (!keystrokes_supr)
	{
		PressKey(key, 0);
		keystrokes_supr = 1;
	}
	else
	{
		PressKey(KEY_DELETE, 0);
		PressKey(key, 0);	
	}

}

int main()
{	
	uchar shiftmode = 0;
	int menuoption = -1, resetoption = -1, combioption = -1, extraoptions = -1;
	int keystrokes_idx = 35;
	int rshift = 0;
	db15 = 1;
	p1map = 0, p2map = 0;
	keystrokes_supr = 0;
	
	// Setup		
	CPU_PRESCALE(0);
	ps2Init();
	QueuePS2Init();	
	SetMapP1(p1map);
	SetMapP2(p2map);

	p1selectnesclon = 0;
	p1startnesclon = 0;
	p2selectnesclon = 0;
	p2startnesclon = 0;

	Cursors(); // Direcciones del joystick como cursores y ENTER por defecto en el inicio

	// Entrada desde DB15, activamos resistencias internas pullup en pines digitales 0 a 12
	DDRC &= ~(1 << 4);	//set UP_BUTTON		as input -> Analogic Pin 4 <-> Digital 0
	DDRC &= ~(1 << 5);	//set DOWN_BUTTON	as input -> Analogic Pin 5 <-> Digital 1
	PORTC |= (1 << 4);	//set UP_BUTTON 	pull up on -> Analogic Pin 4 <-> Digital 0
	PORTC |= (1 << 5);	//set DOWN_BUTTON 	pull up on -> Analogic Pin 5 <-> Digital 1
	
	DB15_PIN01 = 0b11111111; // Ponemos en alto pines 0 - 7
	DB15_PIN02 = DB15_PIN02 | 0b00011111; // Ponemos en alto los pines 8 - 12, respetamos el contenido del resto ya que nos los utilzaremos
	DB15_PORT01 = 0; // Input pullup 2 - 7 
	DB15_PORT02 = DB15_PORT02 & 0b11100000; // Input pullup 8 - 12

	DDRC |= (1 << 0);		// Select as output (Pin A0 -> Player 1)
	DDRC |= (1 << 1);		// Select as output (Pin A1 -> Player 2)
	
	PORTC |= (1 << 0);					// Select Player 1 high
	PORTC |= (1 << 1);					// Select Player 2 high
	// Loop

	while (1) {		
		
		FreeKBBuffer();
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
			ReadDB9P1(&p1);
			// Player 2
			ReadDB9P2(&p2);

			CheckP2SelectStartNesClon();
		}

		CheckP1SelectStartNesClon();
						
		if 
		(		
			(p1.start & p1.button1) ||				// Start + Button 1 (SHIFT MODE)
			(p1.select & p1.start)	||				// Select + Start (SHIFT MODE)
			(p1prev.keymapper & !p1.keymapper)      // Keymapper (SHIFT MODE)	
		)
					
		{			
			
			shiftmode = !shiftmode;	
			if (!shiftmode)
			{
				p1prev.select = 0; p1prev.start = 0; p1prev.button1 = 0;
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
					ReadDB9P1(&p1);
					// Player 2
					ReadDB9P2(&p2);

					CheckP2SelectStartNesClon();
				}

				CheckP1SelectStartNesClon();
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

			// Up -> add menuoption counter
			if (p1prev.up & !p1.up)
			{
				menuoption += (int)(menuoption < 10);
				combioption = -1;
				resetoption = -1;
				extraoptions = -1;
				
			}

			
			// Down -> add resetoption counter
			if (p1prev.down & !p1.down)
			{
				resetoption += (int)(resetoption < 4);
				combioption = -1;
				menuoption = -1;
				extraoptions = -1;
			}
			
			// Right -> add combioption counter
			if (p1prev.right & !p1.right)
			{
				combioption += (int)(combioption < 2);
				menuoption = -1;
				resetoption = -1;
				extraoptions = -1;
			}

			// Left -> Extraoptions
			if (p1prev.left & !p1.left)
			{
				extraoptions += (int)(extraoptions < 3);
				menuoption = -1;
				resetoption = -1;
				combioption = -1;
			}

			if (p2prev.button1 & !p2.button1) // Final combo P2
			{
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
				shiftmode = 0;
				p1.button1 = 0; p1.button2 = 0;
				p1prev.button1 = 0; p1prev.button2 = 0;
				db15 = !db15;
			}
			
			if (p1prev.button1 & !p1.button1) // Final combo
			{				
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
						NMI();
					}
					if (combioption == 1)
					{
						LOAD128();
					}
					if (combioption == 2)
					{
						LOAD48();
					}
					continue;
				}

				// Up counts -> Menu options: R, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
				if (menuoption >= 0)
				{					
					PressKey(MenuOptions[(int)menuoption], 200);
					continue;
				}

				// Down counts -> Reset options: Reset, MasterReset, ROMs, cores, BIOS
				if (resetoption >= 0)
				{
					p1map = 0, p2map = 0;
					SetMapP1(p1map);
					SetMapP2(p2map);

					if (resetoption == 0)
					{
						Reset();
					}
					if (resetoption == 1)
					{
						MasterReset(0);
					}
					if (resetoption == 2)
					{
						MasterReset(1);
					}
					if (resetoption == 3)
					{
						MasterReset(2);
					}
					if (resetoption == 4)
					{
						MasterReset(3);
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
						PressKey(KEY_ESCAPE, 0);						
					}
					if (extraoptions == 2) // Keymap P1
					{
						if (p1map < 3) p1map++; else p1map = 0;
						SetMapP1(p1map);
					}
					if (extraoptions == 3) // Cambio de Video (Scroll / Lock)
					{
						PressKey(KEY_SCROLL, 0);
					}

				}

			}

		}				
		else
		{
			
			if (extraoptions == 0) // modo KEYSTROKES
			{
				
				if (p1prev.button1 & !p1.button1 && keystrokes_supr) // Button 1 -> Hará de KEY_RSHIFT para acceder a mayusculas y a caractéres especiales.									
				{			
					rshift = !rshift;
					PressKey(KEY_DELETE, 0);					
					if (rshift)
					{
						sendCodeMR(KEY_RSHIFT, 0, 0);
						PressKey(Keys[keystrokes_idx], 0);
						sendCodeMR(KEY_RSHIFT, 1, 0);
					}
					else
					{
						PressKey(Keys[keystrokes_idx], 0);
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
						sendCodeMR(KEY_RSHIFT, 1, 0); // Liberamos SHIFT
					}
					if (keystrokes_supr)
					{						
						keystrokes_supr = 0;
					}
					else
					{
						PressKey(KEY_SPACE, 0);
					}
					rshift = 0;
					FreeKBBuffer();
				}

				if (p1.left && !p1prev.left && !p1.button1)
				{
					if (p1.button1)
					{
						sendCodeMR(KEY_RSHIFT, 1, 0); // Liberamos SHIFT
					}
					keystrokes_supr = 0;
					PressKey(KEY_DELETE, 0);
					rshift = 0;
					FreeKBBuffer();
				}

				if (p1.button2 && !p1prev.button2 && !p1.button1)
				{
					if (p1.button1)
					{
						sendCodeMR(KEY_RSHIFT, 1, 0); // Liberamos SHIFT
					}
					keystrokes_supr = 0;
					PressKey(KEY_ENTER, 0);					
					rshift = 0;
					FreeKBBuffer();

				}
								

			}
			else
			{
				if (p1.up != p1prev.up) sendCodeMR(P1KeyMap[0], !p1.up, 0);
				if (p1.down != p1prev.down) sendCodeMR(P1KeyMap[1], !p1.down, 0);
				if (p1.left != p1prev.left) sendCodeMR(P1KeyMap[2], !p1.left, 0);
				if (p1.right != p1prev.right) sendCodeMR(P1KeyMap[3], !p1.right, 0);

				if (p1prev.select & !p1.select) PressKey(P1KeyMap[4], 200);
				if (p1prev.start & !p1.start) PressKey(P1KeyMap[5], 200);

				if (p1.button1 != p1prev.button1) sendCodeMR(P1KeyMap[6], !p1.button1, 0);
				if (p1.button2 != p1prev.button2) sendCodeMR(P1KeyMap[7], !p1.button2, 0);
				if (p1.button3 != p1prev.button3) sendCodeMR(P1KeyMap[8], !p1.button3, 0);
				if (p1.button4 != p1prev.button4) sendCodeMR(P1KeyMap[9], !p1.button4, 0);
				if (p1.button5 != p1prev.button5) sendCodeMR(P1KeyMap[10], !p1.button5, 0);
				if (p1.button6 != p1prev.button6) sendCodeMR(P1KeyMap[11], !p1.button6, 0);
			}
		}

		if (p2.up != p2prev.up) sendCodeMR(P2KeyMap[0], !p2.up, 0);
		if (p2.down != p2prev.down) sendCodeMR(P2KeyMap[1], !p2.down, 0);
		if (p2.left != p2prev.left) sendCodeMR(P2KeyMap[2], !p2.left, 0);
		if (p2.right != p2prev.right) sendCodeMR(P2KeyMap[3], !p2.right, 0);

		if ((p2.select != p2prev.select) & !p2.select) PressKey(P2KeyMap[4], 200);
		if ((p2.start != p2prev.start) & !p2.start) PressKey(P2KeyMap[5], 200);

		if (p2.button1 != p2prev.button1) sendCodeMR(P2KeyMap[6], !p2.button1, 0);
		if (p2.button2 != p2prev.button2) sendCodeMR(P2KeyMap[7], !p2.button2, 0);
		if (p2.button3 != p2prev.button3) sendCodeMR(P2KeyMap[8], !p2.button3, 0);
		if (p2.button4 != p2prev.button4) sendCodeMR(P2KeyMap[9], !p2.button4, 0);
		if (p2.button5 != p2prev.button5) sendCodeMR(P2KeyMap[10], !p2.button5, 0);
		if (p2.button6 != p2prev.button6) sendCodeMR(P2KeyMap[11], !p2.button6, 0);
						
	}
}

