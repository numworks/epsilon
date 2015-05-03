typedef long uint32_t; //FIXME: Extrude this in another file, and assert it

extern const void * _data_segment_start_flash;
extern const void * _data_segment_start_ram;
extern const void * _data_segment_end_ram;

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
  0x20010000, //FIXME: This is the stack pointer!
  _start,
  0,
  0,
  0
};

int main(int argc, char * argv[]);

void _start(void) {
  // This is where execution starts after reset.
  // Many things are not initialized yet so the code here has to pay attention.

  /* Copy data segment to RAM
   * The data segment is R/W but its initialization value matters. It's stored
   * in Flash, but linked as if it were in RAM. Now's our opportunity to copy
   * it. Note that until then the data segment (e.g. global variables) contains
   * garbage values and should not be used. */
//  int dataSegmentSize = &_data_segment_end_ram - &_data_segment_start_ram;
  uint32_t * ramPointer = (uint32_t *)&_data_segment_start_ram;
  uint32_t * flashPointer = (uint32_t *)&_data_segment_start_flash;
  while (ramPointer < (uint32_t *)&_data_segment_end_ram) {
    *ramPointer++ = *flashPointer++;
  }

  main(0, 0x0);
}
