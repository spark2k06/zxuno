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

static	report_t		p1, p1prev;
static	report_t		p2, p2prev;

int main()
{

	// Setup		
	CPU_PRESCALE(0);
	ps2Init();
	QueuePS2Init();
	mapper = 0; // Mapa por defecto al inicio
	SetMap(0);
	
	if (CheckDB15())
	{
		Cursors(); // Direcciones del joystick como cursores y ENTER por defecto al inicio en DB15
	}
	
	_delay_ms(500);
	PressKey(KEY_F2, 200);
	PressKey(KEY_DOWN, 200);
	PressKey(KEY_DOWN, 200);
	PressKey(KEY_DOWN, 200);
	PressKey(KEY_ENTER, 0);
	
	// Entrada desde DB15, activamos resistencias internas pullup en pines digitales 0 a 12
	DDRC &= ~(1 << PINRX0);	//set UP_BUTTON		as input -> Analogic Pin 4 <-> Digital 0
	DDRC &= ~(1 << PINTX1);	//set DOWN_BUTTON	as input -> Analogic Pin 5 <-> Digital 1
	PORTC |= (1 << PINRX0);	//set UP_BUTTON 	pull up on -> Analogic Pin 4 <-> Digital 0
	PORTC |= (1 << PINTX1);	//set DOWN_BUTTON 	pull up on -> Analogic Pin 5 <-> Digital 1
	
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
		p1prev.select = p1.select; p2prev.select = p2.select;
		p1prev.start = p1.start; p2prev.start = p2.start;
		p1prev.button1 = p1.button1; p2prev.button1 = p2.button1;
		p1prev.button2 = p1.button2; p2prev.button2 = p2.button2;
		p1prev.button3 = p1.button3; p2prev.button3 = p2.button3;
		p1prev.button4 = p1.button4; p2prev.button4 = p2.button4;
		p1prev.button5 = p1.button5; p2prev.button5 = p2.button5;
		p1prev.button6 = p1.button6; p2prev.button6 = p2.button6;
				
		ReadGenesisP1(&p1);
		ReadGenesisP2(&p2);					
		
		// Player 1

		if (p1.up != p1prev.up) sendCodeMR(KEY_UP, !p1.up, 0);
		if (p1.down != p1prev.down) sendCodeMR(KEY_DOWN, !p1.down, 0);
		if (p1.left != p1prev.left) sendCodeMR(KEY_LEFT, !p1.left, 0);
		if (p1.right != p1prev.right) sendCodeMR(KEY_RIGHT, !p1.right, 0);
			
		if ((p1.select != p1prev.select) & !p1.select) PressKey(KEY_5, 200);
		if ((p1.start != p1prev.start) & !p1.start) PressKey(KEY_1, 200);
		
		if (p1.button1 != p1prev.button1) sendCodeMR(KEY_Q, !p1.button1, 0);
		if (p1.button2 != p1prev.button2) sendCodeMR(KEY_W, !p1.button2, 0);
		if (p1.button3 != p1prev.button3) sendCodeMR(KEY_E, !p1.button3, 0);
		if (p1.button4 != p1prev.button4) sendCodeMR(KEY_R, !p1.button4, 0);
		if (p1.button5 != p1prev.button5) sendCodeMR(KEY_T, !p1.button5, 0);
		if (p1.button6 != p1prev.button6) sendCodeMR(KEY_Y, !p1.button6, 0);

		// Player 2

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
