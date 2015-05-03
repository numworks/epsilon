#include <stdint.h>
#include <string.h>
// FIXME: Use a libc, and memset, bzerto!

extern const void * _data_section_start_flash;
extern const void * _data_section_start_ram;
extern const void * _data_section_end_ram;
extern const void * _bss_section_start_ram;
extern const void * _bss_section_end_ram;
extern const void * _stack_start;
extern const void * _stack_end;

void _start(void);

/* Interrupt Service Routines are void->void functions */
typedef void(*ISR)(void);

/* Notice: The Cortex-M4 expects all jumps to be made at an odd address when
 * jumping to Thumb code. For example, if you want to execute Thumb code at
 * address 0x100, you'll have to jump to 0x101. Luckily, this idiosyncrasy is
 * properly handled by the C compiler that will generate proper addresses when
 * using function pointers. */

#define INITIALISATION_VECTOR_SIZE 0x6B

ISR InitialisationVector[INITIALISATION_VECTOR_SIZE]
  __attribute__((section(".isr_vector_table")))
  = {
  (ISR)&_stack_start,
  _start,
  0,
  0,
  0
};

int main(int argc, char * argv[]);

void _start(void) {
  // This is where execution starts after reset.
  // Many things are not initialized yet so the code here has to pay attention.

  /* Copy data section to RAM
   * The data section is R/W but its initialization value matters. It's stored
   * in Flash, but linked as if it were in RAM. Now's our opportunity to copy
   * it. Note that until then the data section (e.g. global variables) contains
   * garbage values and should not be used. */
  size_t dataSectionLength = (char *)&_data_section_end_ram - (char *)&_data_section_start_ram;
  memcpy(&_data_section_start_ram, &_data_section_start_flash, dataSectionLength);

  /* Zero-out the bss section in RAM
   * Until we do, any uninitialized global variable will be unusable. */
  size_t bssSectionLength = (char *)&_bss_section_end_ram - (char *)&_bss_section_start_ram;
  memset(&_bss_section_start_ram, 0, bssSectionLength);

  main(0, 0x0);
}
