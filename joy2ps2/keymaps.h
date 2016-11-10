// Definiciones teclado con scancodes ps/2
#define KEY_ESCAPE 0x76
#define KEY_DELETE 0x66
#define KEY_PERIOD 0x49

//Especiales, requieren E0
#define KEY_RIGHT   0x74
#define KEY_LEFT    0x6B
#define KEY_DOWN    0x72
#define KEY_UP      0x75
#define KEY_INS      0x70
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

//Mapa caracteres desde uart, para programa ejemplo Visual Basic
const unsigned char keymapVB[] = {
  0,      //00  
  0,      //01  
  0,    //02 
  0,    //03
  0,    //04 
  0,    //05 
  0,    //06 
  0,    //07 
  KEY_DELETE, //08 
  KEY_TAB,  //09 
  0,    //10 
  0,    //11 
  0,    //12 
  KEY_ENTER,  //13 
  0,    //14 
  0,    //15 
  KEY_LSHIFT, //16 
  KEY_LCTRL,  //17 
  KEY_LALT, //18 
  0,    //19 
  KEY_CAPS, //20 
  0,    //21 
  0,    //22 
  0,    //23 
  0,    //24 
  0,    //25 
  0,    //26 
  KEY_ESCAPE, //27 
  0,    //28 
  0,    //29 
  0,    //30 
  0,    //31 
  KEY_SPACE,  //32 
  KEY_PUP,  //33 
  KEY_PDN,  //34 
  KEY_END,  //35 
  KEY_HOME, //36 
  KEY_LEFT, //37 
  KEY_UP,   //38 
  KEY_RIGHT,      //39
  KEY_DOWN, //40
  0,    //41
  0,    //42
  0,    //43
  0,    //44
  KEY_INS,        //45
  KEY_DEL,  //46
  KEY_BAR,  //47
  KEY_0,    //48
  KEY_1,    //49
  KEY_2,    //50
  KEY_3,    //51
  KEY_4,    //52
  KEY_5,    //53
  KEY_6,    //54
  KEY_7,    //55
  KEY_8,    //56
  KEY_9,    //57
  KEY_PERIOD, //58  //VB
  KEY_COMMA,  //59  //VB
  KEY_BAR,  //60  //VB
  KEY_COLON,  //61  //VB
  0x4E,   //62  //VB
  KEY_TLD,  //63  //VB
  0x52,   //64  //?
  KEY_A,    //65
  KEY_B,    //66
  KEY_C,    //67
  KEY_D,    //68
  KEY_E,    //69
  KEY_F,    //70
  KEY_G,    //71
  KEY_H,    //72
  KEY_I,    //73
  KEY_J,    //74
  KEY_K,    //75
  KEY_L,    //76
  KEY_M,    //77
  KEY_N,    //78
  KEY_O,    //79
  KEY_P,    //80
  KEY_Q,    //81
  KEY_R,    //82
  KEY_S,    //83
  KEY_T,    //84
  KEY_U,    //85
  KEY_V,    //86
  KEY_W,    //87
  KEY_X,    //88
  KEY_Y,    //89
  KEY_Z,    //90
  0x54,   //91 //vb
  0x5b,   //92 //vb
  0x55,   //93 //vb
  0,    //94
  0,    //95
  KEYPAD_0, //96
  KEYPAD_1, //97
  KEYPAD_2, //98
  KEYPAD_3, //99
  KEYPAD_4, //100
  KEYPAD_5, //101
  KEYPAD_6, //102
  KEYPAD_7, //103
  KEYPAD_8, //104
  KEYPAD_9, //105
  KEYPAD_STAR,  //106
  KEYPAD_PLUS,  //107
  KEYPAD_ENT, //108
  KEYPAD_MIN, //109
  KEYPAD_DEL, //110
  KEYPAD_BAR, //111
  KEY_F1,   //112
  KEY_F2,   //113
  KEY_F3,   //114
  KEY_F4,   //115
  KEY_F5,   //116
  KEY_F6,   //117
  KEY_F7,   //118
  KEY_F8,   //119
  KEY_F9,   //120
  KEY_F10,  //121
  KEY_F11,  //122
  KEY_F12,  //123
  0,    //124
  0,    //125
  0,    //126
  0   //127

};
