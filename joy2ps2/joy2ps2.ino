#include <stdio.h>
#include <avr/io.h>
#include <inttypes.h>
#include <util/delay.h>

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

#define HI 1
#define LO 0
#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

uint16_t DB15_PIN, DB15_PINAux;
uint16_t DB15_PINChanges, DB15PINPrev;

uint8_t keyup, keydown, keyleft, keyright;
uint8_t mapper;

void ps2Mode(uint8_t pin, uint8_t mode)
{
  if (mode) { //high
    PS2_DDR &= ~_BV(pin); //input (Truco DDR. Como input sin estado, se pone en modo Hi-Z)
  }
  else { //low
    PS2_DDR |= _BV(pin); //output (Truco DDR. Como output, se pone a 0v)
  }
}

//En us, reloj y semireloj, para los flancos
//zxuno v2 test15: CK1 = 240, CK2 = 480. Uso normal: CK1 = 20, CK2 = 40 microsegundos
//(revertir a normal cuando el core ps/2 del ZX-UNO se mejore)
#define CK1 20 
#define CK2 40

void ps2Init()
{
  //ponemos en alto ambas señales
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

//envio de datos ps/2 simulando reloj con delays.
void sendPS2(unsigned char code)
{

  //Para continuar las líneas deben estar en alto
  if (ps2Stat())
    return;
  // CLK debe encontrarse en alto durante al menos 50us
  _delay_us(50);
  if (!(PS2_PIN & (1 << PS2_CLK)))
    return;

  unsigned char parity = 1;
  unsigned char i = 0;

  //iniciamos transmisión
  ps2Mode(PS2_DAT, LO);
  _delay_us(CK1);

  ps2Mode(PS2_CLK, LO); //bit de comienzo
  _delay_us(CK2);
  ps2Mode(PS2_CLK, HI);
  _delay_us(CK1);
  //enviamos datos
  for (i = 0; i < 8; ++i)
  {
    if ((0b00000001 & code))
      ps2Mode(PS2_DAT, HI);
    else
      ps2Mode(PS2_DAT, LO);

    _delay_us(CK1);
    ps2Mode(PS2_CLK, LO);
    _delay_us(CK2);
    ps2Mode(PS2_CLK, HI);
    _delay_us(CK1);

    //paridad
    if ((0b00000001 & code) == 0b00000001)
    {
      if (!parity)
        parity = 1;
      else
        parity = 0;
    }
    code = code >> 1;
  }

  // Enviamos bit de paridad
  if (parity)
    ps2Mode(PS2_DAT, HI);
  else
    ps2Mode(PS2_DAT, LO);

  _delay_us(CK1);
  ps2Mode(PS2_CLK, LO);
  _delay_us(CK2);
  ps2Mode(PS2_CLK, HI);
  _delay_us(CK1);

  //Bit de parada
  ps2Mode(PS2_DAT, HI);
  _delay_us(CK1);
  ps2Mode(PS2_CLK, LO);
  _delay_us(CK2);
  ps2Mode(PS2_CLK, HI);
  _delay_us(CK1);

  _delay_us(50); //fin  
  
}

//codifica envio de caracteres ps/2 
void sendCodeMR(unsigned char key, uint16_t release)
{
  uint8_t extn = 0;

  //checkeamos si es una tecla con scancode extendido (E0)
  switch (key) {
  case KEY_LEFT:
  case KEY_DOWN:
  case KEY_RIGHT:
  case KEY_UP:
  case KEY_HOME:
  case KEY_END:
  case KEY_PDN:
  case KEY_PUP:
    extn = 1;
    break;
  default:
    extn = 0;
    break;
  }
  //secuencia  

  if (extn)
    sendPS2(0xE0);

  if (key && release)
    sendPS2(0xF0);

  if (key)
    sendPS2(key);

  //fin secuencia
}

void PressKey(unsigned char key)
{
  sendCodeMR(keymapVB[key], 0); //Make
  _delay_ms(100);
  sendCodeMR(keymapVB[key], 1); //Release
}


void LOAD128() // LOAD "" en BASIC 128
{

  PressKey('L');
  PressKey('O');
  PressKey('A');
  PressKey('D');
  sendPS2(KEY_RSHIFT); // Mantenemos pulsado SHIFT
  PressKey('2');
  _delay_ms(100);
  PressKey('2');
  sendPS2(0xF0); // Liberamos SHIFT
  sendPS2(KEY_RSHIFT);
  PressKey(0x0D); // ENTER (13)

}

void LOAD48() // LOAD "" en BASIC 48
{

  PressKey('J');
  sendPS2(KEY_RSHIFT); // Mantenemos pulsado SHIFT
  PressKey('2');
  _delay_ms(100);
  PressKey('2');
  sendPS2(0xF0); // Liberamos SHIFT
  sendPS2(KEY_RSHIFT);
  PressKey(0x0D); // ENTER (13)

}

void NMI() // CTRL + ALT + F5 (NMI)
{

  sendPS2(KEY_LCTRL); // Mantenemos pulsado LCTRL
  sendPS2(KEY_LALT); // Mantenemos pulsado LALT
  PressKey(0x74); // F5 (116)

  sendPS2(0xF0); // Liberamos LALT
  sendPS2(KEY_LALT);
  sendPS2(0xF0); // Liberamos LCTRL
  sendPS2(KEY_LCTRL);
  Cursors();
  _delay_ms(200); // Retardo para evitar pulsacion INTRO por equivocacion

}

void Reset() // CTRL + ALT + Supr (Reset)
{

  sendPS2(KEY_LCTRL); // Mantenemos pulsado LCTRL
  sendPS2(KEY_LALT); // Mantenemos pulsado LALT
  PressKey(46); // Supr

  sendPS2(0xF0); // Liberamos LALT
  sendPS2(KEY_LALT);
  sendPS2(0xF0); // Liberamos LCTRL
  sendPS2(KEY_LCTRL);
  Cursors();

}

void MasterReset() // CTRL + ALT + BackSpace (MasterReset)
{

  sendPS2(KEY_LCTRL); // Mantenemos pulsado LCTRL
  sendPS2(KEY_LALT); // Mantenemos pulsado LALT
  PressKey(8); // BackSpace

  sendPS2(0xF0); // Liberamos LALT
  sendPS2(KEY_LALT);
  sendPS2(0xF0); // Liberamos LCTRL
  sendPS2(KEY_LCTRL);
  Cursors();
  _delay_ms(200); // Retardo para evitar pulsacion ESC por equivocacion

}

void ChangeKeys()
{
  keyup = keyup == KEY_UP ? KEY_Q : KEY_UP;
  keydown = keydown == KEY_DOWN ? KEY_A : KEY_DOWN;
  keyleft = keyleft == KEY_LEFT ? KEY_O : KEY_LEFT;
  keyright = keyright == KEY_RIGHT ? KEY_P : KEY_RIGHT;
}

void Cursors()
{
  keyup = KEY_UP;
  keydown = KEY_DOWN;
  keyleft = KEY_LEFT;
  keyright = KEY_RIGHT;
}


void setup()
{  

  CPU_PRESCALE(0);
  ps2Init();
  mapper = 0; // Mapa por defecto al inicio
  Cursors(); // Direcciones del joystick como cursores por defecto al inicio

  // Entrada desde DB15, activamos resistencias internas pullup en pines digitales 0 a 12
  DB15_PIN01 = 0xFF; // Ponemos en alto pines 0 - 7
  DB15_PIN02 = DB15_PIN02 | 0b00011111; // Ponemos en alto los pines 8 - 12, respetamos el contenido del resto ya que nos los utilzaremos
  DB15_PORT01 = 0; // Input pullup 0 - 7 
  DB15_PORT02 = DB15_PORT02 & 0b11100000; // Input pullup 8 - 12
    
  DB15_PINAux = 0xFFFF; // Estado inicial del joystick

}

void loop() 
{  

    DB15_PIN = (((uint16_t)DB15_PIN02 << 8) + DB15_PIN01) | 0b1110000000000000; // Organizamos los botones en 16 bits (pines digitales 0 a 12, ignorando el resto)
    
    if ((DB15_PINChanges = DB15_PIN ^ DB15_PINAux) != 0) // Esperamos a un cambio de estado del joystick
    { 
     
      DB15PINPrev = DB15_PINAux; // Almacenamos estado anterior del joystick
      DB15_PINAux = DB15_PIN; // Almacenamos en auxiliar el estado actual para esperar despues un cambio del estado del joystick

      // Cambio de mapeo en bucle
      if (DB15_PIN == 0b1111111111111111 && DB15PINPrev == 0b1110111111111111) // Se ha pulsado y soltado KeyMapper
      { 
        if (mapper > 2) // Disponemos de cuatro mapeos
        {
          mapper = 0;
        }
        else
        {
          mapper = mapper + 1;
        }          
        return; 
      } 
            
      // Combinaciones para todos los mapeos
      if (DB15_PIN == 0b1110111111101111 && DB15PINPrev == 0b1110111111111111) { mapper = 0; _delay_ms(200); return; } // Mapper 0 (KeyMapper + Boton 1) -> Inicial
      if (DB15_PIN == 0b1110111111011111 && DB15PINPrev == 0b1110111111111111) { mapper = 1; _delay_ms(200); return; } // Mapper 1 (KeyMapper + Boton 2)
      if (DB15_PIN == 0b1110111110111111 && DB15PINPrev == 0b1110111111111111) { mapper = 2; _delay_ms(200); return; } // Mapper 2 (KeyMapper + Boton 3)
      if (DB15_PIN == 0b1110111101111111 && DB15PINPrev == 0b1110111111111111) { mapper = 3; _delay_ms(200); return; } // Mapper 3 (KeyMapper + Boton 4)

      if (DB15_PIN == 0b1111110111101111 && DB15PINPrev == 0b1111110111111111) { PressKey('1'); _delay_ms(200); return; } // 1 (Select + Boton 1)
      if (DB15_PIN == 0b1111110111011111 && DB15PINPrev == 0b1111110111111111) { PressKey('2'); _delay_ms(200); return; } // 2 (Select + Boton 2)
      if (DB15_PIN == 0b1111110110111111 && DB15PINPrev == 0b1111110111111111) { PressKey('3'); _delay_ms(200); return; } // 3 (Select + Boton 3)
      if (DB15_PIN == 0b1111110101111111 && DB15PINPrev == 0b1111110111111111) { PressKey('4'); _delay_ms(200); return; } // 4 (Select + Boton 4)
      if (DB15_PIN == 0b1111110011111111 && DB15PINPrev == 0b1111110111111111) { ChangeKeys(); _delay_ms(200); return; } // (Select + Start) Cursor <-> OQPA desde keyup, keydown, keyleft y keyright en los mapeos que lo utilicen

      if (DB15_PIN == 0b1111110111110111 && DB15PINPrev == 0b1111110111111111) { PressKey(113); _delay_ms(200); return; } // F2 (Select + Derecha)
      if (DB15_PIN == 0b1111110111111011 && DB15PINPrev == 0b1111110111111111) { PressKey(20); _delay_ms(200); return; } // BloqMayus (Select + Izquierda)
      if (DB15_PIN == 0b1111110111111110 && DB15PINPrev == 0b1111110111111111) { Reset(); _delay_ms(200); return; } // Reset (Select + Arriba)
      if (DB15_PIN == 0b1111110111111101 && DB15PINPrev == 0b1111110111111111) { MasterReset(); _delay_ms(200); return; } // MasterReset (Select + Abajo)

      if (DB15_PIN == 0b1111111011111011 && DB15PINPrev == 0b1111111011111111) { LOAD128(); _delay_ms(200); return; } // Load 128K (Start + Izquierda)
      if (DB15_PIN == 0b1111111011110111 && DB15PINPrev == 0b1111111011111111) { LOAD48(); _delay_ms(200); return; } // Load 48K (Start + Derecha)
      if (DB15_PIN == 0b1111111011101111 && DB15PINPrev == 0b1111111011111111) { NMI(); _delay_ms(200); return; } // NMI (Start + Boton 1)              

      if (mapper == 0) // Mapa 0 (inicial) -> Cursores/OPQA y botones Espacio, V, B, N, G, H. Select -> ESC, Start -> Intro
      {
        // Select y Start son especiales para una mejor integracion con las combinaciones, actuan al ser pulsados y despues soltados.
        if (DB15_PIN == 0b1111111111111111 && DB15PINPrev == 0b1111111011111111) { PressKey(13); return; } // Intro (Start)
        if (DB15_PIN == 0b1111111111111111 && DB15PINPrev == 0b1111110111111111) { PressKey(27); return; } // ESC (Select)

        if (CHECK_BIT(DB15_PIN, 8) && CHECK_BIT(DB15_PIN, 9) &&
          CHECK_BIT(DB15PINPrev, 8) && CHECK_BIT(DB15PINPrev, 9)) // Ignoramos si son pulsados o recien soltados los botones Select o Start
        {
          // Para el resto, se envia pulsacion o liberacion de tecla segun el estado de los mismos.
          if (CHECK_BIT(DB15_PINChanges, 0)) sendCodeMR(keyup, CHECK_BIT(DB15_PIN, 0));
          if (CHECK_BIT(DB15_PINChanges, 1)) sendCodeMR(keydown, CHECK_BIT(DB15_PIN, 1));
          if (CHECK_BIT(DB15_PINChanges, 2)) sendCodeMR(keyleft, CHECK_BIT(DB15_PIN, 2));
          if (CHECK_BIT(DB15_PINChanges, 3)) sendCodeMR(keyright, CHECK_BIT(DB15_PIN, 3));
          if (CHECK_BIT(DB15_PINChanges, 4)) sendCodeMR(KEY_SPACE, CHECK_BIT(DB15_PIN, 4));
          if (CHECK_BIT(DB15_PINChanges, 5)) sendCodeMR(KEY_V, CHECK_BIT(DB15_PIN, 5));
          if (CHECK_BIT(DB15_PINChanges, 6)) sendCodeMR(KEY_B, CHECK_BIT(DB15_PIN, 6));
          if (CHECK_BIT(DB15_PINChanges, 7)) sendCodeMR(KEY_N, CHECK_BIT(DB15_PIN, 7));
          if (CHECK_BIT(DB15_PINChanges, 10)) sendCodeMR(KEY_G, CHECK_BIT(DB15_PIN, 10));
          if (CHECK_BIT(DB15_PINChanges, 11)) sendCodeMR(KEY_H, CHECK_BIT(DB15_PIN, 11));  
        }

      }

      /*
      if (mapper == 1) // Mapa 1 ->
      {
      }
      if (mapper == 2) // Mapa 2 ->
      {
      }
      if (mapper == 3) // Mapa 3 ->
      {
      }
      */
    }

    
}
