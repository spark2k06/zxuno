// Fichero de configuracion

#define switches // Activado para el ZXGo+

#define PS2_DAT		PC4
#define PS2_CLK		PC5

////////Pro Mini
#define PS2_PORT	PORTC
#define PS2_DDR		DDRC
#define PS2_PIN		PINC

#define LED_ON	PORTB |= (1<<5)		
#define LED_OFF	PORTB &= ~(1<<5)
#define LED_CONFIG	DDRB |= (1<<5) //Led en PB5 en Pro mini y similares

//Definicion de pines y puertos en arrays

///////////Pro Mini
//{PB2=10, PB3=11, PB4=12, PC0=A0, PC1=A1};
uint8_t pinsC[COLS] = { 2, 3, 4, 0, 1 }; // Configuracion de pines en el ZXGo+
uint8_t bcdC[COLS] = { 2, 2, 2, 3, 3 }; // Configuracion de pines en el ZXGo+

//{PD2=2, PD3=3, PD4=4, PD5=5, PD6=6, PD7=7, PB0=8, PB1=9};
uint8_t pinsR[ROWS] = { 2, 3, 4, 5, 6, 7, 0, 1 }; // Configuracion de pines en el ZXGo+
uint8_t bcdR[ROWS] = { 4, 4, 4, 4, 4, 4, 2, 2 }; // Configuracion de pines en el ZXGo+

#ifdef switches
//Swiches {PC2=A2 / PC3=A3}; 
uint8_t pinsSW[NUMSW] = { 2, 3 };
uint8_t bcdSW[NUMSW] = { 3, 3 };
#endif