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
void sendCodeMR(unsigned char key, uint16_t release, double ms);
void sendCodeMRE0(unsigned char key, uint16_t release, double ms);
void PressKey(unsigned char key, double ms);
void PressKeyWithE0(unsigned char key, double ms);
void Cursors();
void LOAD128();
void LOAD48();
void NMI();
void Reset();
void MasterReset();
void ChangeKeys();
void SetMap();
unsigned char CheckDB15();

