/*
* Definiciones teclado con scancodes ps/2
*/
//KEY_1: !        KEY_2: @        KEY_3: #        KEY_4: $        KEY_5: %
//KEY_Q: NADA     KEY_W: NADA     KEY_E: NADA     KEY_R: <        KEY_T: >
//KEY_A: ~        KEY_S: |        KEY_D: \        KEY_F: {        KEY_G: }
//KEY_0: _        KEY_9: )        KEY_8: (        KEY_7: ´        KEY_6: &
//KEY_P: "        KEY_O: ;        KEY_I: NADA     KEY_U: ]        KEY_Y: [
//NADA            KEY_Z: :        KEY_X: (libra)  KEY_C: ?        KEY_V: /
//KEY_ENTER: NADA KEY_L: =        KEY_K: +        KEY_J: -        KEY_H: (flecha)
//KEY_SPACE: NADA NADA            KEY_M: .        KEY_N: ,        KEY_B: *

const uint8_t version[8] = { 0,1,0,3,2,0,1,8 }; //DDMMAAAA

// Joy2PS2 Scancodes Index
#define	JOY_UP      1 // E0
#define JOY_DOWN    2 // E0
#define JOY_LEFT    3 // E0
#define JOY_RIGHT   4 // E0
#define JOY_INS	    5 // E0
#define JOY_HOME    6 // E0
#define JOY_PUP     7 // E0
#define JOY_DEL     8 // E0
#define JOY_END     9 // E0
#define JOY_PDN     10 // E0
//
#define JOY_A       11
#define JOY_B       12
#define JOY_C       13
#define JOY_D       14
#define JOY_E       15
#define JOY_F       16
#define JOY_G       17
#define JOY_H       18
#define JOY_I       19
#define JOY_J       20
#define JOY_K       21
#define JOY_L       22
#define JOY_M       23
#define JOY_N       24
#define JOY_O       25
#define JOY_P       26
#define JOY_Q       27
#define JOY_R       28
#define JOY_S       29
#define JOY_T       30
#define JOY_U       31
#define JOY_V       32
#define JOY_W       33
#define JOY_X       34
#define JOY_Y       35
#define JOY_Z       36
#define JOY_1       37
#define JOY_2       38
#define JOY_3       39
#define JOY_4       40
#define JOY_5       41
#define JOY_6       42
#define JOY_7       43
#define JOY_8       44
#define JOY_9       45
#define JOY_0       46
												//
#define JOY_F1      47
#define JOY_F2      48
#define JOY_F3      49
#define JOY_F4      50
#define JOY_F5      51
#define JOY_F6      52
#define JOY_F7      53
#define JOY_F8      54
#define JOY_F9      55
#define JOY_F10     56
#define JOY_F11     57
#define JOY_F12     58
												//
#define JOYPAD_1	59
#define JOYPAD_2	60
#define JOYPAD_3	61
#define JOYPAD_4	62
#define JOYPAD_5	63
#define JOYPAD_6	64
#define JOYPAD_7	65
#define JOYPAD_8	66
#define JOYPAD_9	67
#define JOYPAD_0	68
#define JOYPAD_BAR	69 // E0
#define JOYPAD_STAR 70
#define JOYPAD_MIN	71
#define JOYPAD_PLUS 72
#define JOYPAD_ENT	73 // E0
#define JOYPAD_DEL	74
#define JOYPAD_NUM	75
												//
#define JOY_ESCAPE	76
#define JOY_DELETE	77
#define JOY_PERIOD	78
#define JOY_LCTRL   79
#define JOY_LALT    80
#define JOY_SCROLL	81
#define JOY_ENTER   82
#define JOY_SPACE   83
#define JOY_LSHIFT  84
#define JOY_RSHIFT  85
#define JOY_CAPS	86
#define JOY_TLD		87
#define JOY_TAB		88
#define JOY_APOS	89
#define JOY_COMMA	90
#define JOY_COLON	91
////////////////////////

// Scancodes 
#define KEY_ESCAPE 0x76
#define KEY_DELETE 0x71
#define KEY_BACKSP 0x66
#define KEY_SCRLCK 0x7E

#define KEY_LCTRL   0x14
#define KEY_LALT    0x11

												//Especiales, requieren E0
#define KEY_RIGHT   0x74
#define KEY_LEFT    0x6B
#define KEY_DOWN    0x72
#define KEY_UP      0x75
#define KEY_RCTRL   0x14
#define KEY_RALT    0x11
#define KEY_LWIN    0x1F
#define KEY_RWIN    0x27
#define KEY_APPS    0x2F
#define KEY_PGUP    0x7D
#define KEY_PGDW    0x7A
												//Fin Especiales

#define KEY_A       0x1C
#define KEY_B       0x32
#define KEY_C       0x21
#define KEY_D       0x23
#define KEY_E       0x24
#define KEY_F       0x2B
#define KEY_G       0x34
#define KEY_H       0x33
#define KEY_I       0x43
#define KEY_J       0x3B
#define KEY_K       0x42
#define KEY_L       0x4B
#define KEY_M       0x3A
#define KEY_N       0x31
#define KEY_O       0x44
#define KEY_P       0x4D
#define KEY_Q       0x15
#define KEY_R       0x2D
#define KEY_S       0x1B
#define KEY_T       0x2C
#define KEY_U       0x3C
#define KEY_V       0x2A
#define KEY_W       0x1D
#define KEY_X       0x22
#define KEY_Y       0x35
#define KEY_Z       0x1A
#define KEY_1       0x16
#define KEY_2       0x1E
#define KEY_3       0x26
#define KEY_4       0x25
#define KEY_5       0x2E
#define KEY_6       0x36
#define KEY_7       0x3D
#define KEY_8       0x3E
#define KEY_9       0x46
#define KEY_0       0x45

#define KEY_ENTER   0x5A
#define KEY_SPACE   0x29

#define KEY_F1      0x05
#define KEY_F2      0x06
#define KEY_F3      0x04
#define KEY_F4      0x0c
#define KEY_F5      0x03
#define KEY_F6      0x0B
#define KEY_F7      0x83
#define KEY_F8      0x0A
#define KEY_F9      0x01
#define KEY_F10     0x09
#define KEY_F11     0x78
#define KEY_F12     0x07

#define KEY_LSHIFT  0x12
#define KEY_RSHIFT  0x59

#define KEY_CAPS    0x58

#define KEY_TAB     0x0D

#define KEY_TLD     0x0E //Izxda del 1
#define KEY_MENOS	0x4E //Drcha del 0
#define KEY_IGUAL	0x55 //Izda de Backspace
#define KEY_ACORCHE 0x54 //Drcha de la P
#define KEY_CCORCHE 0x5B //Siguiente a la de la Drcha de la P
#define KEY_BKSLASH 0x5D //Izda del Enter (Puede estar en la fila de la P o de la L
#define KEY_PTOCOMA 0x4C //La Ñ
#define KEY_COMILLA 0x52 //Derecha de la Ñ
#define KEY_COMA    0x41 //Decha de la M
#define KEY_PUNTO   0x49 //Siguiente del de la Derecha de la M
#define KEY_SLASH   0x4A //Izda del Shift Derecho
#define KEY_LESS    0x61 //Izda de la Z

												//SCANCODES SET 1
#define KS1_RELEASE 0x80 //Valor que se suma al codeigo de la tecla para soltarla
#define KS1_ESCAPE 0x01
#define KS1_DELETE 0x53
#define KS1_BACKSP 0x0E
#define KS1_SCRLCK 0x46

#define KS1_LCTRL   0x1D
#define KS1_LALT    0x38

												//Especiales, requieren E0
#define KS1_RIGHT   0x4D
#define KS1_LEFT    0x4B
#define KS1_DOWN    0x50
#define KS1_UP      0x48

#define KS1_RCTRL   0x1D
#define KS1_RALT    0x38

#define KS1_LWIN    0x5B
#define KS1_RWIN    0x5C
#define KS1_APPS    0x5D

#define KS1_PGUP    0x49
#define KS1_PGDW    0x51
#define KS1_HOME    0x47
#define KS1_END     0x4F
#define KS1_INS     0x52
#define KS1_DEL     0x53
												//Fin Especiales

#define KS1_A       0x1E
#define KS1_B       0x30
#define KS1_C       0x2E
#define KS1_D       0x20
#define KS1_E       0x12
#define KS1_F       0x21
#define KS1_G       0x22
#define KS1_H       0x23
#define KS1_I       0x17
#define KS1_J       0x24
#define KS1_K       0x25
#define KS1_L       0x26
#define KS1_M       0x32
#define KS1_N       0x31
#define KS1_O       0x18
#define KS1_P       0x19
#define KS1_Q       0x10
#define KS1_R       0x13
#define KS1_S       0x1F
#define KS1_T       0x14
#define KS1_U       0x16
#define KS1_V       0x2F
#define KS1_W       0x11
#define KS1_X       0x2D
#define KS1_Y       0x15
#define KS1_Z       0x2C
#define KS1_1       0x02
#define KS1_2       0x03
#define KS1_3       0x04
#define KS1_4       0x05
#define KS1_5       0x06
#define KS1_6       0x07
#define KS1_7       0x08
#define KS1_8       0x09
#define KS1_9       0x0A
#define KS1_0       0x0B

#define KS1_ENTER   0x1C
#define KS1_SPACE   0x39

#define KS1_F1      0x3B
#define KS1_F2      0x3C
#define KS1_F3      0x3D
#define KS1_F4      0x3E
#define KS1_F5      0x3F
#define KS1_F6      0x40
#define KS1_F7      0x41
#define KS1_F8      0x42
#define KS1_F9      0x43
#define KS1_F10     0x44
#define KS1_F11     0x57
#define KS1_F12     0x58

#define KS1_LSHIFT  0x2A
#define KS1_RSHIFT  0x36

#define KS1_CAPS    0x3A

#define KS1_TAB     0x0F

#define KS1_TLD     0x29 //Izxda del 1
#define KS1_MENOS	0x0C //Drcha del 0
#define KS1_IGUAL	0x0D //Izda de Backspace
#define KS1_ACORCHE 0x1A //Drcha de la P
#define KS1_CCORCHE 0x1B //Siguiente a la de la Drcha de la P
#define KS1_BKSLASH 0x2B //Izda del Enter (Puede estar en la fila de la P o de la L
#define KS1_PTOCOMA 0x27 //La Ñ
#define KS1_COMILLA 0x28 //Derecha de la Ñ
#define KS1_COMA    0x33 //Decha de la M
#define KS1_PUNTO   0x34 //Siguiente del de la Derecha de la M
#define KS1_SLASH   0x35 //Izda del Shift Derecho
#define KS1_LESS    0x56 //Izda de la Z

#define MODO_A       ori
#define MODO_B       sam
#define MODO_C       jup
#define MODO_D       pcxt
#define MODO_E       kbext
#define MODO_F       -1
#define MODO_G       -1
#define MODO_H       -1
#define MODO_I       -1
#define MODO_J       -1
#define MODO_K       -1
#define MODO_L       -1
#define MODO_M       -1
#define MODO_N       -1
#define MODO_O       -1
#define MODO_P       -1
#define MODO_Q       -1
#define MODO_R       -1
#define MODO_S       -1
#define MODO_T       -1
#define MODO_U       -1
#define MODO_V       -1
#define MODO_W       -1
#define MODO_X       -1
#define MODO_Y       -1
#define MODO_Z       -1
#define MODO_SS      -1
#define MODO_CS      -1
#define MODO_ENTER   -1
#define MODO_SPACE   -1
#define MODO_1       cpc
#define MODO_2       msx
#define MODO_3       c64
#define MODO_4       at8
#define MODO_5       bbc
#define MODO_6       aco
#define MODO_7       ap2
#define MODO_8       vic
#define MODO_9       pc
#define MODO_0       zx

const uint8_t versionKeyCodes[10] = { KEY_0,KEY_1,KEY_2,KEY_3,KEY_4,KEY_5,KEY_6,KEY_7,KEY_8,KEY_9 };
const uint8_t eepromsavename[10] = { KEY_C,KEY_F,KEY_G,KEY_F,KEY_L,KEY_A,KEY_S,KEY_H,KEY_E,KEY_D };

const uint8_t fkbmode0[15] = { KEY_Z,KEY_X,KEY_P,KEY_A,KEY_R,KEY_T,KEY_I,KEY_A,KEY_L,KEY_C,KEY_O,KEY_M,KEY_B,KEY_O,KEY_S };
const uint8_t fkbmode1[12] = { KEY_Z,KEY_X, KEY_F,KEY_U,KEY_L,KEY_L,KEY_C,KEY_O,KEY_M,KEY_B,KEY_O,KEY_S };
const uint8_t fkbmode2[6] = { KEY_Z,KEY_X,KEY_P,KEY_U,KEY_R,KEY_E };

typedef enum { zx, cpc, msx, c64, at8, bbc, aco, ap2, vic, ori, sam, jup, pc, pcxt, kbext } KBMODE;

#ifdef membrane13x11
const uint8_t mapZX[ROWS][COLS] = {
	{ KEY_1,      KEY_2,      KEY_3,      KEY_4,      KEY_5,			0,			0,			0,			0,			   0,			  0 },
	{ KEY_Q,      KEY_W,      KEY_E,      KEY_R,      KEY_T,			0,			0,			0,			0,			   0,			  0 },
	{ KEY_A,      KEY_S,      KEY_D,      KEY_F,      KEY_G,			0,			0,			0,			0,		       0,			  0 },
	{ KEY_0,      KEY_9,      KEY_8,      KEY_7,      KEY_6,			0,			0,			0,			0,			   0,			  0 },
	{ KEY_P,      KEY_O,      KEY_I,      KEY_U,      KEY_Y,			0,			0,			0,			0,			   0,			  0 },
	{ 0,	  KEY_Z,      KEY_X,      KEY_C,      KEY_V,			0,			0,			0,			0,			   0,			  0 },
	{ KEY_ENTER,	  KEY_L,      KEY_K,      KEY_J,      KEY_H,			0,			0,			0,			0,			   0,			  0 },
	{ KEY_SPACE,		  0,      KEY_M,      KEY_N,      KEY_B,			0,			0,			0,			0,		       0,	          0 },
	// Teclas extendidas +2A/+3
	{ 0,		  0,		  0,		  0,		  0,			0,          0,	        0,	       0,    KEY_COMILLA,   KEY_PTOCOMA },
	{ 0,		  0,		  0,		  0,		  0,	KEY_PUNTO,	 KEY_COMA,	        0,	       0,	           0,	          0 },
	{ 0,		  0,		  0,		  0,		  0,	   KEY_F3,	   KEY_F4,	   KEY_F1,  KEY_CAPS,		       0,      KEY_LEFT },
	{ 0,		  0,		  0,		  0,		  0,	KEY_RIGHT,	   KEY_UP, KEY_BACKSP,   KEY_F9,		       0,	   KEY_DOWN },
	{ 0,		  0,		  0,		  0,		  0,			0,          0, KEY_ESCAPE,	 KEY_TAB,			   0,		      0 }
};
#else
const uint8_t mapZX[ROWS][COLS] = {
	{ KEY_1,          KEY_2,          KEY_3,          KEY_4,          KEY_5 },
	{ KEY_Q,          KEY_W,          KEY_E,          KEY_R,          KEY_T },
	{ KEY_A,          KEY_S,          KEY_D,          KEY_F,          KEY_G },
	{ KEY_0,          KEY_9,          KEY_8,          KEY_7,          KEY_6 },
	{ KEY_P,          KEY_O,          KEY_I,          KEY_U,          KEY_Y },
	{ 0,	          KEY_Z,          KEY_X,          KEY_C,          KEY_V },
	{ KEY_ENTER,      KEY_L,          KEY_K,          KEY_J,          KEY_H },
	{ KEY_SPACE,      0,	          KEY_M,          KEY_N,          KEY_B }
};
#endif
const uint8_t mapEXT[ROWS8][COLS5] = { //Mapa especial con caps shift (Igual en todos los Keymaps)
	{ KEY_F2,	  KEY_CAPS,   0,		  0,   KEY_LEFT },
	{ 0,          0,          0,          0,          0 },
	{ 0,          0,          0,          0,          0 },
	{ KEY_BACKSP, 0,  KEY_RIGHT,     KEY_UP,   KEY_DOWN },
	{ 0,          0,          0,          0,          0 },
	{ 0,          0,          0,          0,          0 },
	{ KEY_TAB,    0,          0,          0,		  0 },
	{ KEY_ESCAPE, 0,          0,          0,		  0 }
};
const uint8_t mapSET1[ROWS8][COLS5] = { //MAPA Codeset 1
	{ KS1_1,          KS1_2,          KS1_3,          KS1_4,          KS1_5 },
	{ KS1_Q,          KS1_W,          KS1_E,          KS1_R,          KS1_T },
	{ KS1_A,          KS1_S,          KS1_D,          KS1_F,          KS1_G },
	{ KS1_0,          KS1_9,          KS1_8,          KS1_7,          KS1_6 },
	{ KS1_P,          KS1_O,          KS1_I,          KS1_U,          KS1_Y },
	{ 0,			  KS1_Z,          KS1_X,          KS1_C,          KS1_V },
	{ KS1_ENTER,      KS1_L,          KS1_K,          KS1_J,          KS1_H },
	{ KS1_SPACE,      0,		      KS1_M,          KS1_N,          KS1_B }
};
const uint8_t mapEXT1[ROWS8][COLS5] = { //Mapa especial con caps shift para Codeset1(Igual en todos los Keymaps)
	{ KS1_F2,	  KS1_CAPS,			 0,		  0,   KS1_LEFT },
	{ 0,				 0,			 0,       0,          0 },
	{ 0,				 0,			 0,       0,          0 },
	{ KS1_BACKSP,		 0,  KS1_RIGHT,		  0,   KS1_DOWN },
	{ 0,				 0,			 0,       0,          0 },
	{ 0,				 0,			 0,       0,          0 },
	{ KS1_TAB,			 0,			 0,       0,		  0 },
	{ KS1_ESCAPE,		 0,			 0,       0,		  0 }
};
const uint8_t mapXT1[ROWS8][COLS5] = { //Mapa de PC-XT CodeSet1 pulsando Control (symbol shift)
	{ KS1_1,			  KS1_2,          KS1_3,          KS1_4,         KS1_5 },
	{ 0,					  0,              0,       KS1_COMA,     KS1_PUNTO },
	{ KS1_TLD,		KS1_BKSLASH,    KS1_BKSLASH,    KS1_ACORCHE,   KS1_CCORCHE },
	{ KS1_MENOS,		  KS1_0,          KS1_9,    KS1_COMILLA,	     KS1_7 },
	{ KS1_COMILLA,  KS1_CCORCHE,              0,    KS1_CCORCHE,   KS1_ACORCHE },
	{ 0,		    KS1_PTOCOMA,              0,      KS1_SLASH,     KS1_SLASH },
	{ 0,		      KS1_IGUAL,      KS1_IGUAL,      KS1_MENOS,	     KS1_6 },
	{ 0,					  0,      KS1_PUNTO,       KS1_COMA,		 KS1_8 }
};
const uint8_t modXT1[ROWS8][COLS5] = { //Mod de PC-XT CodeSet1 1 hay q usar Shift, 0 no hay que usar
	{ 1,              1,              1,              1,              1 },
	{ 0,              0,              0,              1,              1 },
	{ 1,              1,              0,              1,              1 },
	{ 1,              1,              1,              0,              1 },
	{ 1,              0,              0,              0,              0 },
	{ 0,              1,              0,              1,              0 },
	{ 0,              0,              1,              0,              1 },
	{ 0,              0,              0,              0,              1 }
};
const uint8_t mapMODO[ROWS8][COLS5] = {
	{ MODO_1,          MODO_2,          MODO_3,          MODO_4,          MODO_5 },
	{ MODO_Q,          MODO_W,          MODO_E,          MODO_R,          MODO_T },
	{ MODO_A,          MODO_S,          MODO_D,          MODO_F,          MODO_G },
	{ MODO_0,          MODO_9,          MODO_8,          MODO_7,          MODO_6 },
	{ MODO_P,          MODO_O,          MODO_I,          MODO_U,          MODO_Y },
	{ MODO_SS,         MODO_Z,          MODO_X,          MODO_C,          MODO_V },
	{ MODO_ENTER,      MODO_L,          MODO_K,          MODO_J,          MODO_H },
	{ MODO_SPACE,      MODO_CS,         MODO_M,          MODO_N,          MODO_B }
};
//#endif

// CKm=Multiplicador de CK1 y CK2
const uint8_t nomZX[] = { 2,KEY_Z,KEY_X,1 }; //Numero de Letras,(letras[n],,),CKm)
const uint8_t nomCPC[] = { 3,KEY_C,KEY_P,KEY_C,4 };
const uint8_t nomMSX[] = { 3,KEY_M,KEY_S,KEY_X,4 };
const uint8_t nomC64[] = { 3,KEY_C,KEY_6,KEY_4,4 };
const uint8_t nomAT8[] = { 5,KEY_A,KEY_T,KEY_A,KEY_R,KEY_I,4 };
const uint8_t nomBBC[] = { 3,KEY_B,KEY_B,KEY_C,4 };
const uint8_t nomACO[] = { 5,KEY_A,KEY_C,KEY_O,KEY_R,KEY_N,4 };
const uint8_t nomVIC[] = { 3,KEY_V,KEY_I,KEY_C,4 };
const uint8_t nomORI[] = { 4,KEY_O,KEY_R,KEY_I,KEY_C,4 };
const uint8_t nomSAM[] = { 3,KEY_S,KEY_A,KEY_M,4 };
const uint8_t nomJUP[] = { 7,KEY_J,KEY_U,KEY_P,KEY_I,KEY_T,KEY_E,KEY_R,4 };
const uint8_t nomAP2[] = { 5,KEY_A,KEY_P,KEY_P,KEY_L,KEY_E,4 };
const uint8_t nomPC[] = { 2,KEY_P,KEY_C,4 };
const uint8_t nomPCXT[] = { 4,KEY_P,KEY_C,KEY_X,KEY_T,4 };
const uint8_t nomKBEXT[] = { 5,KEY_K,KEY_B,KEY_E,KEY_X,KEY_T,4 };

typedef struct {
	uint8_t		up;				// 0
	uint8_t		down;			// 1
	uint8_t		left;			// 2
	uint8_t		right;			// 3
	uint8_t		select;			// 4
	uint8_t		start;			// 5
	uint8_t		button1;		// 6
	uint8_t		button2;		// 7
	uint8_t		button3;		// 8
	uint8_t		button4;		// 9
	uint8_t		button5;		// 10
	uint8_t		button6;		// 11	
} report_t;

unsigned char P1KeyMap[] = {
									 
	JOY_Q,       	// UP
	JOY_A,       	// DOWN
	JOY_O,       	// LEFT
	JOY_P,       	// RIGHT
	JOY_5,	        // SELECT
	JOY_1,		    // START
	JOY_SPACE,      // BUTTON 1
	JOY_X,       	// BUTTON 2
	JOY_M,       	// BUTTON 3
	JOY_D,       	// BUTTON 4
	JOY_F,       	// BUTTON 5
	JOY_C        	// BUTTON 6

};

const unsigned char ScanCodes1[] = { 0x00, 0x48, 0x50, 0x4B, 0x4D, 0x52, 0x47, 0x49, 0x4B, 0x4F, 0x51,   // UP, DOWN, LEFT, RIGHT, INS, HOME, PUP, DEL, END, PDOWN
0x1E, 0x30, 0x2E, 0x20, 0x12, 0x21, 0x22, 0x23, 0x17, 0x24,   // A -> J
0x25, 0x26, 0x32, 0x31, 0x18, 0x19, 0x10, 0x13, 0x1F, 0x14,   // K -> T
0x16, 0x2F, 0x11, 0x2D, 0x15, 0x2C,						   // U -> Z
0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,   // 1 -> 9, 0
0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x41, 0x42, 0x43, 0x44, 0x57, 0x58, // F1 -> F12
0x4F, 0x50, 0x51, 0x4B, 0x4C, 0x4D, 0x47, 0x48, 0x49, 0x52, 0x35, 0x37, 0x4A, 0x4E, 0x1C, 0x53, 0x45, // Keypad Buttons
0x01, 0x0E, 0x34, 0x1D, 0x38, 0x46, 0x1C, 0x39, 0x2A, 0x36, 0x3A, 0x29, 0x0F, 0x0D, 0x33, 0x27 }; // Others

const unsigned char ScanCodes2[] = { 0x00, 0x75, 0x72, 0x6B, 0x74, 0x70, 0x6C, 0x7D, 0x71, 0x69, 0x7A,   // UP, DOWN, LEFT, RIGHT, INS, HOME, PUP, DEL, END, PDOWN
0x1C, 0x32, 0x21, 0x23, 0x24, 0x2B, 0x34, 0x33, 0x43, 0x3B,   // A -> J
0x42, 0x4B, 0x3A, 0x31, 0x44, 0x4D, 0x15, 0x2D, 0x1B, 0x2C,   // K -> T
0x3C, 0x2A, 0x1D, 0x22, 0x35, 0x1A,						   // U -> Z
0x16, 0x1E, 0x26, 0x25, 0x2E, 0x36, 0x3D, 0x3E, 0x46, 0x45,   // 1 -> 9, 0
0x05, 0x06, 0x04, 0x0C, 0x03, 0x0B, 0x83, 0x0A, 0x01, 0x09, 0x78, 0x07, // F1 -> F12
0x69, 0x72, 0x7A, 0x6B, 0x73, 0x74, 0x6C, 0x75, 0x7D, 0x70, 0x4A, 0x7C, 0x7B, 0x79, 0x5A, 0x71, 0x77, // Keypad Buttons
0x76, 0x66, 0x49, 0x14, 0x11, 0x7E, 0x5A, 0x29, 0x12, 0x59, 0x58, 0x0E, 0x0D, 0x55, 0x41, 0x4C }; // Others

