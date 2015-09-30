#include <stdint.h>
#include <string.h>
#include <ion.h>
#include "../init.h"

extern char _data_section_start_flash;
extern char _data_section_start_ram;
extern char _data_section_end_ram;
extern char _bss_section_start_ram;
extern char _bss_section_end_ram;

void abort() {
#ifdef DEBUG
  while (1) {
  }
#else
  ion_reset();
#endif
}

void _start(void) {
  // This is where execution starts after reset.
  // Many things are not initialized yet so the code here has to pay attention.

  /* Copy data section to RAM
   * The data section is R/W but its initialization value matters. It's stored
   * in Flash, but linked as if it were in RAM. Now's our opportunity to copy
   * it. Note that until then the data section (e.g. global variables) contains
   * garbage values and should not be used. */
  size_t dataSectionLength = (&_data_section_end_ram - &_data_section_start_ram);
  memcpy(&_data_section_start_ram, &_data_section_start_flash, dataSectionLength);

  /* Zero-out the bss section in RAM
   * Until we do, any uninitialized global variable will be unusable. */
  size_t bssSectionLength = (&_bss_section_end_ram - &_bss_section_start_ram);
  memset(&_bss_section_start_ram, 0, bssSectionLength);

  init_platform();

  ion_app();

  abort();
}
