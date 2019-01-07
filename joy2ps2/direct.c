#include <avr/io.h>
#include <util/delay.h>
#include "report.h"
#include "direct.h"
#include "keymaps.h"

void ReadDB9P1(report_t *p1)
{	
	// Get D-PAD, B, C buttons state
	PORTC |= (1 << 0);
	_delay_us(14);
	
	p1->up = CHECKUP;					  // Up
	p1->down = CHECKDOWN;				  // Down
	p1->left = !(DB15_PIN01 & (1 << 2));  // Left	
	p1->right = !(DB15_PIN01 & (1 << 3)); // Right
	
	PORTC &= ~(1 << 0);
	_delay_us(14);
	if (!(DB15_PIN01 & (1 << 2)) & !(DB15_PIN01 & (1 << 3)))	// if L/R low then Megadrive pad detected
	{

		p1->button1 = !(DB15_PIN01 & (1 << 4));		// Button A
		p1->start = !(DB15_PIN01 & (1 << 5));		// Start

		PORTC |= (1 << 0);
		_delay_us(14);
		p1->button2 = !(DB15_PIN01 & (1 << 4));	// Button B
		p1->button3 = !(DB15_PIN01 & (1 << 5));	// Button C
		PORTC &= ~(1 << 0);
		_delay_us(14);

		PORTC |= (1 << 0);					// select high
		_delay_us(14);
		PORTC &= ~(1 << 0);					// select low
		_delay_us(14);

		// U/D/L/R are low if six button controller
		if (CHECKUP & CHECKDOWN & !(DB15_PIN01 & (1 << 2)) & !(DB15_PIN01 & (1 << 3)))
		{
			PORTC |= (1 << 0);						// select high
			_delay_us(14);

			p1->button6 = CHECKUP;					// Button Z
			p1->button5 = CHECKDOWN;				// Button Y
			p1->button4 = !(DB15_PIN01 & (1 << 2));	// Button X
			p1->select = !(DB15_PIN01 & (1 << 3));	// Select (Mode)

			PORTC &= ~(1 << 0);						// select low
			_delay_us(14);
			PORTC |= (1 << 0);						// select high
			_delay_us(14);
			PORTC &= ~(1 << 0);						// select low

		}

		// Delay needed for settling joystick down
		_delay_us(2000);

	}
	else
	{
		p1->button1 = !(DB15_PIN01 & (1 << 4));	// Button A
		p1->button2 = !(DB15_PIN01 & (1 << 5));	// Button B
	}
	PORTC |= (1 << 0);
	_delay_us(14);
	
}

void ReadDB9P2(report_t *p2)
{
	// Get P2 D-PAD, B, C buttons state
	PORTC |= (1 << 1);
	_delay_us(14);		

	p2->up = !(DB15_PIN01 & (1 << 6));		// Up P2
	p2->down = !(DB15_PIN02 & (1 << 3));	// Down P2
	p2->left = !(DB15_PIN01 & (1 << 7));	// Left P2
	p2->right = !(DB15_PIN02 & (1 << 2));	// Right P2

	PORTC &= ~(1 << 1);
	_delay_us(14);
	if (!(DB15_PIN01 & (1 << 7)) & !(DB15_PIN02 & (1 << 2)))	// if L/R low then Megadrive pad P2 detected
	{

		p2->button1 = !(DB15_PIN02 & (1 << 1));		// Button A P2
		p2->start = !(DB15_PIN02 & (1 << 0));		// Start P2

		PORTC |= (1 << 1);
		_delay_us(14);
		p2->button2 = !(DB15_PIN02 & (1 << 1));	// Button B P2
		p2->button3 = !(DB15_PIN02 & (1 << 0));	// Button C P2
		PORTC &= ~(1 << 1);
		_delay_us(14);

		PORTC |= (1 << 1);					// select P2 high
		_delay_us(14);
		PORTC &= ~(1 << 1);					// select P2 low
		_delay_us(14);

		// U/D/L/R are low if six button controller
		if (!(DB15_PIN01 & (1 << 6)) & !(DB15_PIN02 & (1 << 3)) & !(DB15_PIN01 & (1 << 7)) & !(DB15_PIN02 & (1 << 2)))
		{
			PORTC |= (1 << 1);						// select P2 high
			_delay_us(14);

			p2->button6 = !(DB15_PIN01 & (1 << 6));	// Button Z P2
			p2->button5 = !(DB15_PIN02 & (1 << 3));	// Button Y P2
			p2->button4 = !(DB15_PIN01 & (1 << 7));	// Button X P2
			p2->select = !(DB15_PIN02 & (1 << 2));	// Select (Mode) P2

			PORTC &= ~(1 << 1);						// select P2 low
			_delay_us(14);
			PORTC |= (1 << 1);						// select P2 high
			_delay_us(14);
			PORTC &= ~(1 << 1);						// select P2 low
			
		}

		// Delay needed for settling joystick down
		_delay_us(2000);

	}
	else
	{
		p2->button1 = !(DB15_PIN02 & (1 << 1));	// Button A
		p2->button2 = !(DB15_PIN02 & (1 << 0));	// Button B
	}
	PORTC |= (1 << 1);
	_delay_us(14);


}

/* --- NeoGeo controller ------------------------------------------------------------------- */

void ReadDB15(report_t *p1) // Solo en modo 1 jugador, ya que el jugador 2 comparte distintos eventos con el db9p2
{	
	
	p1->button3 = p1->button3 || (!(DB15_PIN01 & (1 << 6)));		// Button 3
	p1->button4 = p1->button4 || (!(DB15_PIN01 & (1 << 7)));		// Button 4
	p1->start = p1->start || (!(DB15_PIN02 & (1 << 0)));			// Start
	p1->select = p1->select || (!(DB15_PIN02 & (1 << 1)));			// Select	
	p1->button5 = p1->button5 || (!(DB15_PIN02 & (1 << 2)));		// Button 5
	p1->button6 = p1->button6 || (!(DB15_PIN02 & (1 << 3)));		// Button 6
	

}