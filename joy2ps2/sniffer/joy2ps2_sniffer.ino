#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>

#define PS2_PIN   PINC
#define PS2_DAT   PC3
#define PS2_CLK   PC2
#define PS2_PORT  PORTC
#define PS2_DDR   DDRC
#define CK1 20
#define CK2 40
#define HI 1
#define LO 0
#define CPU_PRESCALE(n) (CLKPR = 0x80, CLKPR = (n))

unsigned char code;
int result;

void ps2Mode(uint8_t pin, uint8_t mode)
{
  if (mode) { //high
    PS2_DDR &= ~_BV(pin); //input (Truco DDR. Como input sin estado, se pone en modo Hi-Z)
  }
  else { //low
    PS2_DDR |= _BV(pin); //output (Truco DDR. Como output, se pone a 0v)
  }
}

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

int get_code_from_host(unsigned char *ret)
{
  unsigned char data = 0x00;
  unsigned char p = 0x01;

  // discard the start bit
  while ((PS2_PIN & (1 << PS2_CLK)));
  while (!(PS2_PIN & (1 << PS2_CLK)));
  
  // read each data bit
  for (int i=0; i<8; i++) {
    while ((PS2_PIN & (1 << PS2_CLK)));
    if ((PS2_PIN & (1 << PS2_DAT))) {
      data = data | (1 << i);
      p = p ^ 1;
    }
    while (!(PS2_PIN & (1 << PS2_CLK)));
  }
  
  // read the parity bit
  while ((PS2_PIN & (1 << PS2_CLK)));
  if (((PS2_PIN & (1 << PS2_DAT)) != 0) != p) {
    return -1;
  }
  while (!(PS2_PIN & (1 << PS2_CLK)));
  
  // discard the stop bit
  while ((PS2_PIN & (1 << PS2_CLK)));
  while (!(PS2_PIN & (1 << PS2_CLK)));
  
  *ret = data;
  return 0;
}

int get_code_from_kb(unsigned char *ret)
{
  unsigned char data = 0x00;
  unsigned char p = 0x01;

  // discard the start bit
  while (!(PS2_PIN & (1 << PS2_CLK)));
  while ((PS2_PIN & (1 << PS2_CLK)));
  
  // read each data bit
  for (int i=0; i<8; i++) {
    while (!(PS2_PIN & (1 << PS2_CLK)));
    if ((PS2_PIN & (1 << PS2_DAT))) {
      data = data | (1 << i);
      p = p ^ 1;
    }
    while ((PS2_PIN & (1 << PS2_CLK)));
  }
  
  // read the parity bit
  while (!(PS2_PIN & (1 << PS2_CLK)));
  if (((PS2_PIN & (1 << PS2_DAT)) != 0) != p) {
    return -1;
  }
  while ((PS2_PIN & (1 << PS2_CLK)));
  
  // discard the stop bit
  while (!(PS2_PIN & (1 << PS2_CLK)));
  while ((PS2_PIN & (1 << PS2_CLK)));

  // 'ack' bit
  while (!(PS2_PIN & (1 << PS2_CLK)));
  while ((PS2_PIN & (1 << PS2_CLK)));
  
  *ret = data;
  return 0;
}

int listen() 
{

  unsigned char ret;

    // wait for a frame
  while ((PS2_PIN & (1 << PS2_DAT)));
  // check to see who's sending
  if ((PS2_PIN & (1 << PS2_CLK))) { // device-to-host
    get_code_from_host(&ret);
    Serial.print("Keyboard: ");
    Serial.println(ret,HEX);
    return 0;
  } else { // host-to-device
    get_code_from_kb(&ret);
    Serial.print("Host: ");
    Serial.println(ret,HEX);
    return 1;
  }
  Serial.println("Error comm.");
  return -1;

}

void setup() {
  CPU_PRESCALE(0);
  ps2Init();

  Serial.begin(9600);
  Serial.println("Ready.");

}

void loop() {

listen();

}
