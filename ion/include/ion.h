#ifndef ION_ION_H
#define ION_ION_H

#include <ion/backlight.h>
#include <ion/battery.h>
#include <ion/board.h>
#include <ion/clipboard.h>
#include <ion/console.h>
#include <ion/display.h>
#include <ion/events.h>
#include <ion/exam_mode.h>
#include <ion/keyboard.h>
#include <ion/led.h>
#include <ion/power.h>
#include <ion/rtc.h>
#include <ion/storage.h>
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

const char * serialNumber();
const volatile char * username();
const char * softwareVersion();
const char * upsilonVersion();
const char * omegaVersion();
const char * patchLevel();
const char * fccId();
const char * pcbVersion();
void updateSlotInfo();
const void * storageAddress();

// CRC32 : non xor-ed, non reversed, direct, polynomial 4C11DB7
uint32_t crc32Word(const uint32_t * data, size_t length); // Only accepts whole 32bit values
uint32_t crc32Byte(const uint8_t * data, size_t length);
uint32_t crc32EatByte(uint32_t previousCRC, uint8_t data);


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
