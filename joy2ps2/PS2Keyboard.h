#include <inttypes.h>
#include "keymaps.h"

void my_delay_ms_10ms_steps(int ms);
void QueuePS2Init(void);
void QueuePS2Put(unsigned char sc, double ms);
void QueuePS2Get(unsigned char *old_sc, double *old_ms);
void ps2Mode(uint8_t pin, uint8_t mode);
void ps2Init();
uint8_t ps2Stat();
void FreeKBBuffer();
void sendPS2(unsigned char code, double ms);
void sendPS2fromqueue(unsigned char code);
int checkState(int t);
int getPS2(unsigned char *ret);
void sendCodeMR(unsigned char key, uint16_t release, double ms, unsigned char scancodeset);
void PressKey(unsigned char key, double ms, unsigned char scancodeset);
void Cursors();
void LOAD128(unsigned char scancodeset);
void LOAD48(unsigned char scancodeset);
void NMI(unsigned char scancodeset);
void Reset(unsigned char scancodeset);
void MasterReset(int extra, unsigned char scancodeset);
void ChangeKeys();
void SetMapP1(int map);
void SetMapP2(int map);