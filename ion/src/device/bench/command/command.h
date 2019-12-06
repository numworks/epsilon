#ifndef ION_DEVICE_BENCH_COMMAND_COMMAND_H
#define ION_DEVICE_BENCH_COMMAND_COMMAND_H

#include <stdint.h>

namespace Ion {
namespace Device {
namespace Bench {
namespace Command {

typedef void (*Function)(const char * input);

void ADC(const char * input);
void Backlight(const char * input);
void Charge(const char * input);
void CRC(const char * input);
void Display(const char * input);
void Exit(const char * input);
void ExternalFlashId(const char * input);
void Jump(const char * input);
void Keyboard(const char * input);
void LCDData(const char * input);
void LCDPins(const char * input);
void LCDTiming(const char * input);
void LED(const char * input);
void MCUSerial(const char * input);
void Ping(const char * input);
void Print(const char * input);
void ScreenID(const char * input);
void Sleep(const char * input);
void Stop(const char * input);
void Standby(const char * input);
void USBPlugged(const char * input);
void VBlank(const char * input);

extern const char * const sOK;
extern const char * const sKO;
extern const char * const sSyntaxError;
extern const char * const sON;
extern const char * const sOFF;

void reply(const char * s);
int8_t hexChar(char c);
bool isHex(char c);
uint32_t numberBase10(const char * s, int maxLength = -1);
uint32_t hexNumber(const char * s, int maxLength = -1);

}
}
}
}

#endif
