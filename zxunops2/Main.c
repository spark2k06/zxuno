/*
Conversor teclado ZX-spectrum 8x5 -> PS/2 de Neuro (Codigo original de Quest) (Comunicacion bidireccional PS2 y correcciones de protocolo de Spark2k06)
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
#include "keymaps.h"

#define LED_ON	PORTB |= (1<<5)		
#define LED_OFF	PORTB &= ~(1<<5)
#define LED_CONFIG	DDRB |= (1<<5) //Led en PB5 en Pro mini y similares

////////Pro Mini
#define PS2_PORT	PORTC
#define PS2_DDR		DDRC
#define PS2_PIN		PINC

#define PS2_DAT		PC4
#define PS2_CLK		PC5

#define HI 1
#define LO 0
#define _IN 1
#define _OUT 0

#define CPU_PRESCALE(n) (CLKPR = 0x80, CLKPR = (n))
#define CPU_16MHz       0x00

//Definicion de pines y puertos en arrays

///////////Pro Mini
//{PB2=10, PB3=11, PB4=12, PC0=A0, PC1=A1};
uint8_t pinsC[COLS] = { 2, 3, 4, 0, 1 };
uint8_t bcdC[COLS] = { 2, 2, 2, 3, 3 };

//{PD2=2, PD3=3, PD4=4, PD5=5, PD6=6, PD7=7, PB0=8, PB1=9};
uint8_t pinsR[ROWS] = { 2, 3, 4, 5, 6, 7, 0, 1 };
uint8_t bcdR[ROWS] = { 4, 4, 4, 4, 4, 4, 2, 2 };

//Swiches {PC2=A2 / PC3=A3}; 
uint8_t pinsSW[NUMSW] = { 2, 3 };
uint8_t bcdSW[NUMSW] = { 3, 3 };

//Teclas Pulsadas en el ultimo pase
uint8_t matriz[ROWS][COLS] = { { 0,0,0,0,0 },{ 0,0,0,0,0 },{ 0,0,0,0,0 },{ 0,0,0,0,0 },{ 0,0,0,0,0 },{ 0,0,0,0,0 },{ 0,0,0,0,0 },{ 0,0,0,0,0 } }; //0=Sin pulsar, 1=Marcada para Soltar, 2=Recien Pulsado, 3=Mantenido
uint8_t botones[NUMSW] = { 0,0 };

unsigned char espera = 0;
unsigned char fnpulsada = 0;
unsigned char fnpulsando = 0;

//uint8_t modo=0; //Modo teclado 0=ZX NATIVO / 1=CPC MAPEADO /2=MSX MAPEADO
enum KBMODE modo; //Modo teclado 0=ZX=NATIVO /Resto otros mapas
uint8_t cambiomodo = 0;
uint8_t soltarteclas;
uint8_t shiftmod;
uint8_t symbolmod;

unsigned char	hostdata = 0;
unsigned char	hostdataAnt;
unsigned char	codeset = 2;

//Teclas Modificadoras (para teclado spectrum)
unsigned char CAPS_SHIFT = KEY_LSHIFT;  //Caps Shift   (NO necesita E0)
unsigned char SYMBOL_SHIFT = KEY_LCTRL; //Symbol Shift (NO necesita E0)

										//Caps Shift (CAPS_SHIFT)
#define CAPS_SHIFT_ROW 5  
#define CAPS_SHIFT_COL 4  

										//Symbol Shift (SYMBOL_SHIFT)
#define SYMBOL_SHIFT_ROW 7   
#define SYMBOL_SHIFT_COL 3   

										//ENTER
#define ENTER_ROW 6
#define ENTER_COL 4 

										//SPACE (Escape)
#define SPACE_ROW 7 
#define SPACE_COL 4 

										//Row 1..5
#define N1_N5_ROW 0
										//Cols 1..5
#define N1_COL 4 //
#define N2_COL 3 //
#define N3_COL 2 //
#define N4_COL 1 //
#define N5_COL 0 //

										//Row 6..0
#define N6_N0_ROW 3
										//Cols 6..0
#define N6_COL 0 //
#define N7_COL 1 //
#define N8_COL 2 //
#define N9_COL 3 //
#define N0_COL 4 //

										//Row Q-T
#define Q_T_ROW 1
										//Cols Q-T
#define Q_COL 4 //
#define W_COL 3 //
#define E_COL 2 //
#define R_COL 1 //
#define T_COL 0 //

										//Row Y-P
#define Y_P_ROW 4
										//Cols Y-P
#define Y_COL 0 //
#define U_COL 1 //
#define I_COL 2 //
#define O_COL 3 //
#define P_COL 4 //

										//Row A-G
#define A_G_ROW 2
										//Cols A-G
#define A_COL 4 //
#define S_COL 3 //
#define D_COL 2 //
#define F_COL 1 //
#define G_COL 0 //

										//Row H-L
#define H_L_ROW 6
										//Cols H-L
#define H_COL 0 //
#define J_COL 1 //
#define K_COL 2 //
#define L_COL 3 //

										//Row Z-V
#define Z_V_ROW 5 //
										//Cols Z-V
#define Z_COL 3 //
#define X_COL 2 //
#define C_COL 1 //
#define V_COL 0 //

										//Row B-M
#define B_M_ROW 7
										//Cols B-M
#define B_COL 0 //
#define N_COL 1 //
#define M_COL 2 //
#define SP_COL 4 //


void leds_debug() {
	LED_ON;
	_delay_ms(60);
	LED_OFF;
	_delay_ms(30);
}

void pinSet(pin, bcd, stat) //stat 1 = in, stat 0 = out
{
	switch (bcd) {
	case 2:  if (stat) DDRB &= ~_BV(pin); else DDRB |= _BV(pin); break;
	case 3:  if (stat) DDRC &= ~_BV(pin); else DDRC |= _BV(pin); break;
	case 4:  if (stat) DDRD &= ~_BV(pin); else DDRD |= _BV(pin); break;
	}
}

uint8_t pinStat(pin, bcd)
{
	switch (bcd) {
	case 2:  if (!(PINB & (1 << pin))) return 1; else return 0; break;
	case 3:  if (!(PINC & (1 << pin))) return 1; else return 0; break;
	case 4:  if (!(PIND & (1 << pin))) return 1; else return 0; break;
	}
	return 0;
}

void pinPut(pin, bcd, stat) //stat 1 = HI, stat 0 = LO
{
	switch (bcd) {
	case 2:  if (!stat) PORTB &= ~_BV(pin); else PORTB |= _BV(pin); break;
	case 3:  if (!stat) PORTC &= ~_BV(pin); else PORTC |= _BV(pin); break;
	case 4:  if (!stat) PORTD &= ~_BV(pin); else PORTD |= _BV(pin); break;
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
uint8_t KBp = 0;  //Pausa en ms entre teclas cuando se pulsan con shift o symbol en modo > 0
uint8_t KBc = 0;  //Pausa en ms entre el uso de los cursores
uint8_t KBn = 0;  //Pausa en ms entre las teclas normales

				  //envio de datos ps/2 simulando reloj con delays.
void sendPS2(unsigned char code)
{
	//Para continuar las líneas deben estar en alto
	//if (ps2Stat())
	//	return;   
	while (ps2Stat());

	unsigned char parity = 1;
	uint8_t i = 0;

	//iniciamos transmisión
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
	char pausa = 25;
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
	KBp = nomcore[nomcore[0] + 2]; //Valor de KBp en la configuracion de nomcore[]
	KBc = nomcore[nomcore[0] + 3]; //Valor de KBc en la configuracion de nomcore[]
	KBn = nomcore[nomcore[0] + 4]; //Valor de KBn en la configuracion de nomcore[]
}

//Inicializar Matriz
void matrixInit()
{
	uint8_t c, r;

	LED_CONFIG;
	LED_OFF;

	for (c = 0; c<NUMSW; c++)
	{
		pinSet(pinsSW[c], bcdSW[c], _IN);
		pinPut(pinsSW[c], bcdSW[c], HI);
	}

	for (c = 0; c<COLS; c++)
	{
		pinSet(pinsC[c], bcdC[c], _IN);
		pinPut(pinsC[c], bcdC[c], HI);
	}

	for (r = 0; r<ROWS; r++)
		pinSet(pinsR[r], bcdR[r], _IN);
}

enum KBMODE cambiarmodo(enum KBMODE modokb)
{   //Quita la pulsacoin de Caps Shift o Symbol Shift. Si se pasa de modo 0 a otro se sueltan las teclas caps y symbol.
	//	if(matriz[CAPS_SHIFT_ROW][CAPS_SHIFT_COL]==3)     {if(!modo) {sendPS2(0xF0); sendPS2(CAPS_SHIFT); espera++;}   matriz[CAPS_SHIFT_ROW][CAPS_SHIFT_COL]=0;} else if(matriz[CAPS_SHIFT_ROW][CAPS_SHIFT_COL]==2)     matriz[CAPS_SHIFT_ROW][CAPS_SHIFT_COL]=0;
	//	if(matriz[SYMBOL_SHIFT_ROW][SYMBOL_SHIFT_COL]==3) {if(!modo) {sendPS2(0xF0); sendPS2(SYMBOL_SHIFT); espera++;} matriz[SYMBOL_SHIFT_ROW][SYMBOL_SHIFT_COL]=0;} else if(matriz[SYMBOL_SHIFT_ROW][SYMBOL_SHIFT_COL]==2) matriz[SYMBOL_SHIFT_ROW][SYMBOL_SHIFT_COL]=0;
	//	if(modo){ shiftmod=0; symbolmod=0;} //Quita las marcas de shift y symbol al cambiar de modo>0 si estan puestas.


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
	if (modokb == xt1) imprimecore(nomXT1);

	//Uso normal: CK1 = 20, CK2 = 40 // Para codigo sin optimizar (x12) CK1 = 240, CK2 = 480.  //JOyPs2 CK1=15 CK2=30 //Mio CK1=4 CK2=8
	//if(modokb>0) CKm=4; else CKm=1; //Se coge del Nombrecore[]
	if (modokb<MAXKB) codeset = 2;
	fnpulsada = 1;
	fnpulsando = 1;
	cambiomodo = 0; //para salir del bucle del cambiomodo
	LED_OFF; //Apaga el led de control (salida de bucle de cambio de modo)
	return modokb;
}

void pulsafn(unsigned char row, unsigned char col, unsigned char key, unsigned char key_E0, unsigned char shift, unsigned char ctrl, unsigned char alt, unsigned char useg)
{ //Quita la pulsacoin de Caps Shift o Symbol Shift. Si se pasa de modo 0 a otro se sueltan las teclas caps y symbol.
	if (matriz[CAPS_SHIFT_ROW][CAPS_SHIFT_COL] == 3) { if (!modo) { sendPS2(0xF0); sendPS2(CAPS_SHIFT); espera++; }   matriz[CAPS_SHIFT_ROW][CAPS_SHIFT_COL] = 0; }
	else if (matriz[CAPS_SHIFT_ROW][CAPS_SHIFT_COL] == 2)     matriz[CAPS_SHIFT_ROW][CAPS_SHIFT_COL] = 0;
	if (matriz[SYMBOL_SHIFT_ROW][SYMBOL_SHIFT_COL] == 3) { if (!modo) { sendPS2(0xF0); sendPS2(SYMBOL_SHIFT); espera++; } matriz[SYMBOL_SHIFT_ROW][SYMBOL_SHIFT_COL] = 0; }
	else if (matriz[SYMBOL_SHIFT_ROW][SYMBOL_SHIFT_COL] == 2) matriz[SYMBOL_SHIFT_ROW][SYMBOL_SHIFT_COL] = 0;
	if (modo) { shiftmod = 0; symbolmod = 0; } //Quita las marcas de shift y symbol al hacer un combo en modo>0 si estan puestas.

	if (espera) { _delay_us(5); espera = 0; }
	if (shift) { if (codeset == 2) { sendPS2(KEY_RSHIFT); espera++; } else { sendPS2(KS1_RSHIFT); espera++; } }//El Shift derecho no necesita E0
	if (ctrl) { if (codeset == 2) { sendPS2(0xE0); sendPS2(KEY_RCTRL);  espera++; } else { sendPS2(0xE0); sendPS2(KS1_RCTRL);  espera++; } }//Se manda E0 para el control derecho (que vale para ambos casos)
	if (alt) { if (codeset == 2) { sendPS2(KEY_LALT);   espera++; } else { sendPS2(KS1_LALT);   espera++; } }//Usamos el Alt izdo siempre
	if (espera) { _delay_us(5); espera = 0; }
	if (key_E0) { sendPS2(0xE0); } //La tecla requiere modo E0 del PS2
	sendPS2(key);
	//_delay_us(useg); Muy poco margen para la liberacion del scancode
	_delay_ms(50);
	if (key_E0) { sendPS2(0xE0); }
	if (codeset == 2) sendPS2(0xF0);
	if (codeset == 2) sendPS2(key);  else sendPS2(key + KS1_RELEASE);
	matriz[row][col] = 0;
	if (shift) { if (codeset == 2) { sendPS2(0xF0); sendPS2(KEY_RSHIFT); } else { sendPS2(KS1_RSHIFT + KS1_RELEASE); } }
	if (ctrl) { if (codeset == 2) { sendPS2(0xE0); sendPS2(0xF0); sendPS2(KEY_RCTRL); } else { sendPS2(0xE0); sendPS2(KS1_RCTRL + KS1_RELEASE); } }
	if (alt) { if (codeset == 2) { sendPS2(0xF0); sendPS2(KEY_LALT); } else { sendPS2(KS1_RALT + KS1_RELEASE); } }
	_delay_us(5);
	fnpulsada = 1;
	fnpulsando = 1;
}

/*
void pulsaysueltateclaconsymbol(unsigned char row, unsigned char col, enum KBMODE modokb)
{
unsigned char key=0, shift=0;

if(modokb==cpc) {key=mapCPC[row][col]; shift=modCPC[row][col];}
if(modokb==msx) {key=mapMSX[row][col]; shift=modMSX[row][col];}
if(modokb==c64) {key=mapC64[row][col]; shift=modC64[row][col];}
if(modokb==at8) {key=mapAT8[row][col]; shift=modAT8[row][col];}
if(modokb==bbc) {key=mapBBC[row][col]; shift=modBBC[row][col];}
if(modokb==aco) {key=mapACO[row][col]; shift=modACO[row][col];}
if(modokb==ap2) {key=mapAP2[row][col]; shift=modAP2[row][col];}
if(modokb==vic) {key=mapVIC[row][col]; shift=modVIC[row][col];}
if(modokb==ori) {key=mapORI[row][col]; shift=modORI[row][col];}
if(modokb==sam) {key=mapSAM[row][col]; shift=modSAM[row][col];}
if(modokb==jup) {key=mapJUP[row][col]; shift=modJUP[row][col];}
if(modokb==xt1) {key=mapXT1[row][col]; shift=modXT1[row][col];}

if(shift) {if(codeset==2) sendPS2(KEY_RSHIFT); else sendPS2(KS1_RSHIFT); }// _delay_us(5);}
sendPS2(key); _delay_us(50);
if(codeset==2) {sendPS2(0xF0);sendPS2(key);} else sendPS2(key+KS1_RELEASE);
if(shift) {if(codeset==2) {sendPS2(0xF0); sendPS2(KEY_RSHIFT);} else sendPS2(KS1_RSHIFT+KS1_RELEASE); }// _delay_us(5);}
_delay_ms(KBp); //Pequeña pausa para evitar la "super-repeticion-atodapastilla"
matriz[row][col]=0;
}
*/
void pulsateclaconsymbol(unsigned char row, unsigned char col, enum KBMODE modokb)
{
	unsigned char key = 0, shift = 0;

	if (modokb == cpc) { key = mapCPC[row][col]; shift = modCPC[row][col]; }
	if (modokb == msx) { key = mapMSX[row][col]; shift = modMSX[row][col]; }
	if (modokb == c64) { key = mapC64[row][col]; shift = modC64[row][col]; }
	if (modokb == at8) { key = mapAT8[row][col]; shift = modAT8[row][col]; }
	if (modokb == bbc) { key = mapBBC[row][col]; shift = modBBC[row][col]; }
	if (modokb == aco) { key = mapACO[row][col]; shift = modACO[row][col]; }
	if (modokb == ap2) { key = mapAP2[row][col]; shift = modAP2[row][col]; }
	if (modokb == vic) { key = mapVIC[row][col]; shift = modVIC[row][col]; }
	if (modokb == ori) { key = mapORI[row][col]; shift = modORI[row][col]; }
	if (modokb == sam) { key = mapSAM[row][col]; shift = modSAM[row][col]; }
	if (modokb == jup) { key = mapJUP[row][col]; shift = modJUP[row][col]; }
	if (modokb == xt1) { key = mapXT1[row][col]; shift = modXT1[row][col]; }

	if (shift) { if (codeset == 2) sendPS2(KEY_RSHIFT); else sendPS2(KS1_RSHIFT); }
	sendPS2(key);
	matriz[row][col] = 3;
}
void sueltateclaconsymbol(unsigned char row, unsigned char col, enum KBMODE modokb)
{
unsigned char key=0, shift=0;

if(modokb==cpc) {key=mapCPC[row][col]; shift=modCPC[row][col];}
if(modokb==msx) {key=mapMSX[row][col]; shift=modMSX[row][col];}
if(modokb==c64) {key=mapC64[row][col]; shift=modC64[row][col];}
if(modokb==at8) {key=mapAT8[row][col]; shift=modAT8[row][col];}
if(modokb==bbc) {key=mapBBC[row][col]; shift=modBBC[row][col];}
if(modokb==aco) {key=mapACO[row][col]; shift=modACO[row][col];}
if(modokb==ap2) {key=mapAP2[row][col]; shift=modAP2[row][col];}
if(modokb==vic) {key=mapVIC[row][col]; shift=modVIC[row][col];}
if(modokb==ori) {key=mapORI[row][col]; shift=modORI[row][col];}
if(modokb==sam) {key=mapSAM[row][col]; shift=modSAM[row][col];}
if(modokb==jup) {key=mapJUP[row][col]; shift=modJUP[row][col];}
if(modokb==xt1) {key=mapXT1[row][col]; shift=modXT1[row][col];}

if(codeset==2) {sendPS2(0xF0);sendPS2(key);} else sendPS2(key+KS1_RELEASE);
if(shift) {if(codeset==2) {sendPS2(0xF0); sendPS2(KEY_RSHIFT);} else sendPS2(KS1_RSHIFT+KS1_RELEASE); }
matriz[row][col]=0;
}
/*
void pulsaysueltateclaconshift(unsigned char row, unsigned char col, unsigned char key)
{
unsigned char cursores=0;
if(!key) //si no esta mapeada saca la mayuscula
{
if(codeset==2) sendPS2(KEY_RSHIFT); else sendPS2(KS1_RSHIFT);//_delay_us(5);
if(codeset==2) sendPS2(mapZX[row][col]); else sendPS2(mapSET1[row][col]);
_delay_us(5);
if(codeset==2) {sendPS2(0xF0); sendPS2(mapZX[row][col]); sendPS2(0xF0); sendPS2(KEY_RSHIFT); } //_delay_us(5);
else           {sendPS2(mapSET1[row][col]+KS1_RELEASE); sendPS2(KS1_RSHIFT+KS1_RELEASE);}
}else
{
if(codeset==2 && (key==KEY_LEFT || key==KEY_RIGHT || key==KEY_UP || key==KEY_DOWN)) {sendPS2(0xE0); cursores=1;} //Es una tecla del codeset2 que necesita E0
if(codeset==1 && (key==KS1_LEFT || key==KS1_RIGHT || key==KS1_UP || key==KS1_DOWN)) {sendPS2(0xE0); cursores=1;}//Es una tecla del codeset1 que necesita E0
sendPS2(key); _delay_us(5);
if(codeset==2 && (key==KEY_LEFT || key==KEY_RIGHT || key==KEY_UP || key==KEY_DOWN)) sendPS2(0xE0); //Es una tecla del codeset2 que necesita E0
if(codeset==1 && (key==KS1_LEFT || key==KS1_RIGHT || key==KS1_UP || key==KS1_DOWN)) sendPS2(0xE0); //Es una tecla del codeset1 que necesita E0
if(codeset==2) {sendPS2(0xF0); sendPS2(key);} else sendPS2(key+KS1_RELEASE); //_delay_us(5);
}
if(!cursores) _delay_ms(KBp); else if(KBc) _delay_ms(KBc);  //Pequeña pausa para evitar la "super-repeticion-atodapastilla"
matriz[row][col]=0;
}
*/
void pulsateclaconshift(unsigned char row, unsigned char col, unsigned char key)
{
	unsigned char cursores = 0;
	if (!key) //si no esta mapeada saca la mayuscula
	{
		if (codeset == 2) sendPS2(KEY_RSHIFT); else sendPS2(KS1_RSHIFT);
		if (codeset == 2) sendPS2(mapZX[row][col]); else sendPS2(mapSET1[row][col]);
	}
	else
	{
		if (codeset == 2 && (key == KEY_LEFT || key == KEY_RIGHT || key == KEY_UP || key == KEY_DOWN)) { sendPS2(0xE0); cursores = 1; } //Es una tecla del codeset2 que necesita E0
		if (codeset == 1 && (key == KS1_LEFT || key == KS1_RIGHT || key == KS1_UP || key == KS1_DOWN)) { sendPS2(0xE0); cursores = 1; }//Es una tecla del codeset1 que necesita E0
		sendPS2(key);
	}
	matriz[row][col] = 3;
}

void sueltateclaconshift(unsigned char row, unsigned char col, unsigned char key)
{
	unsigned char cursores = 0;
	if (!key) //si no esta mapeada saca la mayuscula
	{
		if (codeset == 2) { sendPS2(0xF0); sendPS2(mapZX[row][col]); sendPS2(0xF0); sendPS2(KEY_RSHIFT); }
		else { sendPS2(mapSET1[row][col] + KS1_RELEASE); sendPS2(KS1_RSHIFT + KS1_RELEASE); }
	}
	else
	{
		if (codeset == 2 && (key == KEY_LEFT || key == KEY_RIGHT || key == KEY_UP || key == KEY_DOWN)) sendPS2(0xE0); //Es una tecla del codeset2 que necesita E0
		if (codeset == 1 && (key == KS1_LEFT || key == KS1_RIGHT || key == KS1_UP || key == KS1_DOWN)) sendPS2(0xE0); //Es una tecla del codeset1 que necesita E0
		if (codeset == 2) { sendPS2(0xF0); sendPS2(key); }
		else sendPS2(key + KS1_RELEASE);
	}
	matriz[row][col] = 0;
}

void matrixScan()
{
	uint8_t r, c, b;

	fnpulsada = 0; //Se pone a 0 la pulsacion de una tecla de funcion

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
					if (matriz[r][c] == 2) { matriz[r][c] = 3; } //Pasa de pulsado a mantenido  "3"
					if (matriz[r][c] == 0) { matriz[r][c] = 2; } //Pasa de sin pulsar a pulsado "2"
				}
				else if (matriz[r][c] != 0) { matriz[r][c] = 1; } //Marcado para soltar la tecla "1" (si entra por debounce)
			}
			else if (matriz[r][c] != 0) { matriz[r][c] = 1; } //Marcado para soltar la tecla "1"
		}//Fin de Escaneo de las Columnas para el Row/Fila indicado
		 //desact. row/fila
		pinSet(pinsR[r], bcdR[r], _IN);
	} //fin escaneo de Rows/Filas de la matriz del teclado
	if (cambiomodo)
	{ //Primero verifica que se han soltado todas las teclas antes de pulsar la del modo que se desea.
		if (soltarteclas)
		{
			soltarteclas = 0;
			for (r = 0; r<ROWS; r++) for (c = 0; c<COLS; c++) if (matriz[r][c]>1) soltarteclas = 1;
		}
		if (!soltarteclas)
		{
			//if(matriz[CAPS_SHIFT_ROW][CAPS_SHIFT_COL]==3)     {if(!modo) {sendPS2(0xF0); sendPS2(CAPS_SHIFT); espera++;}   matriz[CAPS_SHIFT_ROW][CAPS_SHIFT_COL]=0;} else if(matriz[CAPS_SHIFT_ROW][CAPS_SHIFT_COL]==2)     matriz[CAPS_SHIFT_ROW][CAPS_SHIFT_COL]=0;
			//if(matriz[SYMBOL_SHIFT_ROW][SYMBOL_SHIFT_COL]==3) {if(!modo) {sendPS2(0xF0); sendPS2(SYMBOL_SHIFT); espera++;} matriz[SYMBOL_SHIFT_ROW][SYMBOL_SHIFT_COL]=0;} else if(matriz[SYMBOL_SHIFT_ROW][SYMBOL_SHIFT_COL]==2) matriz[SYMBOL_SHIFT_ROW][SYMBOL_SHIFT_COL]=0;
			if (matriz[CAPS_SHIFT_ROW][CAPS_SHIFT_COL] == 1) { if (!modo) { sendPS2(0xF0); sendPS2(CAPS_SHIFT); matriz[CAPS_SHIFT_ROW][CAPS_SHIFT_COL] = 0; } }
			if (matriz[SYMBOL_SHIFT_ROW][SYMBOL_SHIFT_COL] == 1) { if (!modo) { sendPS2(0xF0); sendPS2(SYMBOL_SHIFT); matriz[SYMBOL_SHIFT_ROW][SYMBOL_SHIFT_COL] = 0; } }
			if (modo) { shiftmod = 0; symbolmod = 0; } //Quita las marcas de shift y symbol al cambiar de modo>0 si estan puestas.
			for (r = 0; r<ROWS; r++) for (c = 0; c<COLS; c++) if (matriz[r][c]>1) modo = cambiarmodo(mapMODO[r][c]);
		}
		if (cambiomodo) fnpulsada = 1; //Si no se pulsa ninguna tecla sigue en bucle hasta que se pulse
	}
	//Comprobacion de Teclas especiales al tener pulsado Caps Shift y Symbol Shift
	if (!fnpulsada && matriz[CAPS_SHIFT_ROW][CAPS_SHIFT_COL]>1 && matriz[SYMBOL_SHIFT_ROW][SYMBOL_SHIFT_COL]>1)
	{
		if (!fnpulsando)
		{//row, col, key, key_E0, shift, ctrl, alt, museg
			if (codeset == 2)
			{
				//if(matriz[Y_P_ROW][U_COL]>1) {if(modo<MAXKB)modo=cambiarmodo(modo+1); else modo=cambiarmodo(0);}    //Cambio de Modo de Teclado entre 0=ZX, y el resto
				if (matriz[Y_P_ROW][U_COL]>1)
				{
					cambiomodo = 1; fnpulsada = 1; soltarteclas = 1; LED_ON;
					//_delay_ms(100);sendPS2(KEY_SPACE);_delay_ms(100); sendPS2(0xF0);sendPS2(KEY_SPACE);
					//_delay_ms(100);sendPS2(KEY_PUNTO);_delay_ms(100); sendPS2(0xF0);sendPS2(KEY_PUNTO);	 
				} //Activa el cambio de modo lo que dejara en bucle hasta que se pulse una tecla. El led se enciende.
				if (matriz[N1_N5_ROW][N1_COL]>1) pulsafn(N1_N5_ROW, N1_COL, KEY_F1, 0, 0, 0, 0, 5);  //F1
				if (matriz[N1_N5_ROW][N2_COL]>1) pulsafn(N1_N5_ROW, N2_COL, KEY_F2, 0, 0, 0, 0, 5);  //F2
				if (matriz[N1_N5_ROW][N3_COL]>1) pulsafn(N1_N5_ROW, N3_COL, KEY_F3, 0, 0, 0, 0, 5);  //F3
				if (matriz[N1_N5_ROW][N4_COL]>1) pulsafn(N1_N5_ROW, N4_COL, KEY_F4, 0, 0, 0, 0, 5);  //F4
				if (matriz[N1_N5_ROW][N5_COL]>1) pulsafn(N1_N5_ROW, N5_COL, KEY_F5, 0, 0, 0, 0, 5);  //F5
				if (matriz[N6_N0_ROW][N6_COL]>1) pulsafn(N6_N0_ROW, N6_COL, KEY_F6, 0, 0, 0, 0, 5);  //F6 
				if (matriz[N6_N0_ROW][N7_COL]>1) pulsafn(N6_N0_ROW, N7_COL, KEY_F7, 0, 0, 0, 0, 5);  //F7
				if (matriz[N6_N0_ROW][N8_COL]>1) pulsafn(N6_N0_ROW, N8_COL, KEY_F8, 0, 0, 0, 0, 5);  //F8
				if (matriz[N6_N0_ROW][N9_COL]>1) pulsafn(N6_N0_ROW, N9_COL, KEY_F9, 0, 0, 0, 0, 5);  //F9
				if (matriz[N6_N0_ROW][N0_COL]>1) pulsafn(N6_N0_ROW, N0_COL, KEY_F10, 0, 0, 0, 0, 5); //F10
				if (matriz[Q_T_ROW][Q_COL]>1 && modo) pulsafn(Q_T_ROW, Q_COL, KEY_F11, 0, 0, 0, 0, 50); //F11  
				if (matriz[Q_T_ROW][W_COL]>1 && modo) pulsafn(Q_T_ROW, W_COL, KEY_F12, 0, 0, 0, 0, 50); //F12  

				if (matriz[A_G_ROW][A_COL]>1) pulsafn(A_G_ROW, A_COL, KEY_F10, 0, 0, 0, 0, 5);       //F10 para el NEXT (¿Mejor cambiar a otra?)

				if (matriz[Y_P_ROW][Y_COL]>1) pulsafn(Y_P_ROW, Y_COL, KEY_F5, 0, 0, 1, 1, 5);        //ZXUNO NMI (Control+Alt+F5)
				if (matriz[B_M_ROW][B_COL]>1) pulsafn(B_M_ROW, B_COL, KEY_BACKSP, 0, 0, 1, 1, 5);    //ZXUNO Hard Reset (Control+Alt+Backsp)
				if (matriz[B_M_ROW][N_COL]>1) pulsafn(B_M_ROW, N_COL, KEY_DELETE, 0, 0, 1, 1, 5);    //ZXUNO Soft Reset (Control+Alt+Supr)
				if (matriz[A_G_ROW][G_COL]>1) pulsafn(A_G_ROW, G_COL, KEY_SCRLCK, 0, 0, 0, 0, 5);    //ZXUNO RGB/VGA Swich (Bloq Despl)

				if (matriz[Z_V_ROW][V_COL]>1 && modo) imprimeversion();

				if (matriz[Q_T_ROW][E_COL]>1 && modo) pulsafn(Q_T_ROW, E_COL, KEY_PGUP, 1, 0, 0, 0, 5); //Re Pag / Pg Up   (Acorn: VGA) (C64 Disco Anterior)
				if (matriz[Q_T_ROW][R_COL]>1 && modo) pulsafn(Q_T_ROW, R_COL, KEY_PGDW, 1, 0, 0, 0, 5); //Av Pag / Pg Down (Acorn: RGB) (C64 Disco Siguiente)
				if (matriz[A_G_ROW][D_COL]>1 && modo) pulsafn(A_G_ROW, D_COL, KEY_PGUP, 1, 1, 0, 0, 5); //Re Pag / Pg Up   + Shift (C64 10 Discos Anteriores)
				if (matriz[A_G_ROW][F_COL]>1 && modo) pulsafn(A_G_ROW, F_COL, KEY_PGDW, 1, 1, 0, 0, 5); //Av Pag / Pg Down + Shift (C64 10 Discos Siguientes)
			}
			else if (codeset == 1)
			{
				if (matriz[N1_N5_ROW][N1_COL]>1) pulsafn(N1_N5_ROW, N1_COL, KS1_F1, 0, 0, 0, 0, 5);  //F1
				if (matriz[N1_N5_ROW][N2_COL]>1) pulsafn(N1_N5_ROW, N2_COL, KS1_F2, 0, 0, 0, 0, 5);  //F2
				if (matriz[N1_N5_ROW][N3_COL]>1) pulsafn(N1_N5_ROW, N3_COL, KS1_F3, 0, 0, 0, 0, 5);  //F3
				if (matriz[N1_N5_ROW][N4_COL]>1) pulsafn(N1_N5_ROW, N4_COL, KS1_F4, 0, 0, 0, 0, 5);  //F4
				if (matriz[N1_N5_ROW][N5_COL]>1) pulsafn(N1_N5_ROW, N5_COL, KS1_F5, 0, 0, 0, 0, 5);  //F5
				if (matriz[N6_N0_ROW][N6_COL]>1) pulsafn(N6_N0_ROW, N6_COL, KS1_F6, 0, 0, 0, 0, 5);  //F6 
				if (matriz[N6_N0_ROW][N7_COL]>1) pulsafn(N6_N0_ROW, N7_COL, KS1_F7, 0, 0, 0, 0, 5);  //F7
				if (matriz[N6_N0_ROW][N8_COL]>1) pulsafn(N6_N0_ROW, N8_COL, KS1_F8, 0, 0, 0, 0, 5);  //F8
				if (matriz[N6_N0_ROW][N9_COL]>1) pulsafn(N6_N0_ROW, N9_COL, KS1_F9, 0, 0, 0, 0, 5);  //F9
				if (matriz[N6_N0_ROW][N0_COL]>1) pulsafn(N6_N0_ROW, N0_COL, KS1_F10, 0, 0, 0, 0, 5); //F10
				if (matriz[Q_T_ROW][Q_COL]>1) pulsafn(Q_T_ROW, Q_COL, KS1_F11, 0, 0, 0, 0, 5); //F11  
				if (matriz[Q_T_ROW][W_COL]>1) pulsafn(Q_T_ROW, W_COL, KS1_F12, 0, 0, 0, 0, 5); //F12  
				if (matriz[B_M_ROW][B_COL]>1) pulsafn(B_M_ROW, B_COL, KS1_BACKSP, 0, 0, 1, 1, 5);    //ZXUNO Hard Reset (Control+Alt+Backsp)
				if (matriz[B_M_ROW][N_COL]>1) pulsafn(B_M_ROW, N_COL, KS1_DELETE, 0, 0, 1, 1, 5);    //ZXUNO Soft Reset (Control+Alt+Supr)
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
			if (matriz[CAPS_SHIFT_ROW][CAPS_SHIFT_COL] == 2) { sendPS2(CAPS_SHIFT);    matriz[CAPS_SHIFT_ROW][CAPS_SHIFT_COL] = 3;     espera++; }
			if (matriz[SYMBOL_SHIFT_ROW][SYMBOL_SHIFT_COL] == 2) { sendPS2(SYMBOL_SHIFT);  matriz[SYMBOL_SHIFT_ROW][SYMBOL_SHIFT_COL] = 3; espera++; }
			if (matriz[CAPS_SHIFT_ROW][CAPS_SHIFT_COL] == 1) { sendPS2(0xF0); sendPS2(CAPS_SHIFT);    matriz[CAPS_SHIFT_ROW][CAPS_SHIFT_COL] = 0;     espera++; }
			if (matriz[SYMBOL_SHIFT_ROW][SYMBOL_SHIFT_COL] == 1) { sendPS2(0xF0); sendPS2(SYMBOL_SHIFT);  matriz[SYMBOL_SHIFT_ROW][SYMBOL_SHIFT_COL] = 0; espera++; }
			if (espera) { _delay_us(5); espera = 0; }
			//Enviar el resto de Teclas Pulsadas, nunca entrara por shift o symbol de matrix[r][c] ya que se cambia el valor en el bloque anterior a 3 o 0
			for (r = 0; r<ROWS; r++) for (c = 0; c<COLS; c++)
			{
				if (matriz[r][c] == 2) { sendPS2(mapZX[r][c]); }
				if (matriz[r][c] == 1) { sendPS2(0xF0); sendPS2(mapZX[r][c]); matriz[r][c] = 0; }
			}
		}
		else //Manejo de los otros modos de Keymap
		{
			//Poner las marcas de Caps Shift y/o Symbol Shift si estamos en modo diferente al de ZX)
			if (matriz[CAPS_SHIFT_ROW][CAPS_SHIFT_COL] == 2) { shiftmod = 2;  matriz[CAPS_SHIFT_ROW][CAPS_SHIFT_COL] = 4; }
			if (matriz[SYMBOL_SHIFT_ROW][SYMBOL_SHIFT_COL] == 2) { symbolmod = 2; matriz[SYMBOL_SHIFT_ROW][SYMBOL_SHIFT_COL] = 4; }
			if (matriz[CAPS_SHIFT_ROW][CAPS_SHIFT_COL] == 1) { shiftmod = 1;  matriz[CAPS_SHIFT_ROW][CAPS_SHIFT_COL] = 0; }
			if (matriz[SYMBOL_SHIFT_ROW][SYMBOL_SHIFT_COL] == 1) { symbolmod = 1; matriz[SYMBOL_SHIFT_ROW][SYMBOL_SHIFT_COL] = 0; }
			if (symbolmod && shiftmod) { shiftmod = 0; symbolmod = 0; } //Si se pulsan symbol y caps shift a la vez en modo extendido, no vale para nada y los ponemos a 0.
																		//Enviar las teclas si no hay pulsado caps shift ni symbol shift
			if (!shiftmod && !symbolmod)
			{
				for (r = 0; r<ROWS; r++) for (c = 0; c<COLS; c++)
				{
					if (matriz[r][c] == 2) { if (codeset == 2) { sendPS2(mapZX[r][c]); _delay_ms(KBn); } else { sendPS2(mapSET1[r][c]); _delay_ms(KBn); } }
					if (matriz[r][c] == 1) { if (codeset == 2) { sendPS2(0xF0); sendPS2(mapZX[r][c]); _delay_ms(KBn); matriz[r][c] = 0; } else { sendPS2(mapSET1[r][c] + KS1_RELEASE); _delay_ms(KBn); matriz[r][c] = 0; } }
				}
			}
			//Si hay pulsado caps shift o symbol shift comprobamos por si Hay alguna otra tecla pulsada, si la hay, no obedecmos ni al shift ni al symbol.
			if (shiftmod == 2 || symbolmod == 2)
			{
				unsigned char sinteclaspulsadas = 1;
				for (r = 0; r<ROWS; r++) for (c = 0; c<COLS; c++)
				{
					if (matriz[r][c] == 3) sinteclaspulsadas = 0; //si Hay alguna tecla pulsada no obedecmos ni al shift ni al symbol.
				}
				if (shiftmod == 2 && sinteclaspulsadas) shiftmod = 3;  //si Hay alguna tecla pulsada no obedecmos ni al shift ni al symbol.
				if (symbolmod == 2 && sinteclaspulsadas) symbolmod = 3; //si Hay alguna tecla pulsada no obedecmos ni al shift ni al symbol.
			}
			//Si esta pulsada la tecla symbol se pulsa y suelta esa tecla
			if (symbolmod == 3)
			{
				for (r = 0; r<ROWS; r++) for (c = 0; c<COLS; c++)
				{
					//if(matriz[r][c]>1 && matriz[r][c]<4) pulsaysueltateclaconsymbol(r,c,modo);
					if (matriz[r][c] == 2) pulsateclaconsymbol(r, c, modo);
					if (matriz[r][c] == 1) sueltateclaconsymbol(r, c, modo);

					//if(matriz[r][c]==1) matriz[r][c]=0; //Si esta marcada para soltar se pone a 0
				}
			}
			//Si esta pulsada la tecla shift se pulsa y suelta esa tecla
			if (shiftmod == 3)
			{
				for (r = 0; r<ROWS; r++) for (c = 0; c<COLS; c++)
				{
					//if(matriz[r][c]>1 && matriz[r][c]<4) { if(codeset==2) pulsaysueltateclaconshift(r,c,mapEXT[r][c]); else pulsaysueltateclaconshift(r,c,mapEXT1[r][c]); }
					if (matriz[r][c] == 2) { if (codeset == 2) pulsateclaconshift(r, c, mapEXT[r][c]); else pulsateclaconshift(r, c, mapEXT1[r][c]); }
					if (matriz[r][c] == 1) { if (codeset == 2) sueltateclaconshift(r, c, mapEXT[r][c]); else sueltateclaconshift(r, c, mapEXT1[r][c]); }
				}
			}
			//Si se suelta symbol se pone a cero sin mas (No se mantiene ninguna tecla pulsada de symbol que requiera mandar la marca de soltar)
			if (symbolmod == 1) symbolmod = 0;
			if (shiftmod == 1)  shiftmod = 0;

		}//Fin del If/else manejo de modo ZX u otros Keymaps
	}//Fin del If del control del teclado.
}//FIN de Matrixscan


int main()
{
	CPU_PRESCALE(CPU_16MHz);
	matrixInit();
	ps2Init();
	while (1) {
		if (ps2Stat() && modo == MAXKB) // Lineas CLK y/o DATA a 0 y modo PC-XT (Que siempre es el ultimo)
		{
			while (checkState(1000)) // tramos de 5 us (5000 us)
			{
				hostdataAnt = hostdata;
				if (getPS2(&hostdata) == 0)
				{
					if (hostdata == 0xEE)
					{
						sendPS2(0xEE); // Echo
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
						break;
					default:
						break;
					} //Fin del Swich
				} //Fin del IF de si detecta dato			
			} //Fin del while que chequea el estado
		}
		else matrixScan(); //No llegan datos del Host, enviamos teclado.

	}//Fin del Bucle infinito
}