// Definiciones teclado con scancodes ps/2

#define KEY_UP      1 // E0
#define KEY_DOWN    2 // E0
#define KEY_LEFT    3 // E0
#define KEY_RIGHT   4 // E0
#define KEY_INS	    5 // E0
#define KEY_HOME    6 // E0
#define KEY_PUP     7 // E0
#define KEY_DEL     8 // E0
#define KEY_END     9 // E0
#define KEY_PDN     10 // E0
//
#define KEY_A       11
#define KEY_B       12
#define KEY_C       13
#define KEY_D       14
#define KEY_E       15
#define KEY_F       16
#define KEY_G       17
#define KEY_H       18
#define KEY_I       19
#define KEY_J       20
#define KEY_K       21
#define KEY_L       22
#define KEY_M       23
#define KEY_N       24
#define KEY_O       25
#define KEY_P       26
#define KEY_Q       27
#define KEY_R       28
#define KEY_S       29
#define KEY_T       30
#define KEY_U       31
#define KEY_V       32
#define KEY_W       33
#define KEY_X       34
#define KEY_Y       35
#define KEY_Z       36
#define KEY_1       37
#define KEY_2       38
#define KEY_3       39
#define KEY_4       40
#define KEY_5       41
#define KEY_6       42
#define KEY_7       43
#define KEY_8       44
#define KEY_9       45
#define KEY_0       46
//
#define KEY_F1      47
#define KEY_F2      48
#define KEY_F3      49
#define KEY_F4      50
#define KEY_F5      51
#define KEY_F6      52
#define KEY_F7      53
#define KEY_F8      54
#define KEY_F9      55
#define KEY_F10     56
#define KEY_F11     57
#define KEY_F12     58
//
#define KEYPAD_1	59
#define KEYPAD_2	60
#define KEYPAD_3	61
#define KEYPAD_4	62
#define KEYPAD_5	63
#define KEYPAD_6	64
#define KEYPAD_7	65
#define KEYPAD_8	66
#define KEYPAD_9	67
#define KEYPAD_0	68
#define KEYPAD_BAR	69 // E0
#define KEYPAD_STAR 70
#define KEYPAD_MIN	71
#define KEYPAD_PLUS 72
#define KEYPAD_ENT	73 // E0
#define KEYPAD_DEL	74
#define KEYPAD_NUM	75
//
#define KEY_ESCAPE	76
#define KEY_DELETE	77
#define KEY_PERIOD	78
#define KEY_LCTRL   79
#define KEY_LALT    80
#define KEY_SCROLL	81
#define KEY_ENTER   82
#define KEY_SPACE   83
#define KEY_LSHIFT  84
#define KEY_RSHIFT  85
#define KEY_CAPS	86
#define KEY_TLD		87
#define KEY_TAB		88
#define KEY_APOS	89
#define KEY_COMMA	90
#define KEY_COLON	91
////////////////////////

#define DB15_PIN01   PIND
#define DB15_PIN02   PINB
#define DB15_PORT01   DDRD
#define DB15_PORT02   DDRB
#define PS2_PORT  PORTC
#define PS2_DDR   DDRC
#define PS2_PIN   PINC

#define PS2_DAT   PC3
#define PS2_CLK   PC2

#define CPU_PRESCALE(n) (CLKPR = 0x80, CLKPR = (n))
#define CHECKUP (!(PINC & (1 << 4)) | !(DB15_PIN01 & (1 << 0)))
#define CHECKDOWN (!(PINC & (1 << 5)) | !(DB15_PIN01 & (1 << 1)))

#define HI 1
#define LO 0
#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

//En us, reloj y semireloj, para los flancos
//zxuno v2 test15: CK1 = 240, CK2 = 480. Uso normal: CK1 = 20, CK2 = 40 microsegundos
//(revertir a normal cuando el core ps/2 del ZX-UNO se mejore)
uint8_t ckt;
#define CK1 4
#define CK2 8

#define LED_ON	PORTB |= (1<<5)		
#define LED_OFF	PORTB &= ~(1<<5)

uint16_t DB15_PIN;

uint8_t p1map, p2map;

#define QUEUE_ELEMENTS 64
#define QUEUE_SIZE (QUEUE_ELEMENTS + 2)
unsigned char QueuePS2Command[QUEUE_SIZE];
double QueuePS2WaitMS[QUEUE_SIZE];

uint8_t QueueIn, QueueOut;

unsigned char sendcode;
double wait_ms;

unsigned char P1KeyMap[12];
unsigned char P2KeyMap[12];
uint8_t imap;
