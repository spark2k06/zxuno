// Fichero de configuracion del firmware de Sugarless +2

#define membrane13x11 // Desactivado para membranas de 5x8 si se quiere ahorrar tiempo de proceso del avr

#define atmega2560 // Activado para Arduino Mega
//#define atmega128 // Activado para Atmega128

#define PA 1 //
#define PB 2 //
#define PC 3 //
#define PD 4 //
#define PE 5 //
#define PG 7 //
#define PH 8 //
#define PL 12 //

#define ROWS8 8		//Numero de Filas de teclado 8x5
#define COLS5 5		//Numero de Columnas de teclado 8x5

#define ROWS 13		//Numero de Filas de teclado 13x11
#define COLS 11		//Numero de Columnas de teclado 13x11

// Mapa de la matriz
// 1x1 = '1', 1x2 = '2'
// 2x1 = 'Q', 2x2 = 'W'
// ...

#ifdef atmega128

#define LED_ON				PORTA |= (1<<4)   
#define LED_OFF				PORTA &= ~(1<<4)
#define LED_CONFIG			DDRA |= (1<<4)

#define KBEXT_BIDIR_ON		PORTF |= (1<<1)   
#define KBEXT_BIDIR_OFF		PORTF &= ~(1<<1)
#define KBEXT_BIDIR_CONFIG	DDRF |= (1<<1)

#define POWER_ON			PORTA |= (1<<7)   
#define POWER_OFF			PORTA &= ~(1<<7)
#define POWER_CONFIG		DDRA |= (1<<7)

#define RESET_ON			DDRA |= (1<<5); PORTA &= ~(1<<5) // GND
#define RESET_OFF			DDRA &= ~(1<<5); PORTA |= (1<<5) // HI-Z

#define SYSTEM_PIN			6
#define SYSTEM_BCD			PA

#define PS2_DAT				PF3
#define PS2_CLK				PF2

#define PS2_PORT			PORTF
#define PS2_DDR				DDRF
#define PS2_PIN				PINF

//{PC3, PC1, PG1, PD7, PD5, PD3, PD1, PG4, PB7, PB5, PB3};
uint8_t pinsC[COLS] = { 3,  1,  1,  7,  5,  3,  1,  4,  7,  5,  3 };
uint8_t bcdC[COLS] = { PC, PC, PG, PD, PD, PD, PD, PG, PB, PB, PB };

//{PB4, PB6, PG3, PD0, PD2, PD4, PD6, PG0, PC0, PC2, PC4, PC6, PC7};
uint8_t pinsR[ROWS] = { 4,  6,  3,  0,  2,  4,  6,  0,  0,  2,  4,  6,  7 };
uint8_t bcdR[ROWS] = { PB, PB, PG, PD, PD, PD, PD, PG, PC, PC, PC, PC, PC };

//{PE5(1), PE4(2), PE3(3), PE2(4), PE1(6), PE0(7), PC5(9)};
uint8_t pinsJOY2PS2[7] = { 5, 4, 3, 2, 1, 0, 5 };
uint8_t bcdJOY2PS2[7] = { PE, PE, PE, PE, PE, PC};

#define	FAM_DAT_PIN			PINE
#define	FAM_DAT_DDR			DDRE
#define	FAM_LAT_DDR			DDRE
#define	FAM_CLK_DDR			DDRE
#define	FAM_DAT_PORT		PORTE
#define	FAM_LAT_PORT		PORTE
#define	FAM_CLK_PORT		PORTE

uint8_t TZX_ROOT_PIN = 2, TZX_ROOT_BCD = PB;
uint8_t TZX_UP_PIN = 1, TZX_UP_BCD = PB;
uint8_t TZX_DOWN_PIN = 0, TZX_DOWN_BCD = PB;
uint8_t TZX_PLAY_PIN = 7, TZX_PLAY_BCD = PE;
uint8_t TZX_STOP_PIN = 6, TZX_STOP_BCD = PE;

#endif

#ifdef atmega2560

#define LED_ON				PORTL |= (1<<0)   
#define LED_OFF				PORTL &= ~(1<<0)
#define LED_CONFIG			DDRB |= (1<<7)

#define KBEXT_BIDIR_ON      PORTB |= (1<<3)   
#define KBEXT_BIDIR_OFF     PORTB &= ~(1<<3)
#define KBEXT_BIDIR_CONFIG  DDRB |= (1<<3)

#define POWER_ON			PORTB |= (1<<2)   
#define POWER_OFF			PORTB &= ~(1<<2)
#define POWER_CONFIG		DDRB |= (1<<2)

#define RESET_ON			DDRB |= (1<<0); PORTB &= ~(1<<0) // GND
#define RESET_OFF			DDRB &= ~(1<<0); PORTB |= (1<<0) // HI-Z

#define SYSTEM_PIN			1
#define SYSTEM_BCD			PB

#define PS2_DAT				PF3
#define PS2_CLK				PF2

#define PS2_PORT			PORTF
#define PS2_DDR				DDRF
#define PS2_PIN				PINF

//{PL7, PG1, PD7, PC1, PC3, PC5, PC7, PA6, PA4, PA2, PA0};
uint8_t pinsC[COLS] = { 7,  1,  7,  1,  3,  5,  7,  6,  4,  2,  0 };
uint8_t bcdC[COLS] = { PL, PG, PD, PC, PC, PC, PC, PA, PA, PA, PA };

//{PA1, PA3, PA5, PA7, PC6, PC4, PC2, PC0, PG2, PG0, PL6, PL4, PL2};
uint8_t pinsR[ROWS] = { 1,  3,  5,  7,  6,  4,  2,  0,  2,  0,  6,  4,  2 };
uint8_t bcdR[ROWS] = { PA, PA, PA, PA, PC, PC, PC, PC, PG, PG, PL, PL, PL };

//{PH4(1), PH5(2), PH6(3), PB4(4), PB5(6), PB6(7), PL5(9)};
uint8_t pinsJOY2PS2[7] = { 4, 5, 6, 4, 5, 6, 5 };
uint8_t bcdJOY2PS2[7] = { PH, PH, PH, PB, PB, PB, PL };

#define	FAM_DAT_PIN			PINH
#define	FAM_DAT_DDR			DDRH
#define	FAM_LAT_DDR			DDRH
#define	FAM_CLK_DDR			DDRB
#define	FAM_DAT_PORT		PORTH
#define	FAM_LAT_PORT		PORTH
#define	FAM_CLK_PORT		PORTB

uint8_t TZX_ROOT_PIN = 4, TZX_ROOT_BCD = PE;
uint8_t TZX_UP_PIN = 5, TZX_UP_BCD = PE;
uint8_t TZX_DOWN_PIN = 5, TZX_DOWN_BCD = PG;
uint8_t TZX_PLAY_PIN = 3, TZX_PLAY_BCD = PE;
uint8_t TZX_STOP_PIN = 3, TZX_STOP_BCD = PH;

#endif

// Famicom DB9 comun
#define	FAM_DAT	(1<<pinsJOY2PS2[1])
#define	FAM_LAT	(1<<pinsJOY2PS2[2])
#define	FAM_CLK	(1<<pinsJOY2PS2[3])

#define FAMDELAY	24 //12
