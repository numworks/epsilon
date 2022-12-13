#ifndef ION_ION_H
#define ION_ION_H

#include <ion/authentication.h>
#include <ion/backlight.h>
#include <ion/battery.h>
#include <ion/circuit_breaker.h>
#include <ion/clipboard.h>
#include <ion/console.h>
#include <ion/display.h>
#include <ion/events.h>
#include <ion/external_apps.h>
#include <ion/keyboard.h>
#include <ion/led.h>
#include <ion/persisting_bytes.h>
#include <ion/power.h>
#include <ion/reset.h>
#include <ion/storage/file_system.h>
#include <ion/timing.h>
#include <ion/usb.h>
#include <ion/unicode/utf8_decoder.h>
#include <ion/unicode/utf8_helper.h>
#include <stdint.h>
#include <string.h>
#include <setjmp.h>

/* ION is not your regular library. It is a library you link against, but it
 * will take care of configuring the whole environment for you. In POSIX terms,
 * ION will implement the "main" function.
 * Don't worry though, once all its initialization will be performed, ION will
 * jump to your code at ion_main, which you have to implement yourself. */

void ion_main(int argc, const char * const argv[]);

namespace Ion {

const char * epsilonVersion();
const char * kernelVersion();
const char * patchLevel();
const char * fccId();
constexpr static int k_serialNumberLength = 16;
const char * serialNumber();
const char * pcbVersion();
const char * compilationFlags();
const char * runningBootloader();

// CRC32 : non xor-ed, non reversed, direct, polynomial 4C11DB7
uint32_t crc32Word(const uint32_t * data, size_t length); // Only accepts whole 32bit values
uint32_t crc32Byte(const uint8_t * data, size_t length);

// Provides a true random number
uint32_t random();

// Decompress data
void decompress(const uint8_t * src, uint8_t * dst, int srcSize, int dstSize);

// Sets and returns address to the first object that can be allocated on stack
void * stackStart();
void setStackStart(void *);
// Tells whether the stack pointer is within acceptable bounds
bool stackSafe();

// Collect registers in a buffer and returns the stack pointer
uintptr_t collectRegisters(jmp_buf regs);

}

#endif
