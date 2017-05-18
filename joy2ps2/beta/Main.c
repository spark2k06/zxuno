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

static	report_t		p1, p1prev;
static	report_t		p2, p2prev;
static  uchar			p1selectnesclon, p1startnesclon;
static  uchar			p2selectnesclon, p2startnesclon;
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


int main()
{	
	uchar shiftmode = 0;
	int menuoption = -1, resetoption = -1, combioption = -1;
	int escape = 0;
	// Setup		
	CPU_PRESCALE(0);
	ps2Init();
	QueuePS2Init();
	mapper = 0; // Mapa por defecto al inicio
	SetMap(0);

	p1selectnesclon = 0;
	p1startnesclon = 0;
	p2selectnesclon = 0;
	p2startnesclon = 0;

	Cursors(); // Direcciones del joystick como cursores y ENTER por defecto en el inicio
	
	/*	
	_delay_ms(500);
	PressKey(KEY_F2, 200);
	PressKey(KEY_DOWN, 200);
	PressKey(KEY_DOWN, 200);
	PressKey(KEY_DOWN, 200);
	PressKey(KEY_ENTER, 0);	
	*/
	
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
		p1prev.up = p1.up; p2prev.up = p2.up; p1.up = 0; p2.up = 0;
		p1prev.down = p1.down; p2prev.down = p2.down; p1.down = 0; p2.down = 0;
		p1prev.left = p1.left; p2prev.left = p2.left; p1.left = 0; p2.left = 0;
		p1prev.right = p1.right; p2prev.right = p2.right; p1.right = 0; p2.right = 0;
		p1prev.select = p1.select; p2prev.select = p2.select; p1.select = 0; p2.select = 0;
		p1prev.start = p1.start; p2prev.start = p2.start; p1.start = 0; p2.start = 0;
		p1prev.button1 = p1.button1; p2prev.button1 = p2.button1; p1.button1 = 0; p2.button1 = 0;
		p1prev.button2 = p1.button2; p2prev.button2 = p2.button2; p1.button2 = 0; p2.button2 = 0;
		p1prev.button3 = p1.button3; p2prev.button3 = p2.button3; p1.button3 = 0; p2.button3 = 0;
		p1prev.button4 = p1.button4; p2prev.button4 = p2.button4; p1.button4 = 0; p2.button4 = 0;
		p1prev.button5 = p1.button5; p2prev.button5 = p2.button5; p1.button5 = 0; p2.button5 = 0;
		p1prev.button6 = p1.button6; p2prev.button6 = p2.button6; p1.button6 = 0; p2.button6 = 0;
		p1prev.keymapper = p1.keymapper;

		_delay_ms(10); // Para evitar efecto rebote en la lectura de pulsaciones de los gamepads
		
		if (CheckDB15())
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
		
			while (p1.select || p1.start || p1.button1 || p1.up || p1.down || p1.left || p1.right)
			{				
				if (CheckDB15())
				{
					ReadDB15(&p1);
				}
				else
				{
					ReadDB9P1(&p1);
					CheckP1SelectStartNesClon();
				}
				continue;
			}

			// Reset counters
			menuoption = -1;
			resetoption = -1;
			combioption = -1;
			escape = 0;
	
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
				escape = 0;
				
			}

			
			// Down -> add resetoption counter
			if (p1prev.down & !p1.down)
			{
				resetoption += (int)(resetoption < 4);
				combioption = -1;
				menuoption = -1;
				escape = 0;
			}
			
			// Right -> add combioption counter
			if (p1prev.right & !p1.right)
			{
				combioption += (int)(combioption < 2);
				menuoption = -1;
				resetoption = -1;
				escape = 0;
			}

			// Left -> Escape for one button joysticks
			if (p1prev.left & !p1.left)
			{
				escape = 1;
				menuoption = -1;
				resetoption = -1;
				combioption = -1;
			}
			
			if (p1prev.button1 & !p1.button1) // Final combo
			{				
				shiftmode = 0;	
				p1.button1 = 0;
				p1prev.button1 = 0;

				// ChangeKeys
				if (menuoption == -1 && resetoption == -1 && combioption == -1)
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
				// Right counts -> Combination options: NMI, LOAD128, LOAD48
				if (escape)
				{
					PressKey(KEY_ESCAPE, 0);
					continue;
				}

			}

		}				
		else
		{

			if (p1.up != p1prev.up) sendCodeMR(KeyMap[0], !p1.up, 0);
			if (p1.down != p1prev.down) sendCodeMR(KeyMap[1], !p1.down, 0);
			if (p1.left != p1prev.left) sendCodeMR(KeyMap[2], !p1.left, 0);
			if (p1.right != p1prev.right) sendCodeMR(KeyMap[3], !p1.right, 0);

			if (p1prev.select & !p1.select) PressKey(KeyMap[4], 200);
			if (p1prev.start & !p1.start) PressKey(KeyMap[5], 200);

			if (p1.button1 != p1prev.button1) sendCodeMR(KeyMap[6], !p1.button1, 0);
			if (p1.button2 != p1prev.button2) sendCodeMR(KeyMap[7], !p1.button2, 0);
			if (p1.button3 != p1prev.button3) sendCodeMR(KeyMap[8], !p1.button3, 0);
			if (p1.button4 != p1prev.button4) sendCodeMR(KeyMap[9], !p1.button4, 0);
			if (p1.button5 != p1prev.button5) sendCodeMR(KeyMap[10], !p1.button5, 0);
			if (p1.button6 != p1prev.button6) sendCodeMR(KeyMap[11], !p1.button6, 0);
		}

		if (p2.up != p2prev.up) sendCodeMR(KEY_I, !p2.up, 0);
		if (p2.down != p2prev.down) sendCodeMR(KEY_K, !p2.down, 0);
		if (p2.left != p2prev.left) sendCodeMR(KEY_J, !p2.left, 0);
		if (p2.right != p2prev.right) sendCodeMR(KEY_L, !p2.right, 0);

		if ((p2.select != p2prev.select) & !p2.select) PressKey(KEY_6, 200);
		if ((p2.start != p2prev.start) & !p2.start) PressKey(KEY_2, 200);

		if (p2.button1 != p2prev.button1) sendCodeMR(KEY_A, !p2.button1, 0);
		if (p2.button2 != p2prev.button2) sendCodeMR(KEY_S, !p2.button2, 0);
		if (p2.button3 != p2prev.button3) sendCodeMR(KEY_D, !p2.button3, 0);
		if (p2.button4 != p2prev.button4) sendCodeMR(KEY_F, !p2.button4, 0);
		if (p2.button5 != p2prev.button5) sendCodeMR(KEY_G, !p2.button5, 0);
		if (p2.button6 != p2prev.button6) sendCodeMR(KEY_H, !p2.button6, 0);
						
	}
}

