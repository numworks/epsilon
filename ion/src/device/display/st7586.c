#include <assert.h>
#include "st7586.h"

enum {
  COMMAND_MODE = 0,
  DATA_MODE = 1,
  DELAY_MODE = 2
};

enum {
  NOP = 0x00,
  RESET = 0x01,
  SLEEP_IN = 0x10,
  SLEEP_OUT = 0x11,
  DISPLAY_OFF = 0x28,
  DISPLAY_ON = 0x29,
  SET_COLUMN_ADDRESS = 0x2A,
  SET_ROW_ADDRESS = 0x2B,
  WRITE_DISPLAY_DATA = 0x2C,
  ENABLE_DDRAM = 0x3A,
};

typedef struct {
  char mode;
  char payload;
} instruction_t;

#define COMMAND(c) (instruction_t){.mode = (char)COMMAND_MODE, .payload = (char)c}
#define DATA(d) (instruction_t){.mode = (char)DATA_MODE, .payload = (char)d}
#define DELAY(m) (instruction_t){.mode = (char)DELAY_MODE, .payload = (char)m}

static void perform_instruction(st7586_t * c, instruction_t instruction) {
  if (instruction.mode == DELAY_MODE) {
    // FIXME: Should sleep instruction->payload miliseconds
    for (int i = 0; i < 800*instruction.payload; i++) {
      __asm volatile("nop");
    }
  } else {
    c->data_command_pin_write(instruction.mode);
    c->spi_write(&instruction.payload, 1);
  }
}

static void perform_instructions(st7586_t * c, const instruction_t * instructions, size_t length) {
  for (size_t i=0; i<length; i++) {
    perform_instruction(c, instructions[i]);
  }
}

void st7586_initialize(st7586_t * controller) {
  // Falling edge on CSX
  controller->chip_select_pin_write(0);

  const instruction_t init_sequence[] = {
#define USE_MANUFACTURER_INIT 0
#if USE_MANUFACTURER_INIT
    COMMAND(0x01), // ADDED BY ME
    DELAY(220),
    COMMAND(0x11), // Sleep Out
    COMMAND(0x28), // Display OFF
    DELAY(50),
    COMMAND(0xC0), // Vop = B9h
    DATA(0x45),
    DATA(0x01),
    COMMAND(0xC3), // BIAS = 1/14
    DATA(0x00),
    COMMAND(0xC4), // Booster = x8
    DATA(0x07),
    COMMAND(0xD0), // Enable Analog Circuit
    DATA(0x1D),
    COMMAND(0xB5), // N-Line = 0
    DATA(0x00),
    //COMMAND(0x39), // Monochrome Mode
    COMMAND(0x38), // Grey Mode
    COMMAND(0x3A), // Enable DDRAM Interface
    DATA(0x02),
    COMMAND(0x36), // Scan Direction Setting
    DATA(0xc0),   //COM:C160--C1   SEG: SEG384-SEG1
    COMMAND(0xB0), // Duty Setting
    DATA(0x9F),
    COMMAND(0x20), // Display Inversion OFF
    COMMAND(0x2A), // Column Address Setting
    DATA(0x00), // SEG0 -> SEG384
    DATA(0x00),
    DATA(0x00),
    DATA(0x7F),
    COMMAND(0x2B), // Row Address Setting
    DATA(0x00), // COM0 -> COM160
    DATA(0x00),
    DATA(0x00),
    DATA(0x9F),
    COMMAND(0x29) // Display ON
#else
    COMMAND(RESET), DELAY(50), // Software reset, then wait 5ms
    COMMAND(DISPLAY_OFF),

    // Sleep out, requires a 100ms delay
    COMMAND(SLEEP_OUT), DELAY(100),

    // Display on, requires a 20ms delay
    COMMAND(DISPLAY_ON), DELAY(20),

    // Enable DDRAM interface
    // The "DATA" makes no real sense but is mandatory according to the spec
    COMMAND(ENABLE_DDRAM), DATA(0x2),


    // This has been copy-pasted from the manufacturer.
    // FIXME: Understand what it does, and maybe fix it!
    COMMAND(0xC0), // Vop = B9h
    DATA(0x45),
    DATA(0x01),
    COMMAND(0xC3), // BIAS = 1/14
    DATA(0x00),
    COMMAND(0xC4), // Booster = x8
    DATA(0x07),
    COMMAND(0xD0), // Enable Analog Circuit
    DATA(0x1D),
    COMMAND(0xB5), // N-Line = 0
    DATA(0x00),
#endif
  };

  // Send all the initialisation_sequence
  perform_instructions(controller, init_sequence, sizeof(init_sequence)/sizeof(init_sequence[0]));
}

void st7586_set_display_area(st7586_t * controller, uint16_t x_start, uint16_t x_length, uint16_t y_start, uint16_t y_length) {
  /* The datasheet says the panel counts in "columns", groups of 3 pixels.
   * It says 3, but from my understanding pixels are grouped by 2, not by 3. So
   * so let's make this 2 instead of 3. Seems to be working fine! */
  uint16_t x_end = x_start + x_length/2 - 1;
  uint16_t y_end = y_start + y_length - 1;

  assert(x_end >= x_start);
  assert(x_end <= 0x7F);
  assert(y_end >= y_start);
  assert(y_end <= 0x9F);

  const instruction_t sequence[] = {
    COMMAND(SET_COLUMN_ADDRESS),
    DATA(x_start >> 8),
    DATA(x_start),
    DATA(x_end >> 8),
    DATA(x_end),
    COMMAND(SET_ROW_ADDRESS),
    DATA(y_start >> 8),
    DATA(y_start),
    DATA(y_end >> 8),
    DATA(y_end),
  };

  perform_instructions(controller, sequence, sizeof(sequence)/sizeof(sequence[0]));
}

void st7586_enable_frame_data_upload(st7586_t * controller) {
  // Put the screen in "receive frame data"
  perform_instruction(controller, COMMAND(WRITE_DISPLAY_DATA));
  controller->data_command_pin_write(DATA_MODE);
}
