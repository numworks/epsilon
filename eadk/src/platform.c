#include <stdint.h>
#include <errno.h>
#include <stddef.h>

#include "../include/eadk/eadk.h"

// Heap operations

extern char _heap_start;
extern char _heap_end;

void * _sbrk(int incr) {
  static char * heap_end = &_heap_start;
  void * last_heap_end = heap_end;
  heap_end += incr;
  if (heap_end >= (&_heap_end)) {
    errno = ENOMEM;
    return (void *)-1;
  }
  return last_heap_end;
}

// File operations
// For now those only every assume we're writing to STDOUT

#if 0
#include <sys/stat.h>
int _fstat(int file, struct stat * st) {
  st->st_mode = S_IFCHR;
  return 0;
}
#else
int _fstat(int file, void * st)   {
  return 0;
}
#endif

int _isatty(int file) {
  return 1;
}

int _open(const char * name, int flags, int mode) {
  return -1;
}

int _read(int file, char *ptr, int len) {
  return 0;
}

int _lseek(int file, int ptr, int dir) {
  return 0;
}

int _close(int file) {
  return -1;
}

int _write(int file, char * ptr, int len) {
  static eadk_point_t location = {0, 0};
  //location = eadk_display_draw_string(ptr, location, false, eadk_color_black, eadk_color_white, len); // Ideal scenario: returns location, takes len as input
  eadk_display_draw_string(ptr, location, false, eadk_color_black, eadk_color_white);
  return len;
}

// C runtime initialization

extern char _data_section_start_flash;
extern char _data_section_start_ram;
extern char _data_section_end_ram;
extern char _bss_section_start_ram;
extern char _bss_section_end_ram;

/* We use an inline version of memcpy/memset in order not to interfere with any
 * symbol that could be defined in the actual external app. */

static inline void _eadk_memcpy(void * dst, const void * src, size_t n) {
  char * destination = (char *)dst;
  char * source = (char *)src;
  while (n--) {
    *destination++ = *source++;
  }
}

static inline void _eadk_memset(void * b, int c, size_t len) {
  char * destination = (char *)b;
  while (len--) {
    *destination++ = (unsigned char)c;
  }
}

void main(int argc, char * argv[]);

void _start() {
  /* Copy data section to RAM
   * The data section is R/W but its initialization value matters. It's stored
   * in Flash, but linked as if it were in RAM. Now's our opportunity to copy
   * it. Note that until then the data section (e.g. global variables) contains
   * garbage values and should not be used. */
  size_t dataSectionLength = (&_data_section_end_ram - &_data_section_start_ram);
  _eadk_memcpy(&_data_section_start_ram, &_data_section_start_flash, dataSectionLength);

  /* Zero-out the bss section in RAM
   * Until we do, any uninitialized global variable will be unusable. */
  size_t bssSectionLength = (&_bss_section_end_ram - &_bss_section_start_ram);
  _eadk_memset(&_bss_section_start_ram, 0, bssSectionLength);

  main(0, NULL);
}
