// Definiciones teclado con scancodes ps/2
#define KEY_ESCAPE 0x76
#define KEY_DELETE 0x66
#define KEY_PERIOD 0x49

//Especiales, requieren E0
#define KEY_RIGHT   0x74
#define KEY_LEFT    0x6B
#define KEY_DOWN    0x72
#define KEY_UP      0x75
#define KEY_INS	    0x70
#define KEY_HOME    0x6C
#define KEY_PUP     0x7D
#define KEY_DEL     0x71
#define KEY_END     0x69
#define KEY_PDN     0x7A

#define KEY_LCTRL   0x14
#define KEY_LALT    0x11

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

#define KEY_CAPS  0x58

#define KEY_TLD   0x0E
#define KEY_TAB   0x0D
#define KEY_APOS  0x55
#define KEY_PLUS  0x79
#define KEY_MINUS 0x7B
#define KEY_STAR  0x7C
#define KEY_COMMA 0x41
#define KEY_BAR   0x4A
#define KEY_COLON 0x4C

#define KEYPAD_0 0x70
#define KEYPAD_1 0x69
#define KEYPAD_2 0x72
#define KEYPAD_3 0x7A
#define KEYPAD_4 0x6B
#define KEYPAD_5 0x73
#define KEYPAD_6 0x74
#define KEYPAD_7 0x6C
#define KEYPAD_8 0x75
#define KEYPAD_9 0x7D
#define KEYPAD_BAR 0x4A
#define KEYPAD_STAR 0x7C
#define KEYPAD_MIN 0x7B
#define KEYPAD_PLUS 0x79
#define KEYPAD_ENT 0x5A
#define KEYPAD_DEL 0x71
#define KEYPAD_NUM 0x77

// Mapas del joystick

const unsigned char inimap[] = {

	'[','J', 'O', 'Y', '2', 'P', 'S', '2', ']', '0', '0', '0', '1', 'b'

};

const unsigned char db15 = 1;

const unsigned char Map0[] = { // Mapa 0 -> Por defecto al iniciar
	KEY_Q,       	//DB15 00  UP		DB9_1 00  UP
	KEY_A,       	//DB15 01  DOWN		DB9_1 01  DOWN
	KEY_O,       	//DB15 02  LEFT		DB9_1 02  LEFT
	KEY_P,       	//DB15 03  RIGHT	DB9_1 03  RIGHT
	KEY_ESCAPE,     //DB15 04  SELECT	DB9_1 04  BUTTON 1
	KEY_ENTER,      //DB15 05  START	DB9_1 05  BUTTON 2
	KEY_SPACE,      //DB15 06  BUTTON 1	DB9_2 06  UP
	KEY_V,       	//DB15 07  BUTTON 2	DB9_2 07  DOWN
	KEY_B,       	//DB15 08  BUTTON 3	DB9_2 08  LEFT
	KEY_N,       	//DB15 09  BUTTON 4	DB9_2 09  RIGHT
	KEY_G,       	//DB15 10  BUTTON 5	DB9_2 10  BUTTON 1
	KEY_H        	//DB15 11  BUTTON 6	DB9_2 11  BUTTON 2

};

const unsigned char Map1[] = { // Mapa 1
	0,       //DB15 00  UP			DB9_1 00  UP
	0,       //DB15 01  DOWN		DB9_1 01  DOWN
	0,       //DB15 02  LEFT		DB9_1 02  LEFT
	0,       //DB15 03  RIGHT		DB9_1 03  RIGHT
	0,       //DB15 04  SELECT		DB9_1 04  BUTTON 1
	0,       //DB15 05  START		DB9_1 05  BUTTON 2
	0,       //DB15 06  BUTTON 1	DB9_2 06  UP
	0,       //DB15 07  BUTTON 2	DB9_2 07  DOWN
	0,       //DB15 08  BUTTON 3	DB9_2 08  LEFT
	0,       //DB15 09  BUTTON 4	DB9_2 09  RIGHT
	0,       //DB15 10  BUTTON 5	DB9_2 10  BUTTON 1
	0        //DB15 11  BUTTON 6	DB9_2 11  BUTTON 2

};

const unsigned char Map2[] = { // Mapa 2
	0,       //DB15 00  UP			DB9_1 00  UP
	0,       //DB15 01  DOWN		DB9_1 01  DOWN
	0,       //DB15 02  LEFT		DB9_1 02  LEFT
	0,       //DB15 03  RIGHT		DB9_1 03  RIGHT
	0,       //DB15 04  SELECT		DB9_1 04  BUTTON 1
	0,       //DB15 05  START		DB9_1 05  BUTTON 2
	0,       //DB15 06  BUTTON 1	DB9_2 06  UP
	0,       //DB15 07  BUTTON 2	DB9_2 07  DOWN
	0,       //DB15 08  BUTTON 3	DB9_2 08  LEFT
	0,       //DB15 09  BUTTON 4	DB9_2 09  RIGHT
	0,       //DB15 10  BUTTON 5	DB9_2 10  BUTTON 1
	0        //DB15 11  BUTTON 6	DB9_2 11  BUTTON 2

};

const unsigned char Map3[] = { // Mapa 3
	0,       //DB15 00  UP			DB9_1 00  UP
	0,       //DB15 01  DOWN		DB9_1 01  DOWN
	0,       //DB15 02  LEFT		DB9_1 02  LEFT
	0,       //DB15 03  RIGHT		DB9_1 03  RIGHT
	0,       //DB15 04  SELECT		DB9_1 04  BUTTON 1
	0,       //DB15 05  START		DB9_1 05  BUTTON 2
	0,       //DB15 06  BUTTON 1	DB9_2 06  UP
	0,       //DB15 07  BUTTON 2	DB9_2 07  DOWN
	0,       //DB15 08  BUTTON 3	DB9_2 08  LEFT
	0,       //DB15 09  BUTTON 4	DB9_2 09  RIGHT
	0,       //DB15 10  BUTTON 5	DB9_2 10  BUTTON 1
	0        //DB15 11  BUTTON 6	DB9_2 11  BUTTON 2

};