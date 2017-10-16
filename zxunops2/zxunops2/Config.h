// Fichero de configuracion

//#define switches // Activado para el ZXGo+
//#define atmega644 // Activado para teclado +2A/+3 con atmega644

#define PA 1 //
#define PB 2 //
#define PC 3 //
#define PD 4 //

#define ROWS8 8		//Numero de Filas de teclado 8x5
#define COLS5 5		//Numero de Columnas de teclado 8x5

// Mapa de la matriz
// 1x1 = '1', 1x2 = '2'
// 2x1 = 'Q', 2x2 = 'W'
// ...

#ifdef atmega644

#define ROWS 13		//Numero de Filas de teclado en atmega 644
#define COLS 11		//Numero de Columnas de teclado en atmega 644

#define PS2_DAT		PA7
#define PS2_CLK		PA6

#define PS2_PORT	PORTA
#define PS2_DDR		DDRA
#define PS2_PIN		PINA

//{PA4, PA3, PA2, PA1, PA0, PB0, PB1, PB2, PB3, PB4, PB5};
uint8_t pinsC[COLS] = { 4,  3,  2,  1,  0,  0,  1,  2,  3,  4,  5 };
uint8_t bcdC[COLS] = { PA, PA, PA, PA, PA, PB, PB, PB, PB, PB, PB };

//{PC7, PC6, PC5, PC4, PC3, PC2, PC1, PC0, PD4, PD3, PD2, PD1, PD0};
uint8_t pinsR[ROWS] = { 7,  6,  5,  4,  3,  2,  1,  0,  4,  3,  2,  1,  0 };
uint8_t bcdR[ROWS] = { PC, PC, PC, PC, PC, PC, PC, PC, PD, PD, PD, PD, PD };

#else

#define ROWS 8		//Numero de Filas de teclado en atmega 168/328
#define COLS 5		//Numero de Columnas de teclado en atmega 168/328

#define PS2_DAT		PC4
#define PS2_CLK		PC5

////////Pro Mini
#define PS2_PORT	PORTC
#define PS2_DDR		DDRC
#define PS2_PIN		PINC

#define LED_ON	PORTB |= (1<<5)		
#define LED_OFF	PORTB &= ~(1<<5)
#define LED_CONFIG	DDRB |= (1<<5) //Led en PB5 en Pro mini y similares

//{PC1, PC0, PB4, PB3, PB2};
uint8_t pinsC[COLS] = {  1,  0,  4,  3,  2 }; // Configuracion de pines en el ZXGo+
uint8_t bcdC[COLS] =  { PC, PC, PB, PB, PB }; // Configuracion de pines en el ZXGo+

//{PD2, PD3, PD4, PD5, PD6, PD7, PB0, PB1};
uint8_t pinsR[ROWS] = {  2,  3,  4,  5,  6,  7,  0,  1 }; // Configuracion de pines en el ZXGo+
uint8_t bcdR[ROWS] =  { PD, PD, PD, PD, PD, PD, PB, PB }; // Configuracion de pines en el ZXGo+

#endif

#ifdef switches
//Swiches {PC2=A2 / PC3=A3}; 
#define NUMSW 2 //Numero de botones externos en Arduino
uint8_t pinsSW[NUMSW] = { 2, 3 };
uint8_t bcdSW[NUMSW] = { 3, 3 };
#endif
