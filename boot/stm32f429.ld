/* Linker script
 * The role of this script is to take all the object files built by the compiler
 * and produce a single binary suitable for execution.
 * Without an explicit linker script, the linker will produce a binary file that
 * would not match some of our requirements (for example, we want the code to be
 * written at a specific address (in Flash ROM) and the data at another. */

/* Let's instruct the linker about our memory layout.
 * This will let us use shortcuts such as ">FLASH" to ask for a given section
 * to be stored in Flash. */
MEMORY {
  FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 2048K
  RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 192K
  /* We're splitting the CCM in two, this way we're sure nothing will use the
   * stack space. */
  CCM (rwx) : ORIGIN = 0x10000000, LENGTH = 60K
  CCM_STACK (rw) : ORIGIN = 0x1000F000, LENGTH = 4K
}

SECTIONS {

  /* The ISR vector table (explained below) contains pointer to functions that
   * we may want to override depending on the binary we want to produce.
   * The C implementation uses _NameServiceRoutine symbols. If those symbols are
   * defined elsewhere, want to use that version. If they aren't defined, we
   * want to provide a default value. We'll define here the default value that
   * we want to be "absolute zero" (outside of any section).
   * provide here a default implementation (namely, calling crt0's _start on
   * reset, and an infinite loop on HardFault). */

  PROVIDE(_NMIServiceRoutine = 0);
  PROVIDE(_MemManageServiceRoutine = 0);
  PROVIDE(_BusFaultServiceRoutine = 0);
  PROVIDE(_UsageFaultServiceRoutine = 0);
  PROVIDE(_SVCallServiceRoutine = 0);
  PROVIDE(_DebugMonitorServiceRoutine = 0);
  PROVIDE(_PendSVServiceRoutine = 0);
  PROVIDE(_SysTickServiceRoutine = 0);
  PROVIDE(_WWDGServiceRoutine = 0);
  PROVIDE(_PVDServiceRoutine = 0);
  PROVIDE(_TampStampServiceRoutine = 0);
  PROVIDE(_RtcWakeupServiceRoutine = 0);
  PROVIDE(_FlashServiceRoutine = 0);
  PROVIDE(_RCCServiceRoutine = 0);
  PROVIDE(_EXTI0ServiceRoutine = 0);
  PROVIDE(_EXTI1ServiceRoutine = 0);
  PROVIDE(_EXTI2ServiceRoutine = 0);
  PROVIDE(_EXTI3ServiceRoutine = 0);
  PROVIDE(_EXTI4ServiceRoutine = 0);
  PROVIDE(_DMA1Stream0ServiceRoutine = 0);
  PROVIDE(_DMA1Stream1ServiceRoutine = 0);
  PROVIDE(_DMA1Stream2ServiceRoutine = 0);
  PROVIDE(_DMA1Stream3ServiceRoutine = 0);
  PROVIDE(_DMA1Stream4ServiceRoutine = 0);
  PROVIDE(_DMA1Stream5ServiceRoutine = 0);
  PROVIDE(_DMA1Stream6ServiceRoutine = 0);

  .isr_vector_table ORIGIN(FLASH) : {
    /* When booting, the STM32F42xx fetches the content of address 0x0, and
     * extracts from it various key infos: the initial value of the PC register
     * (program counter), the initial value of the stack pointer, and various
     * entry points for interrupt service routines. This data is called the
     * ISR vector table.
     *
     * Note that address 0x0 is always an alias. It points to the beginning of
     * Flash, SRAM, or integrated bootloader depending on the boot mode chosen.
     * (This mode is chosen by setting BOOTn pins on the chip).
     *
     * We're generating the ISR vector table in C, because it's very convenient:
     * using function pointers, we can easily point to the service routine for
     * each interrupt.
     * See ST/RM0090/p70 for more infos. */


    *(.isr_vector_table)
  } >FLASH


  .text : {
    . = ALIGN(4);

  /* We have to finish defining the ISR vectors that might not have been defined
   * previously. We are PROVIDing here the non-zero vectors. In other words, the
   * one where we want to point to actual code.
   * We're doing it here because we want those symbols to live in the .text
   * section. We're simply setting the Reset vector to crt0's _start, and the
   * HardFault one to crt0's _halt. */

    PROVIDE(_ResetServiceRoutine = _start);
    PROVIDE(_HardFaultServiceRoutine = _halt);

    *(.text)
  } >FLASH

  .rodata : {
    . = ALIGN(4);
    *(.rodata)
  } >FLASH

  .data : {
    /* The data section is written to Flash but linked as if it were in RAM.
     *
     * This is required because its initial value matters (so it has to be in
     * persistant memory in the first place), but it is a R/W area of memory
     * so it will have to live in RAM upon execution (in linker lingo, that
     * translate to the data section having a LMA in Flash and a VMA in RAM).
     *
     * This means we'll have to copy it from Flash to RAM on initialization.
     * To do this, we'll need to know the source location of the data section
     * (in Flash), the target location (in RAM), and the size of the section.
     * That's why we're defining three symbols that we'll use in the initial-
     * -ization routine. */
    . = ALIGN(4);
    _data_section_start_flash = LOADADDR(.data);
    _data_section_start_ram = .;
    *(.data)
    _data_section_end_ram = .;
  } >RAM AT> FLASH

  .bss : {
    /* The bss section contains data for all uninitialized variables
     * So like the .data section, it will go in RAM, but unline the data section
     * we don't care at all about an initial value.
     *
     * Before execution, crt0 will erase that section of memory though, so we'll
     * need pointers to the beginning and end of this section. */
    . = ALIGN(4);
    _bss_section_start_ram = .;
    *(.bss)
    _bss_section_end_ram = .;
  } >RAM

  .ccm : {
    . = ALIGN(8);
    _stack_end = .;
    . = . + LENGTH(CCM_STACK) - 8;
    . = ALIGN(8);
    _stack_start = .;
  } >CCM_STACK
}
