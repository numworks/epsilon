/* Linker script
 * The role of this script is to take all the object files built by the compiler
 * and produce a single binary suitable for execution.
 * Without an explicit linker script, the linker will produce a binary file that
 * would not match some of our requirements (for example, we want the code to be
 * written at a specific address (in Flash ROM) and the data at another. */

/* Let's instruct the linker about our memory layout.
 * This will let us use shortcuts such as ">INTERNAL_FLASH" to ask for a given section to
 * be stored in Flash. */

MEMORY {
  INTERNAL_FLASH (rx) : ORIGIN = 0x00200000, LENGTH = 64K
  SRAM (rw)  : ORIGIN = 0x20000000, LENGTH = 256K
  EXTERNAL_FLASH (rx) : ORIGIN = 0x90000000, LENGTH = 8M
  /*
  ITCM (rwx) : ORIGIN = 0x00000000, LENGTH = 16K
  DTCM (rwx) : ORIGIN = 0x20000000, LENGTH = 64K
  SRAM1 (rwx) : ORIGIN = 0x20010000, LENGTH = 176K
  SRAM2 (rwx) : ORIGIN = 0x2003C000, LENGTH = 16K
   */
}

STACK_SIZE = 32K;
FIRST_EXTERNAL_FLASH_SECTOR_SIZE = 4K;

SECTIONS {
  .isr_vector_table ORIGIN(INTERNAL_FLASH) : {
    /* When booting, the STM32F412 fetches the content of address 0x0, and
     * extracts from it various key infos: the initial value of the PC register
     * (program counter), the initial value of the stack pointer, and various
     * entry points to interrupt service routines. This data is called the ISR
     * vector table.
     *
     * Note that address 0x0 is always an alias. It points to the beginning of
     * Flash, SRAM, or integrated bootloader depending on the boot mode chosen.
     * (This mode is chosen by setting the BOOTn pins on the chip).
     *
     * We're generating the ISR vector table in code because it's very
     * convenient: using function pointers, we can easily point to the service
     * routine for each interrupt. */
    KEEP(*(.isr_vector_table))
  } >INTERNAL_FLASH

  .header : {
    KEEP(*(.header))
  } >INTERNAL_FLASH

  /* Internal flash memory */

  /* We link 'jump_to_external_flash' in a separate section from text.internal.
   * Indeed, it is the only internal flash symbol that refers the external
   * flash. This way we can assert that there are no cross references from the
   * internal flash to the external flash. */
  .text.internal_to_external : {
    *(.text._ZL22jump_to_external_flashv*)
  } >INTERNAL_FLASH

  /* Use boot routine and required dependencies */
  /* We're relying on symbols being in their own sub-section. On GCC, this is
   * done with -fdata-sections -ffunction-sections */
  .text.internal : {
    . = ALIGN(4);
    *(.text.start)
    *(.text.abort)
    *(.text.uf_abort)
    *(.text.bf_abort)
    *(.text.nmi_abort)
    *(.text.abort_init)
    *(.text.abort_core)
    *(.text.abort_sleeping)
    *(.text.abort_economy)
    *(.text.abort_screen)
    *(.text.isr_systick)
    *(.text.__assert)
    *(.text.memcpy)
    *(.text.memset)
    /*
    *(.text.strlen)
    *(.text.strncmp)
    *(.text.strlcpy)
    *(.text.strcmp)
    *(.text.memmove)
    */
    /* 'start' dependencies */
    *(.text._ZN3Ion6Device5Board4initEv)
    *(.text._ZN3Ion6Device5Board7initFPUEv)
    *(.text._ZN3Ion6Device5Board7initMPUEv)
    *(.text._ZN3Ion6Device5Board10initClocksEv)
    *(.text._ZNV3Ion6Device4Regs*)
    *(.text._ZN3Ion6Device4Regs*)
    *(.text.*3Ion6Device4Regs4GPIO5PUPDR*)
    *(.text._ZNK3Ion6Device4Regs9AFGPIOPin*)
    *(.text._ZN3Ion6Device13ExternalFlash*)
    *(.text._ZN3Ion6Device5Cache*)
    *(.text._ZN3Ion6Timing6usleepEj)

    /* 'abort' dependencies */
    *(.text._ZN3Ion6Device5Reset4coreEv)
    *(.text._ZN3Ion3LED8setColorE7KDColor)
    *(.text._ZN3Ion3LED11setBlinkingEtf)
    *(.text._ZN3Ion6Device3LED*)
    *(.text._ZNK3Ion6Device4Regs3TIMINS1_8RegisterItEEE4CCMREv)
    *(.text._ZNK3Ion6Device4Regs3TIMINS1_8RegisterItEEE4BaseEv)
    *(.text._ZNK3Ion6Device4Regs3*)
    *(.text.___ZN3Ion6Device4Regs8*)
    *(.text._ZNK3Ion6Device4Regs3TIMINS1_8RegisterItEEE4CCMREv)
    *(.text._ZNK3Ion6Device4Regs3TIMINS1_8RegisterItEEE4CCMREv*)
    *(.text._ZN3Ion6Device5Board15initPeripheralsEb)
    *(.text._ZN3Ion6Device9Backlight4initEv)
    *(.text._ZN3Ion6Device6Timing4initEv)
    *(.text._ZN3Ion6Timing6msleepEj)
    *(.text._ZN3Ion6Device8Keyboard4initEv)
    *(.text._ZN3Ion6Device7Battery8initGPIOEv)
    *(.text._ZN3Ion6Device3USB8initGPIOEv)
    *(.text._ZN3Ion6Device9Backlight8setLevelEh)
    *(.text._ZN3Ion6Device6Timing19setSysTickFrequencyEi)
    *(.text._ZN3Ion6Device5Board17setClockFrequencyENS1_9FrequencyE)
    *(.text._ZN3Ion6Device9Backlight10sendPulsesEi)
    *(.text._ZN3Ion6Device5Board19shutdownPeripheralsEb)
    *(.text._ZN3Ion6Device6Timing8shutdownEv)
    *(.text._ZN3Ion6Device8Keyboard8shutdownEv)
    *(.text._ZN9KDContext10drawStringEPKc7KDPointPK6KDFont7KDColorS6_i)
    *(.text._ZNK8TextArea11ContentView12drawStringAtEP9KDContextiiPKci7KDColorS5_S4_S4_S5_*)
    *(.text._ZL11KDPointZero*)
    *(.text._ZGVZN12KDIonContext13sharedContextEvE7context)
    *(.text._ZZN12KDIonContext13sharedContextEvE7context)
    *(.text._ZN12KDIonContext13sharedContextEv)
    *(.text._ZN20KDPostProcessContext15pushRectUniformE6KDRect7KDColor)
    *(.text._ZN26KDPostProcessInvertContext15pushRectUniformE6KDRect7KDColor)
    *(.text._ZN12KDIonContext15pushRectUniformE6KDRect7KDColor)
    *(.text._ZN24KDPostProcessZoomContext15pushRectUniformE6KDRect7KDColor)
    *(.text._ZN25KDPostProcessGammaContext15pushRectUniformE6KDRect7KDColor)
    *(.text._ZN16KDRealIonContext15pushRectUniformE6KDRect7KDColor)
    *(.text._ZN3Ion7Display15pushRectUniformE6KDRect7KDColor)
    *(.text._ZNK6KDRect15intersectedWithERKS_)
    *(.text._ZN7KDColor6RGB888Ehhh)
    *(.text._ZN3Ion6Device7Display14setDrawingAreaE6KDRectNS1_11OrientationE)
    *(.text.powf)
    *(.text._ZN9KDContext16pushOrPullStringEPKc7KDPointPK6KDFont7KDColorS6_ibPi)
    *(.text._ZNK7KDPoint12translatedByES_)
    *(.text._ZNK6KDRect12translatedByE7KDPoint)
    *(.text._Z7toGammai)
    *(.text._ZN3Ion7Display8pullRectE6KDRectP7KDColor)
    *(.text._ZN24KDPostProcessZoomContext8pullRectE6KDRectP7KDColor)
    *(.text._ZN16KDRealIonContext8pullRectE6KDRectP7KDColor)
    *(.text._ZN25KDPostProcessGammaContext8pullRectE6KDRectP7KDColor)
    *(.text._ZN12KDIonContext8pullRectE6KDRectP7KDColor)
    *(.text._ZN26KDPostProcessInvertContext8pullRectE6KDRectP7KDColor)
    *(.text._ZN20KDPostProcessContext8pullRectE6KDRectP7KDColor)
    *(.text.sqrtf)
    *(.text._ZN11UTF8Decoder13nextCodePointEv)
    *(.text._ZN7KDColor5blendES_S_h)
    *(.text._ZN9KDContext17blendRectWithMaskE6KDRect7KDColorPKhPS1_)
    *(.text.scalbnf)
    *(.text._ZNK6KDRect10intersectsERKS_)
    *(.text._ZN9KDContext18fillRectWithPixelsE6KDRectPK7KDColorPS1_)
    *(.text._ZN9KDContext15setClippingRectE6KDRect)
    *(.text._ZN9KDContext9setOriginE7KDPoint)
    *(.text._ZN20KDPostProcessContext9setOriginE7KDPoint)
    *(.text._ZN20KDPostProcessContext15pushRectUniformE6KDRect7KDColor)
    *(.text._ZN26KDPostProcessInvertContext15pushRectUniformE6KDRect7KDColor)
    *(.text._ZN20KDPostProcessContext8pushRectE6KDRectPK7KDColor)
    *(.text._ZN12KDIonContext15pushRectUniformE6KDRect7KDColor)
    *(.text._ZN12KDIonContext8pushRectE6KDRectPK7KDColor)
    *(.text._ZN26KDPostProcessInvertContext8pushRectE6KDRectPK7KDColor)
    *(.text._ZN24KDPostProcessZoomContext15pushRectUniformE6KDRect7KDColor)
    *(.text._ZN24KDPostProcessZoomContext8pushRectE6KDRectPK7KDColor)
    *(.text._ZN25KDPostProcessGammaContext15pushRectUniformE6KDRect7KDColor)
    *(.text._ZN25KDPostProcessGammaContext8pushRectE6KDRectPK7KDColor)
    *(.text._ZN16KDRealIonContext15pushRectUniformE6KDRect7KDColor)
    *(.text._ZN16KDRealIonContext8pushRectE6KDRectPK7KDColor)
    *(.text._ZN3Ion7Display8pushRectE6KDRectPK7KDColor)
    *(.text._ZN3Ion7Display15pushRectUniformE6KDRect7KDColor)
    *(.text._ZNK6KDRect7isEmptyEv)
    *(.text._ZN20KDPostProcessContext15setClippingRectE6KDRect)
    *(.text._ZNK6KDFont17indexForCodePointE9CodePoint)
    *(.text._ZNK6KDFont26fetchGrayscaleGlyphAtIndexEhPh)
    *(.text.LZ4_decompress_safe*)
    *(.text.LZ4_wildCopy*)
    *(.text.*DFU*)
    *(.text.*isEnumerated*)
    *(.text._ZN3Ion3USB6enableEv)
    *(.text._ZN3Ion7Battery5levelEv)
    *(.text._ZN3Ion7Battery7voltageEv)
    *(.text._ZN3Ion3USB9isPluggedEv)
    *(.text.*sleepConfiguration*)
    *(.text.*onOnOffKeyDown*)
    *(.text.*WakeUp*)
    *(.text._ZN3Ion6Device9Backlight*)
    *(.text._ZN3Ion9Backlight*)

    /* Rodata Truc Relou */
    *(.text._ZNK10Statistics5Store6medianEi)
    *(.text._ZNK10Regression5Store12meanOfColumnEiib)
    *(.text._ZNK6Shared15DoublePairStore11sumOfColumnEiib)
    *(.text._ZNK10Statistics5Store13firstQuartileEi)
    *(.text._ZNK10Regression5Store23squaredValueSumOfColumnEiib)
    *(.text._ZNK10Regression5Store16varianceOfColumnEiib)
    *(.text._ZNK10Statistics5Store8maxValueEi)
    *(.text._ZNK10Regression5Store25standardDeviationOfColumnEiib)
    *(.text._ZNK10Statistics5Store13thirdQuartileEi)
    *(.text._ZNK10Statistics5Store8minValueEi)
    *(.text._ZNK6Shared8Interval18IntervalParameters*)
    *(.text._ZN6Shared8Interval18IntervalParameters*)
    *(.text.sqrt)
    *(.text.log)
    *(.text._ZN17GlobalPreferences23sharedGlobalPreferencesEv)
    *(.text._ZNK10Statistics5Store16sumOfOccurrencesEi)
    *(.text.floor)
    *(.text.ceil)
    *(.text._ZNK10Statistics5Store21frequenciesAreIntegerEi)
    *(.text._ZNK10Statistics5Store34sortedElementAtCumulatedPopulationEidb)
    *(.text._ZNK10Statistics5Store33sortedElementAtCumulatedFrequencyEidb)
    *(.text.round)
    *(.text._ZNK10Statistics5Store8minIndexEPdi*)
    *(.text.LZ4_decompress_safe*)

    /* 'standby' dependencies '*/
    *(.text._ZN3Ion6Device5Power20internalFlashStandbyEv)
    *(.text._ZN3Ion6Device5Power17enterLowPowerModeEv)
    *(.text._ZN3Ion6Device5Board14shutdownClocksEb)
    *(.text._ZN3Ion6Device5Reset16coreWhilePluggedEv)

    /* 'suspend' dependencies '*/
    *(.text._ZN3Ion6Device5Power20internalFlashSuspendEb)

    /* 'Reset::jump' dependencies '*/
    *(.text._ZN3Ion6Device5Reset17internalFlashJumpEj)

    /* Optimization */
    */libgcc.a:(.text)
  } >INTERNAL_FLASH


  .rodata.internal : {
    . = ALIGN(4);
    *(.rodata._ZN3Ion6Device13ExternalFlash*)
    /* 'start' dependencies */
    *(.rodata._ZN3Ion6Device4RegsL5GPIOAE)
    *(.rodata._ZN3Ion6Device4RegsL5GPIOBE)
    *(.rodata._ZN3Ion6Device3LED6ConfigL7RGBPinsE)
    *(.rodata._ZN3Ion6Device5Board4initEv.str1.4)
    *(.rodata._ZL12KDColorWhite*)
    *(.rodata._ZL10KDColorRed*)
    *(.rodata._ZL12KDColorBlack*)
    *(.rodata._ZN4CodeL15BackgroundColorEd*)
    *(.rodata._ZN3Ion6Device3SWD6ConfigL4PinsE)
    *(.rodata._ZN3Ion6Device7Display6ConfigL8FSMCPinsE)
    *(.rodata._ZZN3Ion6Device7Display9initPanelEvE11calibration)
    *(.rodata._ZN3Ion6Device3USB6ConfigL5DmPinE)
    *(.rodata._ZN3Ion6Device3USB6ConfigL5DpPinE)
    *(.rodata._ZN3Ion6Device3USB6ConfigL7VbusPinE)
    *(.rodata._ZN3Ion6Device8Keyboard6ConfigL10ColumnPinsE)
    *(.rodata._ZN3Ion6Device8Keyboard6ConfigL7RowPinsE)
    *(.rodata._ZZN3Ion6Device3USB12shutdownGPIOEvE4Pins)
    *(.rodata._ZN6KDFont16privateLargeFontE)
    *(.rodata.abort.str1.1)
    *(.rodata.uf_abort.str1.1)
    *(.rodata.bf_abort.str1.1)
    *(.rodata.nmi_abort.str1.1)
    *(.rodata.abort_screen.str1.1)
    *(.rodata._ZL5table*)
    *(.rodata._ZL15glyphDataOffset*)
    *(.rodata._ZL11KDPointZero*)
    *(.rodata._ZL9glyphData*)
    *(.rodata._ZN4CodeL14HighlightColorE*)
    *(.rodata._ZTV12KDIonContext)
    *(.rodata._ZTV16KDRealIonContext)
    *(.rodata._ZTV24KDPostProcessZoomContext)
    *(.rodata._ZTV25KDPostProcessGammaContext)
    *(.rodata._ZTV26KDPostProcessInvertContext)
    *(.rodata.bp)
    *(.rodata.dp_h)
    *(.rodata.dp_l)
    *(.rodata._ZN11MicroPython5Color5ParseEPvNS0_4ModeE*)
    *(.rodata._ZN9Clipboard10storedTextEv*)
    *(.rodata._ZN8Sequence14ListController27toolboxForInputEventHandlerEP17InputEventHandler*)
    *(.rodata._ZN8Sequence23TypeParameterController25willDisplayCellAtLocationEP13HighlightCellii*)
    *(.rodata._ZN6KDFont16privateSmallFontE)
    *(.rodata._ZN4I18nL23CountryPreferencesArrayE)
    *(.rodata._ZN3Ion6Device3LED6ConfigL7RGBPinsE*)
    *(.rodata._ZN4I18nL23CountryPreferencesArrayE*)
    *(.rodata._ZN3Ion6Device3USB6ConfigL7VbusPinE*)
    *(.rodata.bp*)
    *(.rodata.dp_l*)
    *(.rodata.dp_h*)
    *(.rodata.abort_sleeping.str1.1)
    *(.rodata.abort_core.str1.1)
    *(.rodata.dfu_bootloader)
    *(.rodata)
  } >INTERNAL_FLASH

  .exam_mode_buffer ORIGIN(EXTERNAL_FLASH) : {
    _exam_mode_buffer_start = .;
    KEEP(*(.exam_mode_buffer))
    /* Note: We don't increment "." here, we set it. */
    . = ORIGIN(EXTERNAL_FLASH) + FIRST_EXTERNAL_FLASH_SECTOR_SIZE;
    _exam_mode_buffer_end = .;
  } >EXTERNAL_FLASH

  /* External flash memory */
  .text.external : {
    . = ALIGN(4);
    *(.text)
    *(.text.*)
  } >EXTERNAL_FLASH

  .rodata.external : {
    *(.rodata)
    *(.rodata.*)
  } >EXTERNAL_FLASH

  .init_array : {
    . = ALIGN(4);
    _init_array_start = .;
    KEEP (*(.init_array*))
    _init_array_end = .;
  } >INTERNAL_FLASH

  .data : {
    /* The data section is written to Flash but linked as if it were in RAM.
     *
     * This is required because its initial value matters (so it has to be in
     * persistant memory in the first place), but it is a R/W area of memory
     * so it will have to live in RAM upon execution (in linker lingo, that
     * translates to the data section having a LMA in Flash and a VMA in RAM).
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
    *(.data.*)
    _data_section_end_ram = .;
  } >SRAM AT> INTERNAL_FLASH

  .bss : {
    /* The bss section contains data for all uninitialized variables
     * So like the .data section, it will go in RAM, but unlike the data section
     * we don't care at all about an initial value.
     *
     * Before execution, crt0 will erase that section of memory though, so we'll
     * need pointers to the beginning and end of this section. */
    . = ALIGN(4);
    _bss_section_start_ram = .;
    *(.bss)
    *(.bss.*)
    /* The compiler may choose to allocate uninitialized global variables as
     * COMMON blocks. This can be disabled with -fno-common if needed. */
    *(COMMON)
    _bss_section_end_ram = .;
  } >SRAM

  .heap : {
    _heap_start = .;
    /* Note: We don't increment "." here, we set it. */
    . = (ORIGIN(SRAM) + LENGTH(SRAM) - STACK_SIZE);
    _heap_end = .;
  } >SRAM

  .stack : {
    . = ALIGN(8);
    _stack_end = .;
    . += (STACK_SIZE - 8);
    . = ALIGN(8);
    _stack_start = .;
  } >SRAM

  /DISCARD/ : {
    /* exidx and extab are needed for unwinding, which we don't use */
    *(.ARM.exidx*)
    *(.ARM.extab*)
  }
}

NOCROSSREFS_TO(.text.external .text.internal);
NOCROSSREFS_TO(.rodata.external .text.internal);
NOCROSSREFS_TO(.text.external .rodata.internal);
NOCROSSREFS_TO(.rodata.external .rodata.internal);
NOCROSSREFS_TO(.text.external .isr_vector_table);
NOCROSSREFS_TO(.rodata.external .isr_vector_table);
NOCROSSREFS_TO(.text.external .header);
NOCROSSREFS_TO(.rodata.external .header);

NOCROSSREFS_TO(.exam_mode_buffer .text.internal);
NOCROSSREFS_TO(.exam_mode_buffer .rodata.internal);
NOCROSSREFS_TO(.exam_mode_buffer .isr_vector_table);
NOCROSSREFS_TO(.exam_mode_buffer .header);
