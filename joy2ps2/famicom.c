#include <avr/io.h>
#include <util/delay.h>
#include "report.h"
#include "famicom.h"
#include "Keymaps.h"

/*	D5	Data
	D7	Latch
	B0	Clock

	The Famicom controller uses a TTL 4021 chip, which seems to be quite slow. The
	timings used in this routine are based on the actual timing used by the Famicom.
	Although the real controllers may work with shorter delays, this ensures
	compatibility.

	See protocol document for details of device detection
*/

#define FAMDELAY	24 //12

uchar CheckFamicomP1()
{
	uchar	byte0;

	DDRC &= ~(FAM_DAT_P1);		// Data as input
	PORTC |= FAM_DAT_P1;		// with pull-up

	DDRD |= FAM_LAT_P1;			// Latch as output
	PORTD |= FAM_LAT_P1;		// starting high

	DDRD |= FAM_CLK_P1;			// Clock as output
	PORTD |= FAM_CLK_P1;		// starting high

	_delay_us(FAMDELAY * 2);	// latch pulse
	PORTD &= ~(FAM_LAT_P1);		// latch low again
	_delay_us(FAMDELAY);		// settle time

	byte0 = FamicomReadByteP1();

	DDRD &= ~(FAM_LAT_P1);		// Latch as input
	PORTD |= FAM_LAT_P1;		// with pull-up

	DDRD &= ~(FAM_CLK_P1);		// Clock  as input
	PORTD |= FAM_CLK_P1;		// with pull-up

	return (byte0 & (1 << 2));	// Devolvemos el estado de Select
}

uchar CheckFamicomP2()
{
	uchar	byte0;
	
	DDRB &= ~(FAM_DAT_P2);		// Data as input
	PORTB |= FAM_DAT_P2;		// with pull-up

	DDRD |= FAM_LAT_P2;			// Latch as output
	PORTD |= FAM_LAT_P2;		// starting high

	DDRB |= FAM_CLK_P2;			// Clock as output
	PORTB |= FAM_CLK_P2;		// starting high

	_delay_us(FAMDELAY * 2);	// latch pulse
	PORTD &= ~(FAM_LAT_P2);		// latch low again
	_delay_us(FAMDELAY);		// settle time

	byte0 = FamicomReadByteP2();

	DDRD &= ~(FAM_LAT_P2);		// Latch as input
	PORTD |= FAM_LAT_P2;		// with pull-up

	DDRB &= ~(FAM_CLK_P2);		// Clock  as input
	PORTB |= FAM_CLK_P2;		// with pull-up

	return (byte0 & (1 << 2));	// Devolvemos el estado de Select
	
}

void ReadFamicomP1(report_t *p1)
{		
	uchar	byte0;

	DDRC	&= ~(FAM_DAT_P1);			// Data as input
	PORTC	|= FAM_DAT_P1;				// with pull-up

	DDRD	|= FAM_LAT_P1;				// Latch as output
	PORTD	|= FAM_LAT_P1;				// starting high

	DDRD	|= FAM_CLK_P1;				// Clock as output
	PORTD	|= FAM_CLK_P1;				// starting high

	_delay_us(FAMDELAY * 2);			// latch pulse
	PORTD	&= ~(FAM_LAT_P1);			// latch low again
	_delay_us(FAMDELAY);				// settle time

	byte0 = FamicomReadByteP1();

	p1->button1 = (byte0 & (1 << 0));	// A
	p1->button2 = (byte0 & (1 << 1));	// B
	p1->select = (byte0 & (1 << 2));	// Select
	p1->start = (byte0 & (1 << 3));		// Start
	p1->up = (byte0 & (1 << 4));		// Up
	p1->down = (byte0 & (1 << 5));		// Down
	p1->left = (byte0 & (1 << 6));		// Left
	p1->right = (byte0 & (1 << 7));		// Right
	
	DDRD &= ~(FAM_LAT_P1);				// Latch as input
	PORTD |= FAM_LAT_P1;				// with pull-up

	DDRD &= ~(FAM_CLK_P1);				// Clock  as input
	PORTD |= FAM_CLK_P1;				// with pull-up
	
}

uchar FamicomReadByteP1(void)
{
	uchar i = 0, j;

	for (j = 0; j < 8; j++)
	{
		i = i >> 1;
		if (!(PINC & FAM_DAT_P1)) i |= (1<<7);	// button pressed
		PORTD &= ~(FAM_CLK_P1);					// clock low
		_delay_us(FAMDELAY);
		PORTD |= FAM_CLK_P1;					// clock high
		_delay_us(FAMDELAY);
	}

	return i;
}

void ReadFamicomP2(report_t *p2)
{
	uchar	byte0;

	DDRB &= ~(FAM_DAT_P2);				// Data as input
	PORTB |= FAM_DAT_P2;				// with pull-up

	DDRD |= FAM_LAT_P2;					// Latch as output
	PORTD |= FAM_LAT_P2;				// starting high

	DDRB |= FAM_CLK_P2;					// Clock as output
	PORTB |= FAM_CLK_P2;				// starting high

	_delay_us(FAMDELAY * 2);			// latch pulse
	PORTD &= ~(FAM_LAT_P2);				// latch low again
	_delay_us(FAMDELAY);				// settle time

	byte0 = FamicomReadByteP2();

	p2->button1 = (byte0 & (1 << 0));	// A
	p2->button2 = (byte0 & (1 << 1));	// B
	p2->select = (byte0 & (1 << 2));	// Select
	p2->start = (byte0 & (1 << 3));		// Start
	p2->up = (byte0 & (1 << 4));		// Up
	p2->down = (byte0 & (1 << 5));		// Down
	p2->left = (byte0 & (1 << 6));		// Left
	p2->right = (byte0 & (1 << 7));		// Right

	DDRD &= ~(FAM_LAT_P2);				// Latch as input
	PORTD |= FAM_LAT_P2;				// with pull-up

	DDRB &= ~(FAM_CLK_P2);				// Clock  as input
	PORTB |= FAM_CLK_P2;				// with pull-up

}

uchar FamicomReadByteP2(void)
{
	uchar i = 0, j;

	for (j = 0; j < 8; j++)
	{
		i = i >> 1;
		if (!(PINB & FAM_DAT_P2)) i |= (1 << 7);	// button pressed
		PORTB &= ~(FAM_CLK_P2);					// clock low
		_delay_us(FAMDELAY);
		PORTB |= FAM_CLK_P2;					// clock high
		_delay_us(FAMDELAY);
	}

	return i;
}
