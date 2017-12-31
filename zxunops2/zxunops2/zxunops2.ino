/*
Conversor teclado ZX-spectrum 8x5 -> PS/2 de Neuro (Codigo original de Quest) (Comunicacion bidireccional PS2 y correcciones de protocolo de Spark2k06)

26-09-2017: [Spark2k06]

-> Optimizacion de codigo. Ahora entra en un atmega 168 al liberar memoria dinamica ocupada por los diferentes mapas.

-> Mejora del proceso de interceptaci�n de teclas pulsadas y soltadas por la matriz de teclado, incluyendo la combinacion de CAPS y SYMBOL por cada tecla para facilitar su gestion en modos distintos al de ZXSpectrum. Esta mejora ha permitido eliminar la anterior gestion y simplificarla, evitando asi pausas entre teclas cuando se usan junto con CAPS o SYMBOL.

-> Antighosting de CAPS y SYMBOL en cores distintos al de ZXSpectrum.

-> Posibilidad de ver la version de firmware con CAPS+SYMBOL+V en cualquier modo, incluido el de ZXSpectrum.

-> Nueva funcion para almacenar el estado actual de modo de teclado en la EEPROM del atmega, y este sea cargado por defecto al conectarlo. Muy util si se quiere dejar durante el arranque del ZXUno un core por defecto distinto al del ZXSpectrum. A esta funcion se accede mediante el combo CAPS+SYMBOL+Z.

-> Nuevas funcionalidades para el modo de teclado PCXT:

    -> Funcion tipematica simulada (aunque mejorable mediante interrupciones y tiempos precisos). Necesaria para el core de PCXT y muchos conversores comerciales de PS/2 a USB. Si estos no la necesitan no pasa nada porque sera ignorada por el mismo, ya que los teclados PS/2 convencionales funcionan de esta manera.
    
    -> Deshabilitacion de escucha de comandos una vez inicializado el teclado. Aunque la escucha permanece activa si se estan recibiendo comandos echos (algunos conversores comerciales de PS/2 a USB lo requieren para su correcto funcionamiento). La escucha activa de comandos es especialmente problematica con el uso simultaneo de otro teclado.
    
    -> Si se va a usar como teclado externo, es importante haber guardado previamente en la EEPROM el modo de teclado PCXT, ya que s�lo este modo dispone de escucha activa temporal hasta la inicializacion del mismo.
    
    -> Si se va a utilizar un conversor comercial de PS/2 a USB, es importante que sea de tipo activo, ya que los pasivos solo funcionaran con teclados duales (estos en su firmware son capaces de identificar y controlar tanto PS/2 como USB).

26-09-2017: [/Spark2k06]

(1-10) TODOS           F1-F10
(Q)    TODOS MENOS ZX  F11
(W)    TODOS MENOS ZX  F12
(G)    TODOS (*)       RGB/VGA Swich: (Block Despl)
(B)    TODOS (*)       Hard Reset: (Control+Alt+Backsp)
(N)    ZXUNO           Soft Reset: (Control+Alt+Supr)
(Y)    ZXUNO           NMI: (Control+Alt+F5)
(E)    TODOS MENOS ZX  Re Pag / PGUP
(R)    TODOS MENOS ZX  Av Pag / PGDW
(E)    TODOS MENOS ZX  Re Pag / PGUP + Shift
(R)    TODOS MENOS ZX  Av Pag / PGDW + Shift
(A)    TODOS           F10 para el NEXT
BBC:            F12 RESET / *MENU (Listado de Programas en el BEEB.MMB)
ACORN ELECTRON: F10 RESET / CHAIN"MENU" (Listado de Programas en el BEEB.MMB)
ACORN ATOM: 	F10 RESET / *MENU (Listado de Programas en la SD) / Re Pag: VGA - Av Pag: RGB
ATARI 800: 		F11 Sel Disco & Reset / F12 Menu / Re Pag: VGA - Av Pag: RGB
APPLE II:   	F12 Reset / F1-F10 Selecciona Disco de SD RAW

C64             Re Pag: Retrocede 1 disco / Av Pag: Avanza 1 disco / (Re pag-Av Pag)+Shift: 10 Discos
*/

#include <stdio.h>
#include <avr/io.h>
#include <inttypes.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include "config.h"
#include "keymaps.h"

#define HI 1
#define LO 0
#define _IN 1
#define _OUT 0

#define CPU_PRESCALE(n) (CLKPR = 0x80, CLKPR = (n))
#define CPU_16MHz       0x00

//Teclas Pulsadas en el ultimo pase
// Pulsada              -> 0b00000001 = 0x01
// Mantenida            -> 0b00000010 = 0x02
// Liberada             -> 0b00000100 = 0x04
// CapsShift            -> 0b00001000 = 0x08 (Este flag solo se tiene en cuenta en modos distintos al de ZX)
// SymbolShift          -> 0b00010000 = 0x10 (Este flag solo se tiene en cuenta en modos distintos al de ZX)

uint8_t matriz[ROWS][ROWS]; // Probando a no inicializarlo (por defecto deberia inicializarse con ceros)
uint8_t botones[NUMSW] = { 0,0 };
uint8_t opqa_cursors = 0;
uint8_t fkbmode = 0;

unsigned char espera = 0;
unsigned char fnpulsada = 0;
unsigned char fnpulsando = 0;


//uint8_t modo=0; //Modo teclado 0=ZX NATIVO / 1=CPC MAPEADO /2=MSX MAPEADO
enum KBMODE modo; //Modo teclado 0=ZX=NATIVO /Resto otros mapas
uint8_t cambiomodo = 0;
uint8_t soltarteclas;
uint8_t cs_counter = 0, ss_counter = 0;

unsigned char	hostdata = 0;
unsigned char	hostdataAnt;
unsigned char	codeset = 2;
unsigned char	antighosting = 0;
unsigned char	kbescucha = 0;
uint32_t      timeout_escucha = 0;
uint16_t      typematic = 0;
uint16_t      typematicfirst = 0;
unsigned char typematic_code = 0;
uint8_t       typematic_codeaux = 0;
uint8_t       kbalt = 0;

//Teclas Modificadoras (para teclado spectrum)
unsigned char CAPS_SHIFT = KEY_LSHIFT;  //Caps Shift   (NO necesita E0)
unsigned char SYMBOL_SHIFT = KEY_LCTRL; //Symbol Shift (NO necesita E0)

										//Caps Shift (CAPS_SHIFT)
#define CAPS_SHIFT_ROW 5  
#define CAPS_SHIFT_COL 0  

										//Symbol Shift (SYMBOL_SHIFT)
#define SYMBOL_SHIFT_ROW 7   
#define SYMBOL_SHIFT_COL 1   

										//SPACE (Escape)
#define SPACE_ROW 7 
#define SPACE_COL 0 

										//Row 1..5
#define N1_N5_ROW 0
										//Cols 1..5
#define N1_COL 0 //
#define N2_COL 1 //
#define N3_COL 2 //
#define N4_COL 3 //
#define N5_COL 4 //

										//Row 6..0
#define N6_N0_ROW 3
										//Cols 6..0
#define N6_COL 4 //
#define N7_COL 3 //
#define N8_COL 2 //
#define N9_COL 1 //
#define N0_COL 0 //

										//Row Q-T
#define Q_T_ROW 1
										//Cols Q-T
#define Q_COL 0 //
#define W_COL 1 //
#define E_COL 2 //
#define R_COL 3 //
#define T_COL 4 //

										//Row Y-P
#define Y_P_ROW 4
										//Cols Y-P
#define Y_COL 4 //
#define U_COL 3 //
#define I_COL 2 //
#define O_COL 1 //
#define P_COL 0 //

										//Row A-G
#define A_G_ROW 2
										//Cols A-G
#define A_COL 0 //
#define S_COL 1 //
#define D_COL 2 //
#define F_COL 3 //
#define G_COL 4 //

										//Row H-L
#define H_L_ROW 6
										//Cols H-L
#define H_COL 4 //
#define J_COL 3 //
#define K_COL 2 //
#define L_COL 1 //

										//Row Z-V
#define Z_V_ROW 5 //
										//Cols Z-V
#define Z_COL 1 //
#define X_COL 2 //
#define C_COL 3 //
#define V_COL 4 //

										//Row B-M
#define B_M_ROW 7
										//Cols B-M
#define B_COL 4 //
#define N_COL 3 //
#define M_COL 2 //
#define SP_COL 0 //

#ifndef atmega644
void leds_debug() {
	LED_ON;
	_delay_ms(60);
	LED_OFF;
	_delay_ms(30);
}
#endif

void pinSet(uint8_t pin, uint8_t bcd, uint8_t stat) //stat 1 = in, stat 0 = out
{
	switch (bcd) {
#ifdef atmega644
	case PA:  if (stat) DDRA &= ~_BV(pin); else DDRA |= _BV(pin); break;
#endif
	case PB:  if (stat) DDRB &= ~_BV(pin); else DDRB |= _BV(pin); break;
	case PC:  if (stat) DDRC &= ~_BV(pin); else DDRC |= _BV(pin); break;
	case PD:  if (stat) DDRD &= ~_BV(pin); else DDRD |= _BV(pin); break;
	}
}

uint8_t pinStat(uint8_t pin, uint8_t bcd)
{
	switch (bcd) {
#ifdef atmega644
	case PA:  if (!(PINA & (1 << pin))) return 1; else return 0; break;
#endif
	case PB:  if (!(PINB & (1 << pin))) return 1; else return 0; break;
	case PC:  if (!(PINC & (1 << pin))) return 1; else return 0; break;
	case PD:  if (!(PIND & (1 << pin))) return 1; else return 0; break;
	}
	return 0;
}

void pinPut(uint8_t pin, uint8_t bcd, uint8_t stat) //stat 1 = HI, stat 0 = LO
{
	switch (bcd) {
#ifdef atmega644
	case PA:  if (!stat) PORTA &= ~_BV(pin); else PORTA |= _BV(pin); break;
#endif
	case PB:  if (!stat) PORTB &= ~_BV(pin); else PORTB |= _BV(pin); break;
	case PC:  if (!stat) PORTC &= ~_BV(pin); else PORTC |= _BV(pin); break;
	case PD:  if (!stat) PORTD &= ~_BV(pin); else PORTD |= _BV(pin); break;
	}
}

void ps2Mode(uint8_t pin, uint8_t mode)
{
	if (mode) PS2_DDR &= ~_BV(pin); //Hi-Entrada 
	else     PS2_DDR |= _BV(pin);  //Low-Salilda

}

void ps2Init()
{
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

uint8_t checkState(uint16_t tramo) {
	uint16_t cont = 0;

	while (cont++ < tramo) {
		if (!(PS2_PIN & (1 << PS2_DAT)))
			return 1;
		_delay_us(5);
	}

	return 0;
}

void _delay_us_4usteps(uint8_t us)
{
	while (0<us)
	{
		_delay_us(4);
		us -= 4;
	}
}

//En us, reloj y semireloj, para los flancos
//Uso normal: CK1 = 20, CK2 = 40 // Para codigo sin optimizar (x12) CK1 = 240, CK2 = 480.  //JOyPs2 CK1=15 CK2=30 //Nuevo JoyPs2 CK1=16 CK2=32 //Mio CK1=4 CK2=8
#define CK1 4
#define CK2 8
uint8_t CKm = 1;  //Multiplicador de CK1 y CK2

//envio de datos ps/2 simulando reloj con delays.
void sendPS2(unsigned char code)
{
	//Para continuar las l�neas deben estar en alto
	//if (ps2Stat())
	//	return;   
	while (ps2Stat());

	unsigned char parity = 1;
	uint8_t i = 0;

	//iniciamos transmisi�n
	ps2Mode(PS2_DAT, LO);
	_delay_us_4usteps(CK1*CKm);

	ps2Mode(PS2_CLK, LO); //bit de comienzo
	_delay_us_4usteps(CK2*CKm);
	ps2Mode(PS2_CLK, HI);
	_delay_us_4usteps(CK1*CKm);
	//enviamos datos
	for (i = 0; i < 8; ++i)
	{
		if (code & (1 << i))
		{
			ps2Mode(PS2_DAT, HI);
			parity = parity ^ 1;
		}
		else ps2Mode(PS2_DAT, LO);

		_delay_us_4usteps(CK1*CKm);
		ps2Mode(PS2_CLK, LO);
		_delay_us_4usteps(CK2*CKm);
		ps2Mode(PS2_CLK, HI);
		_delay_us_4usteps(CK1*CKm);
	}

	// Enviamos bit de paridad
	if (parity) ps2Mode(PS2_DAT, HI);
	else        ps2Mode(PS2_DAT, LO);

	_delay_us_4usteps(CK1*CKm);
	ps2Mode(PS2_CLK, LO);
	_delay_us_4usteps(CK2*CKm);
	ps2Mode(PS2_CLK, HI);
	_delay_us_4usteps(CK1*CKm);

	//Bit de parada
	ps2Mode(PS2_DAT, HI);
	_delay_us_4usteps(CK1*CKm);
	ps2Mode(PS2_CLK, LO);
	_delay_us_4usteps(CK2*CKm);
	ps2Mode(PS2_CLK, HI);
	_delay_us_4usteps(CK1*CKm);

	_delay_us(50);    //fin

}

int getPS2(unsigned char *ret) //Lectura de PS2 para acceso bidireccional
{
	unsigned char data = 0x00;
	unsigned char p = 0x01;
	uint8_t i = 0;

	// discard the start bit
	while ((PS2_PIN & (1 << PS2_DAT)));
	while (!(PS2_PIN & (1 << PS2_CLK)));

	// Bit de comienzo
	_delay_us_4usteps(CK1*CKm);
	ps2Mode(PS2_CLK, LO);
	_delay_us_4usteps(CK2*CKm);
	ps2Mode(PS2_CLK, HI);
	_delay_us_4usteps(CK1*CKm);

	// read each data bit
	for (i = 0; i<8; i++) {
		if ((PS2_PIN & (1 << PS2_DAT))) {
			data = data | (1 << i);
			p = p ^ 1;
		}
		_delay_us_4usteps(CK1*CKm);
		ps2Mode(PS2_CLK, LO);
		_delay_us_4usteps(CK2*CKm);
		ps2Mode(PS2_CLK, HI);
		_delay_us_4usteps(CK1*CKm);
	}

	// read the parity bit	
	if (((PS2_PIN & (1 << PS2_DAT)) != 0) != p) {
		return -1;
	}
	_delay_us_4usteps(CK1*CKm);
	ps2Mode(PS2_CLK, LO);
	_delay_us_4usteps(CK2*CKm);
	ps2Mode(PS2_CLK, HI);
	_delay_us_4usteps(CK1*CKm);

	// send 'ack' bit
	ps2Mode(PS2_DAT, LO);
	_delay_us_4usteps(CK1*CKm);
	ps2Mode(PS2_CLK, LO);
	_delay_us_4usteps(CK2*CKm);
	ps2Mode(PS2_CLK, HI);
	ps2Mode(PS2_DAT, HI);

	_delay_us(100);

	*ret = data;
	return 0;
}

void imprimeversion() //Imprime la fecha de la version en modos que no sean ZX ni PC
{
	int n;
	char pausa = 50;
	if (!modo) 
	{ 
		sendPS2(0xF0); sendPS2(CAPS_SHIFT); matriz[CAPS_SHIFT_ROW][CAPS_SHIFT_COL] = 0;
		sendPS2(0xF0); sendPS2(SYMBOL_SHIFT); matriz[SYMBOL_SHIFT_ROW][SYMBOL_SHIFT_COL] = 0;
	}
    
    sendPS2(KEY_SPACE); _delay_ms(pausa); sendPS2(0xF0); sendPS2(KEY_SPACE);

	for (n = 0; n<8; n++)
	{
		if (n == 2 || n == 4) { _delay_ms(pausa); sendPS2(KEY_PUNTO); _delay_ms(pausa); sendPS2(0xF0); sendPS2(KEY_PUNTO); }
		_delay_ms(pausa);
		sendPS2(versionKeyCodes[version[n]]);
		_delay_ms(pausa);
		sendPS2(0xF0);
		sendPS2(versionKeyCodes[version[n]]);
		_delay_ms(pausa);
	}
	fnpulsada = 1;
	fnpulsando = 1;
}

void eepromsave() //Imprime ' .CFGFLASHED' y guarda en la EEPROM el modo actual
{	
	int n;
	char pausa = 50;
	if (!modo)
	{
		sendPS2(0xF0); sendPS2(CAPS_SHIFT); matriz[CAPS_SHIFT_ROW][CAPS_SHIFT_COL] = 0;
		sendPS2(0xF0); sendPS2(SYMBOL_SHIFT); matriz[SYMBOL_SHIFT_ROW][SYMBOL_SHIFT_COL] = 0;
	}
	eeprom_write_byte((uint8_t*)5, (uint8_t)modo);	
	if (codeset == 2)
	{
		_delay_ms(pausa); sendPS2(KEY_SPACE); _delay_ms(pausa); sendPS2(0xF0); sendPS2(KEY_SPACE);
		_delay_ms(pausa); sendPS2(KEY_PUNTO); _delay_ms(pausa); sendPS2(0xF0); sendPS2(KEY_PUNTO);
		for (n = 0; n < 10; n++)
		{
			_delay_ms(pausa);
			sendPS2(eepromsavename[n]);
			_delay_ms(pausa);
			sendPS2(0xF0);
			sendPS2(eepromsavename[n]);
			_delay_ms(pausa);
		}
	}
	fnpulsada = 1;
	fnpulsando = 1;
}

void imprimecore(const uint8_t nomcore[]) //Imprime el nombre del core
{
	int n;
	char pausa = 100;
	_delay_ms(pausa); sendPS2(KEY_SPACE); _delay_ms(pausa); sendPS2(0xF0); sendPS2(KEY_SPACE);
	_delay_ms(pausa); sendPS2(KEY_PUNTO); _delay_ms(pausa); sendPS2(0xF0); sendPS2(KEY_PUNTO);
	for (n = 1; n<nomcore[0] + 1; n++)
	{
		_delay_ms(pausa);
		sendPS2(nomcore[n]);
		_delay_ms(pausa);
		sendPS2(0xF0);
		sendPS2(nomcore[n]);
		_delay_ms(pausa);
	}
	CKm = nomcore[nomcore[0] + 1]; //Valor de CKm en la configuracion de nomcore[]

}

void cambiafkbmode()
{
	int n;
	char pausa = 50;
	if (!modo)
	{
		sendPS2(0xF0); sendPS2(CAPS_SHIFT); matriz[CAPS_SHIFT_ROW][CAPS_SHIFT_COL] = 0;
		sendPS2(0xF0); sendPS2(SYMBOL_SHIFT); matriz[SYMBOL_SHIFT_ROW][SYMBOL_SHIFT_COL] = 0;
	}	
	if (codeset == 2)
	{
		_delay_ms(pausa); sendPS2(KEY_SPACE); _delay_ms(pausa); sendPS2(0xF0); sendPS2(KEY_SPACE);
		_delay_ms(pausa); sendPS2(KEY_PUNTO); _delay_ms(pausa); sendPS2(0xF0); sendPS2(KEY_PUNTO);

		switch (fkbmode)
		{
		case 0:
			for (n = 0; n < 15; n++)
			{
				_delay_ms(pausa);
				sendPS2(fkbmode0[n]);
				_delay_ms(pausa);
				sendPS2(0xF0);
				sendPS2(fkbmode0[n]);
				_delay_ms(pausa);
				eeprom_write_byte((uint8_t*)6, (uint8_t)fkbmode);
			}
			break;
		case 1:
			for (n = 0; n < 12; n++)
			{
				_delay_ms(pausa);
				sendPS2(fkbmode1[n]);
				_delay_ms(pausa);
				sendPS2(0xF0);
				sendPS2(fkbmode1[n]);
				_delay_ms(pausa);
				eeprom_write_byte((uint8_t*)6, (uint8_t)fkbmode);
			}
			break;
		case 2:
			for (n = 0; n < 6; n++)
			{
				_delay_ms(pausa);
				sendPS2(fkbmode2[n]);
				_delay_ms(pausa);
				sendPS2(0xF0);
				sendPS2(fkbmode2[n]);
				_delay_ms(pausa);
				eeprom_write_byte((uint8_t*)6, (uint8_t)fkbmode);
			}
			break;
		default:
			break;
		}
		
	}
	fnpulsada = 1;
	fnpulsando = 1;
}

//Inicializar Matriz
void matrixInit()
{
	uint8_t c, r;

#ifndef atmega644
	LED_CONFIG;
	LED_OFF;
#endif

#ifdef switches
	for (c = 0; c<NUMSW; c++)
	{
		pinSet(pinsSW[c], bcdSW[c], _IN);
		pinPut(pinsSW[c], bcdSW[c], HI);
	}
#endif

	for (c = 0; c<COLS; c++)
	{
		pinSet(pinsC[c], bcdC[c], _IN);
		pinPut(pinsC[c], bcdC[c], HI);
	}

	for (r = 0; r < ROWS; r++)
	{
		pinSet(pinsR[r], bcdR[r], _IN);
	}
}



enum KBMODE cambiarmodo2(enum KBMODE modokb)
{
	opqa_cursors = 0;
	if (modokb == zx)  CKm = nomZX[nomZX[0] + 1];
	if (modokb == cpc) CKm = nomCPC[nomCPC[0] + 1];
	if (modokb == msx) CKm = nomMSX[nomMSX[0] + 1];
	if (modokb == c64) CKm = nomC64[nomC64[0] + 1];
	if (modokb == at8) CKm = nomAT8[nomAT8[0] + 1];
	if (modokb == bbc) CKm = nomBBC[nomBBC[0] + 1];
	if (modokb == aco) CKm = nomACO[nomACO[0] + 1];
	if (modokb == ap2) CKm = nomAP2[nomAP2[0] + 1];
	if (modokb == vic) CKm = nomVIC[nomVIC[0] + 1];
	if (modokb == ori) CKm = nomORI[nomORI[0] + 1];
	if (modokb == sam) CKm = nomSAM[nomSAM[0] + 1];
	if (modokb == jup) CKm = nomJUP[nomJUP[0] + 1];
  if (modokb == pc) { CKm = nompc[nompc[0] + 1]; kbescucha = 1; timeout_escucha = 0; codeset = 2; } // Iniciamos la escucha para que se pueda cambiar al core de PC/XT.
	
	return modokb;
}

enum KBMODE cambiarmodo(enum KBMODE modokb)
{
	opqa_cursors = 0;
	if (modokb == zx)  imprimecore(nomZX);
	if (modokb == cpc) imprimecore(nomCPC);
	if (modokb == msx) imprimecore(nomMSX);
	if (modokb == c64) imprimecore(nomC64);
	if (modokb == at8) imprimecore(nomAT8);
	if (modokb == bbc) imprimecore(nomBBC);
	if (modokb == aco) imprimecore(nomACO);
	if (modokb == ap2) imprimecore(nomAP2);
	if (modokb == vic) imprimecore(nomVIC);
	if (modokb == ori) imprimecore(nomORI);
	if (modokb == sam) imprimecore(nomSAM);
	if (modokb == jup) imprimecore(nomJUP);
  if (modokb == pc) { kbescucha = 1; timeout_escucha = 0; codeset = 2; imprimecore(nompc); } // Iniciamos la escucha para que se pueda cambiar al core de PC/XT.

	//Uso normal: CK1 = 20, CK2 = 40 // Para codigo sin optimizar (x12) CK1 = 240, CK2 = 480.  //JOyPs2 CK1=15 CK2=30 //Mio CK1=4 CK2=8
	//if(modokb>0) CKm=4; else CKm=1; //Se coge del Nombrecore[]
	if (modokb<MAXKB) codeset = 2;
	fnpulsada = 1;
	fnpulsando = 1;
	cambiomodo = 0; //para salir del bucle del cambiomodo
#ifndef atmega644
	LED_OFF; //Apaga el led de control (salida de bucle de cambio de modo)
#endif
	return modokb;

}

void pulsafn(unsigned char row, unsigned char col, unsigned char key, unsigned char key_E0, unsigned char shift, unsigned char ctrl, unsigned char alt, unsigned char useg)
{ 	
	if (espera) { _delay_us(5); espera = 0; }
	if (shift) { if (codeset == 2) { sendPS2(KEY_LSHIFT); espera++; } else { sendPS2(KS1_LSHIFT); espera++; } }//El Shift no necesita E0
	if (ctrl) { if (codeset == 2) { sendPS2(0xE0); sendPS2(KEY_RCTRL);  espera++; } else { sendPS2(0xE0); sendPS2(KS1_RCTRL);  espera++; } }//Se manda E0 para el control derecho (que vale para ambos casos)
	if (alt) { if (codeset == 2) { sendPS2(KEY_LALT);   espera++; } else { sendPS2(KS1_LALT);   espera++; } }//Usamos el Alt izdo siempre
	if (espera) { _delay_us(5); espera = 0; }
	if (key_E0) { sendPS2(0xE0); } //La tecla requiere modo E0 del PS2
	sendPS2(key);
	_delay_ms(50);
	if (key_E0) { sendPS2(0xE0); }
	if (codeset == 2) sendPS2(0xF0);
	if (codeset == 2) sendPS2(key);  else sendPS2(key + KS1_RELEASE);
	matriz[row][col] = 0;
	if (shift) { if (codeset == 2) { sendPS2(0xF0); sendPS2(KEY_LSHIFT); } else { sendPS2(KS1_LSHIFT + KS1_RELEASE); } }
	if (ctrl) { if (codeset == 2) { sendPS2(0xE0); sendPS2(0xF0); sendPS2(KEY_RCTRL); } else { sendPS2(0xE0); sendPS2(KS1_RCTRL + KS1_RELEASE); } }
	if (alt) { if (codeset == 2) { sendPS2(0xF0); sendPS2(KEY_LALT); } else { sendPS2(KS1_RALT + KS1_RELEASE); } }
	_delay_us(5);
	fnpulsada = 1;
	fnpulsando = 1;
  kbalt = 0;
}

unsigned char traducekey(unsigned char key, enum KBMODE modokb) // con esta funcion ahorramos muchas matrices de mapas y por tanto memoria dinamica del AVR
{
	// Se hace OR 0x80 al key que no requiera shift (KEY_F7 es el unico scancode incompatible, ya se resolveria en caso de necesidad)
	// combinaciones no usables, comun a todos los cores
	key =
	key == KEY_E ? 0 : key == KEY_W ? 0 : key == KEY_Q ? 0 : 
	key == KEY_I ? 0 : key == KEY_SPACE ? 0 : key == KEY_ENTER ? 0 : key;		

	if (modokb == pc && codeset == 1) return key; // pcxt tiene su propio mapa con scancodes distintos

	// combinaciones numericas comunes a todos los cores
	if (key == KEY_1 || key == KEY_3 || key == KEY_4 || key == KEY_5) return key;
	
	// traduccion de key segun el core
	if (key != 0)
	{
		switch (modokb)
		{
		case cpc:
			key =
				key == KEY_2 ? KEY_ACORCHE | 0x80 : key == KEY_T ? KEY_PUNTO : key == KEY_R ? KEY_COMA : key == KEY_G ? KEY_BKSLASH :
				key == KEY_F ? KEY_CCORCHE : key == KEY_D ? KEY_LESS | 0x80 : key == KEY_S ? KEY_ACORCHE : key == KEY_A ? KEY_LESS :
				key == KEY_Y ? KEY_CCORCHE | 0x80 : key == KEY_U ? KEY_BKSLASH | 0x80 : key == KEY_O ? KEY_COMILLA | 0x80 : key == KEY_P ? KEY_2 :
				key == KEY_V ? KEY_SLASH | 0x80 : key == KEY_C ? KEY_SLASH : key == KEY_X ? KEY_IGUAL : key == KEY_Z ? KEY_PTOCOMA | 0x80 :
				key == KEY_H ? KEY_IGUAL | 0x80 : key == KEY_J ? KEY_MENOS | 0x80 : key == KEY_K ? KEY_COMILLA : key == KEY_L ? KEY_MENOS :
				key == KEY_B ? KEY_PTOCOMA : key == KEY_N ? KEY_COMA | 0x80 : key == KEY_M ? KEY_PUNTO | 0x80 :

				(key == KEY_6 || key == KEY_7 || key == KEY_8 || key == KEY_9 || key == KEY_0) ? key : 0;
			break;

		case msx:
			key =
				key == KEY_2 ? KEY_COMILLA : key == KEY_T ? KEY_PUNTO : key == KEY_R ? KEY_COMA : key == KEY_G ? KEY_ACORCHE :
				key == KEY_F ? KEY_TLD | 0x80 : key == KEY_D ? KEY_BKSLASH | 0x80 : key == KEY_S ? KEY_BKSLASH : key == KEY_A ? KEY_CCORCHE :
				key == KEY_6 ? KEY_COMILLA | 0x80 : key == KEY_7 ? KEY_PTOCOMA : key == KEY_8 ? KEY_0 : key == KEY_0 ? KEY_IGUAL | 0x80 :
				key == KEY_Y ? KEY_2 : key == KEY_U ? KEY_ACORCHE | 0x80 : key == KEY_O ? KEY_PTOCOMA | 0x80 : key == KEY_P ? KEY_8 :
				key == KEY_V ? KEY_SLASH | 0x80 : key == KEY_C ? KEY_SLASH : key == KEY_Z ? KEY_IGUAL : key == KEY_H ? KEY_7 :
				key == KEY_J ? KEY_MENOS | 0x80 : key == KEY_K ? KEY_TLD : key == KEY_L ? KEY_6 : key == KEY_B ? KEY_9 :
				key == KEY_N ? KEY_COMA | 0x80 : key == KEY_M ? KEY_PUNTO | 0x80 : 0;
			break;

		case c64:
			key =
				key == KEY_2 ? KEY_ACORCHE | 0x80 : key == KEY_T ? KEY_COMA : key == KEY_R ? KEY_LESS : key == KEY_S ? KEY_MENOS :
				key == KEY_Y ? KEY_PTOCOMA : key == KEY_U ? KEY_COMILLA : key == KEY_O ? KEY_COMILLA | 0x80 : key == KEY_P ? KEY_2 :
				key == KEY_V ? KEY_SLASH | 0x80 : key == KEY_C ? KEY_SLASH : key == KEY_Z ? KEY_PTOCOMA | 0x80 : key == KEY_H ? KEY_BKSLASH | 0x80 :
				key == KEY_J ? KEY_MENOS | 0x80 : key == KEY_K ? KEY_F10 | 0x80 : key == KEY_L ? KEY_IGUAL | 0x80 : key == KEY_B ? KEY_CCORCHE | 0x80 :
				key == KEY_N ? KEY_LESS | 0x80 : key == KEY_M ? KEY_COMA | 0x80 :

				(key == KEY_6 || key == KEY_7 || key == KEY_8 || key == KEY_9 || key == KEY_0) ? key : 0;
			break;

		case at8:
			key =
				key == KEY_2 ? KEY_8 : key == KEY_T ? KEY_IGUAL | 0x80 : key == KEY_R ? KEY_MENOS | 0x80 : key == KEY_W ? KEY_F12 | 0x80 :
				key == KEY_Q ? KEY_F11 | 0x80 : key == KEY_D ? KEY_COMILLA : key == KEY_S ? KEY_CCORCHE : key == KEY_Y ? KEY_COMA :
				key == KEY_U ? KEY_PUNTO : key == KEY_O ? KEY_PTOCOMA | 0x80 : key == KEY_P ? KEY_2 : key == KEY_V ? KEY_SLASH | 0x80 :
				key == KEY_C ? KEY_SLASH : key == KEY_Z ? KEY_PTOCOMA : key == KEY_H ? KEY_BKSLASH : key == KEY_J ? KEY_ACORCHE | 0x80 :
				key == KEY_K ? KEY_COMILLA | 0x80 : key == KEY_L ? KEY_CCORCHE | 0x80 : key == KEY_B ? KEY_BKSLASH | 0x80 : key == KEY_N ? KEY_COMA | 0x80 :
				key == KEY_M ? KEY_PUNTO | 0x80 :

				(key == KEY_6 || key == KEY_7 || key == KEY_8 || key == KEY_9 || key == KEY_0) ? key : 0;
			break;

		case bbc:
			key =
				key == KEY_2 ? KEY_TLD | 0x80 : key == KEY_T ? KEY_PUNTO : key == KEY_R ? KEY_COMA : key == KEY_W ? KEY_F12 | 0x80 :
				key == KEY_Q ? KEY_F12 : key == KEY_S ? KEY_LESS : key == KEY_A ? KEY_MENOS | 0x80 : key == KEY_O ? KEY_PTOCOMA | 0x80 :
				key == KEY_P ? KEY_2 : key == KEY_V ? KEY_SLASH | 0x80 : key == KEY_C ? KEY_SLASH : key == KEY_X ? KEY_BKSLASH :
				key == KEY_Z ? KEY_COMILLA | 0x80 : key == KEY_H ? KEY_IGUAL | 0x80 : key == KEY_J ? KEY_BKSLASH | 0x80 : key == KEY_K ? KEY_PTOCOMA :
				key == KEY_L ? KEY_MENOS : key == KEY_B ? KEY_COMILLA : key == KEY_N ? KEY_COMA | 0x80 : key == KEY_M ? KEY_PUNTO | 0x80 :

				(key == KEY_6 || key == KEY_7 || key == KEY_8 || key == KEY_9 || key == KEY_0) ? key : 0;
			break;

		case aco:
			key =
				key == KEY_2 ? KEY_0 : key == KEY_T ? KEY_PUNTO : key == KEY_R ? KEY_COMA : key == KEY_D ? KEY_BKSLASH | 0x80 :
				key == KEY_Y ? KEY_ACORCHE : key == KEY_U ? KEY_CCORCHE | 0x80 : key == KEY_O ? KEY_PTOCOMA | 0x80 : key == KEY_P ? KEY_2 :
				key == KEY_V ? KEY_SLASH | 0x80 : key == KEY_C ? KEY_SLASH : key == KEY_X ? KEY_ACORCHE : key == KEY_Z ? KEY_COMILLA | 0x80 :
				key == KEY_J ? KEY_MENOS | 0x80 : key == KEY_K ? KEY_PTOCOMA : key == KEY_L ? KEY_MENOS : key == KEY_B ? KEY_COMILLA :
				key == KEY_N ? KEY_COMA | 0x80 : key == KEY_M ? KEY_PUNTO | 0x80 :

				(key == KEY_6 || key == KEY_7 || key == KEY_8 || key == KEY_9 || key == KEY_0) ? key : 0;
			break;

		case ap2:
			key =
				key == KEY_T ? KEY_PUNTO : key == KEY_R ? KEY_COMA : key == KEY_6 ? KEY_7 : key == KEY_7 ? KEY_COMILLA | 0x80 :
				key == KEY_8 ? KEY_9 : key == KEY_9 ? KEY_0 : key == KEY_0 ? KEY_MENOS : key == KEY_Y ? KEY_ACORCHE | 0x80 :
				key == KEY_U ? KEY_CCORCHE | 0x80 : key == KEY_O ? KEY_PTOCOMA | 0x80 : key == KEY_P ? KEY_COMILLA : key == KEY_V ? KEY_SLASH | 0x80 :
				key == KEY_C ? KEY_SLASH : key == KEY_Z ? KEY_PTOCOMA : key == KEY_H ? KEY_6 : key == KEY_J ? KEY_MENOS | 0x80 :
				key == KEY_K ? KEY_IGUAL : key == KEY_L ? KEY_IGUAL | 0x80 : key == KEY_B ? KEY_8 : key == KEY_N ? KEY_COMA | 0x80 :
				key == KEY_M ? KEY_PUNTO | 0x80 : key == KEY_2 ? key : 0;
			break;

		case vic:
			key =
				key == KEY_2 ? KEY_ACORCHE | 0x80 : key == KEY_T ? KEY_COMA : key == KEY_R ? KEY_LESS : key == KEY_S ? KEY_IGUAL :
				key == KEY_Y ? KEY_PTOCOMA : key == KEY_U ? KEY_COMILLA : key == KEY_O ? KEY_COMILLA | 0x80 : key == KEY_P ? KEY_2 :
				key == KEY_V ? KEY_SLASH | 0x80 : key == KEY_C ? KEY_SLASH : key == KEY_Z ? KEY_PTOCOMA | 0x80 : key == KEY_J ? KEY_IGUAL | 0x80 :
				key == KEY_K ? KEY_MENOS | 0x80 : key == KEY_L ? KEY_BKSLASH | 0x80 : key == KEY_B ? KEY_CCORCHE | 0x80 : key == KEY_N ? KEY_LESS | 0x80 :
				key == KEY_M ? KEY_COMA | 0x80 :

				(key == KEY_6 || key == KEY_7 || key == KEY_8 || key == KEY_9 || key == KEY_0) ? key : 0;
			break;

		case ori:
			key =
				key == KEY_T ? KEY_PUNTO : key == KEY_R ? KEY_COMA : key == KEY_G ? KEY_CCORCHE : key == KEY_F ? KEY_ACORCHE :
				key == KEY_D ? KEY_BKSLASH | 0x80 : key == KEY_S ? KEY_BKSLASH : key == KEY_6 ? KEY_7 : key == KEY_7 ? KEY_COMILLA | 0x80 :
				key == KEY_8 ? KEY_9 : key == KEY_9 ? KEY_0 : key == KEY_Y ? KEY_ACORCHE | 0x80 : key == KEY_U ? KEY_CCORCHE | 0x80 :
				key == KEY_O ? KEY_PTOCOMA | 0x80 : key == KEY_P ? KEY_COMILLA : key == KEY_V ? KEY_SLASH | 0x80 : key == KEY_C ? KEY_SLASH :
				key == KEY_X ? KEY_MENOS : key == KEY_Z ? KEY_PTOCOMA : key == KEY_H ? KEY_6 : key == KEY_J ? KEY_MENOS | 0x80 :
				key == KEY_K ? KEY_IGUAL : key == KEY_L ? KEY_IGUAL | 0x80 : key == KEY_B ? KEY_8 : key == KEY_N ? KEY_COMA | 0x80 :
				key == KEY_M ? KEY_PUNTO | 0x80 : key == KEY_0 ? key : 0;
			break;

		case sam:
			key =
				key == KEY_2 ? KEY_BKSLASH : key == KEY_T ? KEY_LESS : key == KEY_R ? KEY_LESS | 0x80 : key == KEY_G ? KEY_BKSLASH | 0x80 :
				key == KEY_F ? KEY_COMILLA | 0x80 : key == KEY_A ? KEY_PTOCOMA | 0x80 : key == KEY_7 ? KEY_MENOS | 0x80 : key == KEY_0 ? KEY_SLASH :
				key == KEY_O ? KEY_COMA : key == KEY_P ? KEY_2 : key == KEY_V ? KEY_7 : key == KEY_C ? KEY_MENOS :
				key == KEY_X ? KEY_ACORCHE | 0x80 : key == KEY_Z ? KEY_PUNTO : key == KEY_H ? KEY_ACORCHE : key == KEY_J ? KEY_SLASH | 0x80 :
				key == KEY_K ? KEY_CCORCHE | 0x80 : key == KEY_L ? KEY_0 : key == KEY_B ? KEY_CCORCHE : key == KEY_N ? KEY_COMA | 0x80 :
				key == KEY_M ? KEY_PUNTO | 0x80 :

				(key == KEY_6 || key == KEY_8 || key == KEY_9) ? key : 0;
			break;

		case jup:
			key =
				key == KEY_2 ? KEY_BKSLASH | 0x80 : key == KEY_T ? KEY_LESS : key == KEY_R ? KEY_LESS | 0x80 : key == KEY_F ? KEY_COMILLA :
				key == KEY_D ? KEY_TLD | 0x80 : key == KEY_7 ? KEY_MENOS | 0x80 : key == KEY_0 ? KEY_SLASH : key == KEY_Y ? KEY_ACORCHE :
				key == KEY_O ? KEY_COMA : key == KEY_P ? KEY_2 : key == KEY_V ? KEY_7 : key == KEY_C ? KEY_MENOS : key == KEY_X ? KEY_COMILLA | 0x80 :
				key == KEY_Z ? KEY_PUNTO : key == KEY_H ? KEY_ACORCHE | 0x80 : key == KEY_J ? KEY_SLASH | 0x80 : key == KEY_K ? KEY_CCORCHE | 0x80 :
				key == KEY_L ? KEY_0 : key == KEY_B ? KEY_CCORCHE : key == KEY_N ? KEY_COMA | 0x80 : key == KEY_M ? KEY_PUNTO | 0x80 :

				(key == KEY_6 || key == KEY_8 || key == KEY_9) ? key : 0;
			break;
		case pc:
			key =
				key == KEY_0 ? KEY_MENOS : key == KEY_9 ? KEY_0 : key == KEY_8 ? KEY_9 : key == KEY_7 ? KEY_COMILLA | 0x80 :
				key == KEY_6 ? KEY_7 : key == KEY_T ? KEY_PUNTO : key == KEY_R ? KEY_COMA : key == KEY_G ? KEY_CCORCHE :
				key == KEY_F ? KEY_ACORCHE : key == KEY_D ? KEY_LESS | 0x80 : key == KEY_S ? KEY_BKSLASH : key == KEY_A ? KEY_TLD :
				key == KEY_Y ? KEY_ACORCHE | 0x80 : key == KEY_U ? KEY_CCORCHE | 0x80 : key == KEY_O ? KEY_PTOCOMA | 0x80 : key == KEY_P ? KEY_COMILLA :
				key == KEY_V ? KEY_SLASH | 0x80 : key == KEY_C ? KEY_SLASH : key == KEY_Z ? KEY_PTOCOMA : key == KEY_H ? KEY_6 :
				key == KEY_J ? KEY_MENOS | 0x80 : key == KEY_K ? KEY_IGUAL : key == KEY_L ? KEY_IGUAL | 0x80 : key == KEY_B ? KEY_8 :
				key == KEY_N ? KEY_COMA | 0x80 : key == KEY_M ? KEY_PUNTO | 0x80 : key == KEY_2 ? key : 0;
			break;

		default:
			key = 0;
			break;
		}
	}

	return key;
}

void pulsateclaconsymbol(unsigned char row, unsigned char col, enum KBMODE modokb)
{
	unsigned char key = 0, shift = 0;
  typematicfirst = 0;
  typematic_codeaux = 0;
	key = traducekey(mapZX[row][col], modokb);
	if (key != 0)
	{
		if (modokb == pc && codeset == 1) { key = mapXT1[row][col]; shift = modXT1[row][col]; }
		else { shift = !(key & 0x80); if (!shift) key ^= 0x80; }

	    if (shift) { if (codeset == 2) { sendPS2(KEY_LSHIFT); typematic_codeaux = KEY_LSHIFT; } else { sendPS2(KS1_LSHIFT); typematic_codeaux = KS1_LSHIFT; } }   
	    sendPS2(key);
	    typematic_code = key;
  } 
}
void sueltateclaconsymbol(unsigned char row, unsigned char col, enum KBMODE modokb)
{
	unsigned char key=0, shift=0;
  	typematic_code = 0;
	key = traducekey(mapZX[row][col], modokb);
	if (key != 0)
	{
		if (modokb == pc && codeset == 1) { key = mapXT1[row][col]; shift = modXT1[row][col]; }
		else { shift = !(key & 0x80); if (!shift) key ^= 0x80; }

		if (codeset == 2) { sendPS2(0xF0); sendPS2(key); }
		else sendPS2(key + KS1_RELEASE);

		if (shift) { if (codeset == 2) { sendPS2(0xF0); sendPS2(KEY_LSHIFT); } else sendPS2(KS1_LSHIFT + KS1_RELEASE); }
				
	}	
}
void pulsateclaconshift(unsigned char row, unsigned char col, unsigned char key)
{	
  	typematicfirst = 0;
  	typematic_codeaux = 0;
	if (!key) //si no esta mapeada saca la mayuscula
	{
    	if (codeset == 2) { sendPS2(KEY_LSHIFT); typematic_codeaux = KEY_LSHIFT; } else { sendPS2(KS1_LSHIFT); typematic_codeaux = KS1_LSHIFT; }
    	if (codeset == 2) { sendPS2(mapZX[row][col]); typematic_code = mapZX[row][col]; } else { sendPS2(mapSET1[row][col]); typematic_code = mapSET1[row][col]; }
	}
	else
	{
	    if (codeset == 2 && (key == KEY_LEFT || key == KEY_RIGHT || key == KEY_UP || key == KEY_DOWN)) { sendPS2(0xE0); typematic_codeaux = 0xE0; } //Es una tecla del codeset2 que necesita E0
	    if (codeset == 1 && (key == KS1_LEFT || key == KS1_RIGHT || key == KS1_UP || key == KS1_DOWN)) { sendPS2(0xE0); typematic_codeaux = 0xE0; }//Es una tecla del codeset1 que necesita E0
	    sendPS2(key);
	    typematic_code = key;
	}	
}

void sueltateclaconshift(unsigned char row, unsigned char col, unsigned char key)
{
  	typematic_code = 0;
	if (!key) //si no esta mapeada saca la mayuscula
	{
		if (codeset == 2) { sendPS2(0xF0); sendPS2(mapZX[row][col]); sendPS2(0xF0); sendPS2(KEY_LSHIFT); }
		else { sendPS2(mapSET1[row][col] + KS1_RELEASE); sendPS2(KS1_LSHIFT + KS1_RELEASE); }
	}
	else
	{
		if (codeset == 2 && (key == KEY_LEFT || key == KEY_RIGHT || key == KEY_UP || key == KEY_DOWN)) sendPS2(0xE0); //Es una tecla del codeset2 que necesita E0
		if (codeset == 1 && (key == KS1_LEFT || key == KS1_RIGHT || key == KS1_UP || key == KS1_DOWN)) sendPS2(0xE0); //Es una tecla del codeset1 que necesita E0
		if (codeset == 2) { sendPS2(0xF0); sendPS2(key); }
		else sendPS2(key + KS1_RELEASE);
	}	
}
#ifdef atmega644
void traduceextra2a(uint8_t r, uint8_t c, int8_t p)
{
	uint8_t isextra2a = 1;	
	uint8_t re = 0, ce = 0, rt = 0, ct = 0;
	uint8_t csss_status = 0;

	switch (mapZX[r][c])
	{


	case KEY_COMILLA:
		re = SYMBOL_SHIFT_ROW;
		ce = SYMBOL_SHIFT_COL;
		rt = Y_P_ROW;
		ct = P_COL;
		ss_counter += p;
		break;
	case KEY_PTOCOMA:
		re = SYMBOL_SHIFT_ROW;
		ce = SYMBOL_SHIFT_COL;
		rt = Y_P_ROW;
		ct = O_COL;
		ss_counter += p;
		break;
	case KEY_PUNTO:
		re = SYMBOL_SHIFT_ROW;
		ce = SYMBOL_SHIFT_COL;
		rt = B_M_ROW;
		ct = M_COL;
		ss_counter += p;
		break;
	case KEY_COMA:
		re = SYMBOL_SHIFT_ROW;
		ce = SYMBOL_SHIFT_COL;
		rt = B_M_ROW;
		ct = N_COL;
		ss_counter += p;
		break;
	case KEY_F1:
		re = CAPS_SHIFT_ROW;
		ce = CAPS_SHIFT_COL;
		rt = N1_N5_ROW;
		ct = N1_COL;
		cs_counter += p;
		break;
	case KEY_F3:
		re = CAPS_SHIFT_ROW;
		ce = CAPS_SHIFT_COL;
		rt = N1_N5_ROW;
		ct = N3_COL;
		cs_counter += p;
		break;
	case KEY_F4:
		re = CAPS_SHIFT_ROW;
		ce = CAPS_SHIFT_COL;
		rt = N1_N5_ROW;
		ct = N4_COL;
		cs_counter += p;
		break;
	case KEY_F9:
		re = CAPS_SHIFT_ROW;
		ce = CAPS_SHIFT_COL;
		rt = N6_N0_ROW;
		ct = N9_COL;
		cs_counter += p;
		break;
	case KEY_CAPS:
		re = CAPS_SHIFT_ROW;
		ce = CAPS_SHIFT_COL;
		rt = N1_N5_ROW;
		ct = N2_COL;
		cs_counter += p;
		break;
	case KEY_LEFT:
		re = CAPS_SHIFT_ROW;
		ce = CAPS_SHIFT_COL;
		rt = N1_N5_ROW;
		ct = N5_COL;
		cs_counter += p;
		break;
	case KEY_DOWN:
		re = CAPS_SHIFT_ROW;
		ce = CAPS_SHIFT_COL;
		rt = N6_N0_ROW;
		ct = N6_COL;
		cs_counter += p;
		break;
	case KEY_UP:
		re = CAPS_SHIFT_ROW;
		ce = CAPS_SHIFT_COL;
		rt = N6_N0_ROW;
		ct = N7_COL;
		cs_counter += p;
		break;
	case KEY_RIGHT:
		re = CAPS_SHIFT_ROW;
		ce = CAPS_SHIFT_COL;
		rt = N6_N0_ROW;
		ct = N8_COL;
		cs_counter += p;
		break;
#ifdef xchg_del_break
	case KEY_ESCAPE:
#else
	case KEY_BACKSP:
#endif
		re = CAPS_SHIFT_ROW;
		ce = CAPS_SHIFT_COL;
		rt = N6_N0_ROW;
		ct = N0_COL;
		cs_counter += p;
		break;
#ifdef xchg_del_break
	case KEY_BACKSP:
#else
	case KEY_ESCAPE:
#endif
		re = CAPS_SHIFT_ROW;
		ce = CAPS_SHIFT_COL;
		rt = SPACE_ROW;
		ct = SPACE_COL;
		cs_counter += p;
		break;
	case KEY_TAB:
		re = CAPS_SHIFT_ROW;
		ce = CAPS_SHIFT_COL;
		rt = SYMBOL_SHIFT_ROW;
		ct = SYMBOL_SHIFT_COL;
		cs_counter += p;
		break;

	default:
		isextra2a = 0;
		break;
	}
		
	if (isextra2a)
	{	
		csss_status = matriz[rt][ct] & 0x18;
		if (p == -1)
		{
			if (cs_counter == 0 && ss_counter == 0) matriz[re][ce] = matriz[r][c];
		}
		else matriz[re][ce] = matriz[r][c];
		matriz[rt][ct] = matriz[r][c] | csss_status;

	}
	
}
#endif
void matrixScan()
{
	uint8_t r, c;
	uint8_t keyaux = 0;
	
	fnpulsada = 0; //Se pone a 0 la pulsacion de una tecla de funcion

#ifdef switches
	uint8_t b;
	//Controla los Botones Externos del Arduino
	for (b = 0; b<NUMSW; b++)
	{
		if (pinStat(pinsSW[b], bcdSW[b]))
		{
			_delay_us(10); //debounce
			if (pinStat(pinsSW[b], bcdSW[b]))
			{
				if (b == 0) //NMI
				{
					sendPS2(0xE0); sendPS2(KEY_RCTRL);
					sendPS2(KEY_LALT);
					sendPS2(KEY_F5);
					_delay_us(5);
					sendPS2(0xF0); sendPS2(KEY_F5);
					sendPS2(0xE0); sendPS2(0xF0); sendPS2(KEY_RCTRL);
					sendPS2(0xF0); sendPS2(KEY_LALT);
					_delay_us(5);
					fnpulsada = 1;
				}
				if (b == 1) //VGA-SWICH
				{
					sendPS2(KEY_SCRLCK); _delay_us(5);
					sendPS2(0xF0); sendPS2(KEY_SCRLCK); _delay_us(5);
					fnpulsada = 1;
				}
			}
		}
	} //Fin de la comprobacion de botones
#endif

	  //Escaneo de la matriz del teclado
	if (!fnpulsada) for (r = 0; r<ROWS; r++)
	{
		//activar row/fila
		pinSet(pinsR[r], bcdR[r], _OUT);
		pinPut(pinsR[r], bcdR[r], LO);
		_delay_us(5);
		for (c = 0; c<COLS; c++)
		{

			if (pinStat(pinsC[c], bcdC[c]))
			{
				_delay_us(10); //debounce
				if (pinStat(pinsC[c], bcdC[c]))
				{					
					if (matriz[r][c] & 0x01)
					{
						matriz[r][c] |= 0x02; //Marcado como mantenido "0x02"
						#ifdef atmega644
						if (r > 7 && c > 4) // pulsa extra +2A/+3
						{
							traduceextra2a(r, c, 0);
						}
						#endif
					}
					else
					{
						matriz[r][c] |= 0x01; // Marcado como pulsado "0x01"
						#ifdef atmega644
						if (r > 7 && c > 4) // pulsa extra +2A/+3
						{
							traduceextra2a(r, c, 1);
						}
						#endif
					}

				}

				else if (matriz[r][c] & 0x01)
				{ 
					matriz[r][c] &= ~0x02; // Ya no esta mantenida "~0x02"
					matriz[r][c] &= ~0x01; // Ya no esta pulsada "~0x01"
					matriz[r][c] |= 0x04; // Marcado para soltar la tecla "0x04" (si entra por debounce)
					#ifdef atmega644
					if (r > 7 && c > 4) // suelta extra +2A/+3
					{
						traduceextra2a(r, c, -1);
						matriz[r][c] = 0;
					}
					#endif

				} 
			}
			else if (matriz[r][c] & 0x01)
			{ 
				matriz[r][c] &= ~0x02; // Ya no esta mantenida "~0x02"
				matriz[r][c] &= ~0x01; // Ya no esta pulsada "~0x01"
				matriz[r][c] |= 0x04; // Marcado para soltar la tecla "0x04"
				#ifdef atmega644
				if (r > 7 && c > 4) // suelta extra +2A/+3
				{
					traduceextra2a(r, c, -1);
					matriz[r][c] = 0;
				}
				#endif
			}

		}//Fin de Escaneo de las Columnas para el Row/Fila indicado
		 //desact. row/fila
		pinSet(pinsR[r], bcdR[r], _IN);
	} //fin escaneo de Rows/Filas de la matriz del teclado	

	if (cambiomodo)
	{ //Primero verifica que se han soltado todas las teclas antes de pulsar la del modo que se desea.
		if (soltarteclas)
		{
			soltarteclas = 0;
			for (r = 0; r<ROWS8; r++) for (c = 0; c<COLS5; c++) if (matriz[r][c] & 0x01) soltarteclas = 1;
		}
		if (!soltarteclas)
		{
			if (matriz[CAPS_SHIFT_ROW][CAPS_SHIFT_COL] & 0x04) { matriz[CAPS_SHIFT_ROW][CAPS_SHIFT_COL] = 0; if (!modo) { sendPS2(0xF0); sendPS2(CAPS_SHIFT); } }
			if (matriz[SYMBOL_SHIFT_ROW][SYMBOL_SHIFT_COL] & 0x04) { matriz[SYMBOL_SHIFT_ROW][SYMBOL_SHIFT_COL] = 0; if (!modo) { sendPS2(0xF0); sendPS2(SYMBOL_SHIFT); } }
			for (r = 0; r<ROWS8; r++) for (c = 0; c<COLS5; c++) if (matriz[r][c] & 0x01) modo = cambiarmodo(mapMODO[r][c]);
		}
		if (cambiomodo) fnpulsada = 1; //Si no se pulsa ninguna tecla sigue en bucle hasta que se pulse
	}
	//Comprobacion de Teclas especiales al tener pulsado Caps Shift y Symbol Shift
	if (!fnpulsada && (matriz[CAPS_SHIFT_ROW][CAPS_SHIFT_COL] & 0x01) && (matriz[SYMBOL_SHIFT_ROW][SYMBOL_SHIFT_COL] & 0x01))
	{
		if (!fnpulsando)
		{//row, col, key, key_E0, shift, ctrl, alt, museg

			if (modo == pc)
			{
				if (matriz[Z_V_ROW][C_COL] & 0x01) opqa_cursors = !opqa_cursors; // OPQA -> Cursores (Activacion / Desactivacion)
        if (matriz[H_L_ROW][L_COL] & 0x01) kbalt = 1; // Modo Alt activado para F1 - F12
				_delay_ms(200);
			}

			if (codeset == 2)
			{		
				if ((matriz[H_L_ROW][J_COL] & 0x01) && !modo)
				{
					fkbmode++; 
					fkbmode = fkbmode > 2 ? 0 : fkbmode;
					cambiafkbmode();
				}
				if ((matriz[Y_P_ROW][U_COL] & 0x01) && (fkbmode == 1 || modo))
				{
					cambiomodo = 1; fnpulsada = 1; soltarteclas = 1; 
					#ifndef atmega644 
					LED_ON; 
					#endif
				} //Activa el cambio de modo lo que dejara en bucle hasta que se pulse una tecla. El led se enciende.
				if ((matriz[N1_N5_ROW][N1_COL] & 0x01) && modo) pulsafn(N1_N5_ROW, N1_COL, KEY_F1, 0, 0, 0, kbalt, 5);  //F1
				if ((matriz[N1_N5_ROW][N2_COL] & 0x01) && modo) pulsafn(N1_N5_ROW, N2_COL, KEY_F2, 0, 0, 0, kbalt, 5);  //F2
				if ((matriz[N1_N5_ROW][N3_COL] & 0x01) && modo) pulsafn(N1_N5_ROW, N3_COL, KEY_F3, 0, 0, 0, kbalt, 5);  //F3
				if ((matriz[N1_N5_ROW][N4_COL] & 0x01) && modo) pulsafn(N1_N5_ROW, N4_COL, KEY_F4, 0, 0, 0, kbalt, 5);  //F4
				if ((matriz[N1_N5_ROW][N5_COL] & 0x01) && modo) pulsafn(N1_N5_ROW, N5_COL, KEY_F5, 0, 0, 0, kbalt, 5);  //F5
				if ((matriz[N6_N0_ROW][N6_COL] & 0x01) && modo) pulsafn(N6_N0_ROW, N6_COL, KEY_F6, 0, 0, 0, kbalt, 5);  //F6
				if ((matriz[N6_N0_ROW][N7_COL] & 0x01) && modo) pulsafn(N6_N0_ROW, N7_COL, KEY_F7, 0, 0, 0, kbalt, 5);  //F7
				if ((matriz[N6_N0_ROW][N8_COL] & 0x01) && modo) pulsafn(N6_N0_ROW, N8_COL, KEY_F8, 0, 0, 0, kbalt, 5);  //F8
				if ((matriz[N6_N0_ROW][N9_COL] & 0x01) && modo) pulsafn(N6_N0_ROW, N9_COL, KEY_F9, 0, 0, 0, kbalt, 5);  //F9
				if ((matriz[N6_N0_ROW][N0_COL] & 0x01) && modo) pulsafn(N6_N0_ROW, N0_COL, KEY_F10, 0, 0, 0, kbalt, 5); //F10
				if ((matriz[Q_T_ROW][Q_COL] & 0x01) && modo) pulsafn(Q_T_ROW, Q_COL, KEY_F11, 0, 0, 0, kbalt, 50); //F11
				if ((matriz[Q_T_ROW][W_COL] & 0x01) && modo) pulsafn(Q_T_ROW, W_COL, KEY_F12, 0, 0, 0, kbalt, 50); //F12
        
				if ((matriz[A_G_ROW][S_COL] & 0x01) && modo)
				{
					if (modo == at8) //F8 + F10 para Atari
					{
						sendPS2(KEY_F8);
						_delay_ms(50);
						sendPS2(KEY_F10);

						_delay_ms(50);

						sendPS2(0xF0);
						sendPS2(KEY_F10);
						_delay_ms(1000);
						sendPS2(0xF0);
						sendPS2(KEY_F8);
						_delay_ms(50);

						fnpulsada = 1;
						fnpulsando = 1;
					}
					if (modo == c64) //Ctrl + F12 para C64
					{
						pulsafn(A_G_ROW, S_COL, KEY_F12, 0, 0, 1, 0, 5);
					}
				}
				if ((matriz[A_G_ROW][A_COL] & 0x01) && (fkbmode == 1 || modo)) pulsafn(A_G_ROW, A_COL, KEY_F10, 0, 0, 0, 0, 5);       //F10 para el NEXT (¿Mejor cambiar a otra?)

				if ((matriz[Y_P_ROW][Y_COL] & 0x01) && (fkbmode != 2 || modo)) pulsafn(Y_P_ROW, Y_COL, KEY_F5, 0, 0, 1, 1, 5);        //ZXUNO NMI (Control+Alt+F5)
				if ((matriz[B_M_ROW][B_COL] & 0x01) && (fkbmode != 2 || modo)) pulsafn(B_M_ROW, B_COL, KEY_BACKSP, 0, 0, 1, 1, 5);    //ZXUNO Hard Reset (Control+Alt+Backsp)
				if ((matriz[B_M_ROW][N_COL] & 0x01) && (fkbmode != 2 || modo)) pulsafn(B_M_ROW, N_COL, KEY_DELETE, 0, 0, 1, 1, 5);    //ZXUNO Soft Reset (Control+Alt+Supr)
				if ((matriz[A_G_ROW][G_COL] & 0x01) && (fkbmode != 2 || modo)) pulsafn(A_G_ROW, G_COL, KEY_SCRLCK, 0, 0, 0, 0, 5);    //ZXUNO RGB/VGA Swich (Bloq Despl)

				if ((matriz[Z_V_ROW][V_COL] & 0x01) && (fkbmode == 1 || modo)) imprimeversion();
				if ((matriz[Z_V_ROW][X_COL] & 0x01) && (fkbmode == 1 || modo)) eepromsave();											//Guarda en la EEPROM el modo actual de teclado

				if ((matriz[Q_T_ROW][E_COL] & 0x01) && modo) pulsafn(Q_T_ROW, E_COL, KEY_PGUP, 1, 0, 0, 0, 5); //Re Pag / Pg Up   (Acorn: VGA) (C64 Disco Anterior)
				if ((matriz[Q_T_ROW][R_COL] & 0x01) && modo) pulsafn(Q_T_ROW, R_COL, KEY_PGDW, 1, 0, 0, 0, 5); //Av Pag / Pg Down (Acorn: RGB) (C64 Disco Siguiente)
				if ((matriz[A_G_ROW][D_COL] & 0x01) && modo) pulsafn(A_G_ROW, D_COL, KEY_PGUP, 1, 1, 0, 0, 5); //Re Pag / Pg Up   + Shift (C64 10 Discos Anteriores)
				if ((matriz[A_G_ROW][F_COL] & 0x01) && modo) pulsafn(A_G_ROW, F_COL, KEY_PGDW, 1, 1, 0, 0, 5); //Av Pag / Pg Down + Shift (C64 10 Discos Siguientes)
				
			}
			else if (codeset == 1)
			{
				if (matriz[N1_N5_ROW][N1_COL] & 0x01) pulsafn(N1_N5_ROW, N1_COL, KS1_F1, 0, 0, 0, kbalt, 5);  //F1
				if (matriz[N1_N5_ROW][N2_COL] & 0x01) pulsafn(N1_N5_ROW, N2_COL, KS1_F2, 0, 0, 0, kbalt, 5);  //F2
				if (matriz[N1_N5_ROW][N3_COL] & 0x01) pulsafn(N1_N5_ROW, N3_COL, KS1_F3, 0, 0, 0, kbalt, 5);  //F3
				if (matriz[N1_N5_ROW][N4_COL] & 0x01) pulsafn(N1_N5_ROW, N4_COL, KS1_F4, 0, 0, 0, kbalt, 5);  //F4
				if (matriz[N1_N5_ROW][N5_COL] & 0x01) pulsafn(N1_N5_ROW, N5_COL, KS1_F5, 0, 0, 0, kbalt, 5);  //F5
				if (matriz[N6_N0_ROW][N6_COL] & 0x01) pulsafn(N6_N0_ROW, N6_COL, KS1_F6, 0, 0, 0, kbalt, 5);  //F6 
				if (matriz[N6_N0_ROW][N7_COL] & 0x01) pulsafn(N6_N0_ROW, N7_COL, KS1_F7, 0, 0, 0, kbalt, 5);  //F7
				if (matriz[N6_N0_ROW][N8_COL] & 0x01) pulsafn(N6_N0_ROW, N8_COL, KS1_F8, 0, 0, 0, kbalt, 5);  //F8
				if (matriz[N6_N0_ROW][N9_COL] & 0x01) pulsafn(N6_N0_ROW, N9_COL, KS1_F9, 0, 0, 0, kbalt, 5);  //F9
				if (matriz[N6_N0_ROW][N0_COL] & 0x01) pulsafn(N6_N0_ROW, N0_COL, KS1_F10, 0, 0, 0, kbalt, 5); //F10
				if (matriz[Q_T_ROW][Q_COL] & 0x01) pulsafn(Q_T_ROW, Q_COL, KS1_F11, 0, 0, 0, kbalt, 5);		  //F11  
				if (matriz[Q_T_ROW][W_COL] & 0x01) pulsafn(Q_T_ROW, W_COL, KS1_F12, 0, 0, 0, kbalt, 5);		  //F12  
				if (matriz[B_M_ROW][B_COL] & 0x01) pulsafn(B_M_ROW, B_COL, KS1_BACKSP, 0, 0, 1, 1, 5);    //ZXUNO Hard Reset (Control+Alt+Backsp)
				if (matriz[B_M_ROW][N_COL] & 0x01) pulsafn(B_M_ROW, N_COL, KS1_DELETE, 0, 0, 1, 1, 5);    //ZXUNO Soft Reset (Control+Alt+Supr)
       
			}
		}
   
	}
 
	else fnpulsando = 0; //Fin de escaneo de combos

						 //Control de teclado
	if (!fnpulsada) //Si no se ha pulsado ningun tecla de funcion y el modo es 0 (ZX-Spectrum)
	{		
		if (!modo)
		{
			//Enviar la pulsacion de Caps Shift y/o Symbol Shift si estamos en modo ZX)
			if ((matriz[CAPS_SHIFT_ROW][CAPS_SHIFT_COL] & 0x01) && !(matriz[CAPS_SHIFT_ROW][CAPS_SHIFT_COL] & 0x02)) { sendPS2(CAPS_SHIFT);    matriz[CAPS_SHIFT_ROW][CAPS_SHIFT_COL] |= 0x02;     espera++; } // Probar a suprimir matriz[CAPS_SHIFT_ROW][CAPS_SHIFT_COL] |= 0x02; (Ya se deja mantenido durante el scan)
			if ((matriz[SYMBOL_SHIFT_ROW][SYMBOL_SHIFT_COL] & 0x01) && !(matriz[SYMBOL_SHIFT_ROW][SYMBOL_SHIFT_COL] & 0x02)) { sendPS2(SYMBOL_SHIFT);  matriz[SYMBOL_SHIFT_ROW][SYMBOL_SHIFT_COL] |= 0x02; espera++; } // Probar a suprimir matriz[SYMBOL_SHIFT_ROW][SYMBOL_SHIFT_COL] |= 0x02; (Ya se deja mantenido durante el scan)
			if (matriz[CAPS_SHIFT_ROW][CAPS_SHIFT_COL] & 0x04) { sendPS2(0xF0); sendPS2(CAPS_SHIFT);    matriz[CAPS_SHIFT_ROW][CAPS_SHIFT_COL] = 0;     espera++; }
			if (matriz[SYMBOL_SHIFT_ROW][SYMBOL_SHIFT_COL] & 0x04) { sendPS2(0xF0); sendPS2(SYMBOL_SHIFT);  matriz[SYMBOL_SHIFT_ROW][SYMBOL_SHIFT_COL] = 0; espera++; }
			if (espera) { _delay_us(5); espera = 0; }
			//Enviar el resto de Teclas Pulsadas, nunca entrara por shift o symbol de matrix[r][c] ya que se cambia el valor en el bloque anterior a 3 o 0
			for (r = 0; r<ROWS8; r++) for (c = 0; c<COLS5; c++)
			{
				if ((matriz[r][c] & 0x01) && !(matriz[r][c] & 0x02)) sendPS2(mapZX[r][c]);
				if (matriz[r][c] & 0x04) { sendPS2(0xF0); sendPS2(mapZX[r][c]); matriz[r][c] = 0; }
			}
		}
		else // Manejo de los otros modos de Keymap
		{

			
			if (!antighosting && cs_counter == 0 && ss_counter == 0)
			{
				if (((matriz[CAPS_SHIFT_ROW][CAPS_SHIFT_COL] & 0x01) && !(matriz[CAPS_SHIFT_ROW][CAPS_SHIFT_COL] & 0x02)) ||
					((matriz[SYMBOL_SHIFT_ROW][SYMBOL_SHIFT_COL] & 0x01) && !(matriz[SYMBOL_SHIFT_ROW][SYMBOL_SHIFT_COL] & 0x02)))
				{
					for (r = 0; r < ROWS8; r++) for (c = 0; c < COLS5; c++)
					{
						// Ignoramos teclas capssymbol y symbolshift en el recorrido
						if (r == CAPS_SHIFT_ROW && c == CAPS_SHIFT_COL) continue;
						if (r == SYMBOL_SHIFT_ROW && c == SYMBOL_SHIFT_COL) continue;
						if (matriz[r][c] & 0x01) { antighosting = 1; break; }
					}

				}
			}
			else
			{
				if (!(matriz[CAPS_SHIFT_ROW][CAPS_SHIFT_COL] & 0x01) && !(matriz[SYMBOL_SHIFT_ROW][SYMBOL_SHIFT_COL] & 0x01))
					antighosting = 0;
			}
			
			for (r = 0; r<ROWS8; r++) for (c = 0; c<COLS5; c++)
			{
				// Ignoramos teclas capssymbol y symbolshift en el recorrido
				if (r == CAPS_SHIFT_ROW && c == CAPS_SHIFT_COL) continue;
				if (r == SYMBOL_SHIFT_ROW && c == SYMBOL_SHIFT_COL) continue;

				if ((matriz[r][c] & 0x01) && !(matriz[r][c] & 0x02)) // Gestion de pulsado
				{					
					if (!antighosting)
					{
						if (matriz[CAPS_SHIFT_ROW][CAPS_SHIFT_COL] & 0x01) matriz[r][c] |= 0x08; // Se marca capsshift
						if (matriz[SYMBOL_SHIFT_ROW][SYMBOL_SHIFT_COL] & 0x01) matriz[r][c] |= 0x10; // Se marca symbolshift						
					}					

					if ((matriz[r][c] & 0x08) && !(matriz[r][c] & 0x10)) pulsateclaconshift(r, c, codeset == 2 ? mapEXT[r][c] : mapEXT1[r][c]); // Pulsar con capsshift
					if ((matriz[r][c] & 0x10) && !(matriz[r][c] & 0x08)) pulsateclaconsymbol(r, c, modo); // Pulsar con symbolshift
					if (!(matriz[r][c] & 0x08) && !(matriz[r][c] & 0x10))
					{
						
						if (opqa_cursors)
						{							
							typematicfirst = 0;
							typematic_codeaux = 0;
							
							if (codeset == 2)
							{
								keyaux = mapZX[r][c];
								keyaux = keyaux == KEY_O ? KEY_LEFT : keyaux;
								keyaux = keyaux == KEY_P ? KEY_RIGHT : keyaux;
								keyaux = keyaux == KEY_Q ? KEY_UP : keyaux;
								keyaux = keyaux == KEY_A ? KEY_DOWN : keyaux;
								if (keyaux == KEY_LEFT || keyaux == KEY_RIGHT || keyaux == KEY_UP || keyaux == KEY_DOWN) typematic_codeaux = 0xE0; //Es una tecla del codeset2 que necesita E0
							}
							if (codeset == 1)
							{
								keyaux = mapSET1[r][c];
								keyaux = keyaux == KS1_O ? KS1_LEFT : keyaux;
								keyaux = keyaux == KS1_P ? KS1_RIGHT : keyaux;
								keyaux = keyaux == KS1_Q ? KS1_UP : keyaux;
								keyaux = keyaux == KS1_A ? KS1_DOWN : keyaux;
								if (keyaux == KS1_LEFT || keyaux == KS1_RIGHT || keyaux == KS1_UP || keyaux == KS1_DOWN) typematic_codeaux = 0xE0; //Es una tecla del codeset1 que necesita E0
							}							
																
							typematic_code = keyaux;							
							if (typematic_codeaux > 0) sendPS2(0xE0);
							sendPS2(typematic_code);
						}
						else
						{
							typematicfirst = 0;
							if (codeset == 2) typematic_code = mapZX[r][c];
							else typematic_code = mapSET1[r][c]; // Pulsar sin modificadores
														
							typematic_codeaux = 0;
							sendPS2(typematic_code);
						}
					}
					// Si estan pulsados capsshift y symbolshift, no hacemos nada

					matriz[r][c] |= 0x02; // Se marca mantenida
				}

				if ((matriz[r][c] & 0x04)) // Gestion de liberado
				{
					if ((matriz[r][c] & 0x08) && !(matriz[r][c] & 0x10)) sueltateclaconshift(r, c, codeset == 2 ? mapEXT[r][c] : mapEXT1[r][c]); // Liberar con capsshift					
					if ((matriz[r][c] & 0x10) && !(matriz[r][c] & 0x08)) sueltateclaconsymbol(r, c, modo); // Liberar con symbolshift					
					if (!(matriz[r][c] & 0x08) && !(matriz[r][c] & 0x10))
					{
						if (opqa_cursors)
						{
							typematic_code = 0;
							
							if (codeset == 2)
							{					
								keyaux = mapZX[r][c];
								keyaux = keyaux == KEY_O ? KEY_LEFT : keyaux;
								keyaux = keyaux == KEY_P ? KEY_RIGHT : keyaux;
								keyaux = keyaux == KEY_Q ? KEY_UP : keyaux;
								keyaux = keyaux == KEY_A ? KEY_DOWN : keyaux;

								if (keyaux == KEY_LEFT || keyaux == KEY_RIGHT || keyaux == KEY_UP || keyaux == KEY_DOWN) sendPS2(0xE0); //Es una tecla del codeset2 que necesita E0
								sendPS2(0xF0); sendPS2(keyaux);																								
							}
							if (codeset == 1)
							{
								keyaux = mapSET1[r][c];
								keyaux = keyaux == KS1_O ? KS1_LEFT : keyaux;
								keyaux = keyaux == KS1_P ? KS1_RIGHT : keyaux;
								keyaux = keyaux == KS1_Q ? KS1_UP : keyaux;
								keyaux = keyaux == KS1_A ? KS1_DOWN : keyaux;
								if (keyaux == KS1_LEFT || keyaux == KS1_RIGHT || keyaux == KS1_UP || keyaux == KS1_DOWN) sendPS2(0xE0); //Es una tecla del codeset1 que necesita E0
								sendPS2(keyaux + KS1_RELEASE);
							}
						}
						else
						{
							if (codeset == 2) { sendPS2(0xF0); sendPS2(mapZX[r][c]); typematic_code = typematic_code == mapZX[r][c] ? 0 : typematic_code; }
							else { sendPS2(mapSET1[r][c] + KS1_RELEASE); typematic_code = typematic_code == mapSET1[r][c] ? 0 : typematic_code; } // Liberar sin modificadores
						}
					}
					matriz[r][c] = 0; // Fin de gestion de la tecla

				}
			}									

		}//Fin del If/else manejo de modo ZX u otros Keymaps
	}//Fin del If del control del teclado.
}//FIN de Matrixscan

void setup()
{
  CPU_PRESCALE(CPU_16MHz);
  ps2Init();
  matrixInit();
  const uint8_t ZXUNO_SIGNATURE[] = { 'Z','X','U','N','O' };
  uint8_t checksignature[5];
  uint8_t issigned = 1;
  eeprom_read_block((void*)&checksignature, (const void*)0, 5);
    
  for (int n = 0; n < 5; n++) if (checksignature[n] != ZXUNO_SIGNATURE[n]) issigned = 0;
  if (issigned)
  {
    modo = cambiarmodo2(eeprom_read_byte((uint8_t*)5));   
	fkbmode = eeprom_read_byte((uint8_t*)6);
	fkbmode = fkbmode > 2 ? 0 : fkbmode;
  }
  else
  {
    eeprom_write_block((void*)&ZXUNO_SIGNATURE, (const void*)0, 5); // Guardamos la firma
    eeprom_write_byte((uint8_t*)5, (uint8_t)0); // y modo ZX por defecto
  }  
}

void loop()
{
        if (ps2Stat() && modo == MAXKB && (kbescucha || timeout_escucha > 0)) // Lineas CLK y/o DATA a 0 y escucha activa
      {                      // Solo hay escucha activa en modo PC, hasta su inicializacion.
                               // Una vez completada la inicializacion de teclado, no es necesario mantener activa la escucha de comandos excepto si se hace eco
                         
    
      while (checkState(1000)) // tramos de 5 us (5000 us)
      {
        hostdataAnt = hostdata;
        if (getPS2(&hostdata) == 0)
        {
                timeout_escucha = 100000;   // Dejamos tiempo para que se complete la inicializacion
          if (hostdata == 0xEE)
          {
            sendPS2(0xEE); // Echo
            kbescucha = 1; // Si se hace eco, mantenemos la escucha de comandos (necesario en determinados adaptadores PS/2 -> USB)
          }
          else
          {
            sendPS2(0xFA); // Ack
          }
          switch (hostdata)
          {
          case 0x00: // second bit of 0xED or 0xF3 (or get scancode set)  
            if (hostdataAnt == 0xF0)
            {
              sendPS2(codeset);
            }
            break;
          case 0x01: // set scancode 1
            if (hostdataAnt == 0xF0)
            {
              codeset = 1;
            }
            break;
          case 0x02: // set scancode 2
            if (hostdataAnt == 0xF0)
            {
              codeset = 2;
            }
            break;
          case 0xED: // set/reset LEDs
            break;
          case 0xF2: // ID          
            sendPS2(0xAB);
            sendPS2(0x83);
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
            sendPS2(0xAA); 
                  kbescucha = 0; // En el inicio iniciamos la cuenta atras de timeout
            break;
          default:
            break;
          } //Fin del Swich
        } //Fin del IF de si detecta dato     
      } //Fin del while que chequea el estado

      }
    else
    {
      if (timeout_escucha > 0) timeout_escucha--;
      if (modo == MAXKB && typematic_code != 0 && (typematicfirst++ > 1000 || codeset == 2) && typematic++ > 150) // Funcion tipematica simulada para PC
      {
        if (typematic_codeaux != 0) sendPS2(typematic_codeaux);
        sendPS2(typematic_code); typematic = 0;
      }
      matrixScan(); //No llegan datos del Host, enviamos teclado.
    }
  
}

