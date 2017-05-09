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
void Display(const char * input);
void Exit(const char * input);
void Keyboard(const char * input);
void LED(const char * input);
void MCUSerial(const char * input);
void Ping(const char * input);
void Suspend(const char * input);

extern const char * const sOK;
extern const char * const sKO;
extern const char * const sSyntaxError;
extern const char * const sON;
extern const char * const sOFF;

void reply(const char * s);
int8_t hexChar(char c);
bool isHex(char c);
uint32_t hexNumber(const char * s);

}
}
}
}

#endif
