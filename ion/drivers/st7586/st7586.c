#include "st7586.h"

enum {
  COMMAND_MODE = 0,
  DATA_MODE = 1,
  DELAY_MODE = 2
};

enum {
  NOP = 0x00,     // No operation
  RESET = 0x01, // Software reset
  SLEEP_IN = 0x10,
  SLEEP_OUT = 0x11,
  DISPLAY_OFF = 0x28,
  DISPLAY_ON = 0x29,
  /*
  RAMWR = 0x2C,   // Memory write
  MADCTL = 0x36,  // Memory access control
  PIXSET = 0x3A,  // Pixel format set
  IFMODE = 0xB0,  // RGB interface signal control
  FRMCTR1 = 0xB1, // Frame rate control in normal/full-color mode
  IFCTL = 0xF6    // Interface control
  */
};
/*
#define IFMODE_EPL (1<<0)
#define IFMODE_DPL (1<<1)
#define IFMODE_HSPL (1<<2)
#define IFMODE_VSPL (1<<3)
#define IFMODE_RCM_DE (2<<5)
// DE mode: Valid data is determined by the DE signal
#define IFMODE_RCM_SYNC (3<<5)
// Sync mode: In sync mode, DE signal is ignored
#define IFMODE_BYPASS (1<<7)


#define MADCTL_MH (1<<2)
#define MADCTL_BGR (1<<3)
#define MADCTL_ML (1<<4)
#define MADCTL_MV (1<<5)
#define MADCTL_MX (1<<6)
#define MADCTL_MY (1<<7)
*/

typedef struct {
  char mode;
  char payload;
} instruction_t;

#define COMMAND(c) (instruction_t){.mode = (char)COMMAND_MODE, .payload = (char)c}
#define DATA(d) (instruction_t){.mode = (char)DATA_MODE, .payload = (char)d}
#define DELAY(m) (instruction_t){.mode = (char)DELAY_MODE, .payload = (char)m}

static instruction_t initialisation_sequence[] = {
  COMMAND(RESET), DELAY(50), // Software reset, then wait 5ms
  COMMAND(DISPLAY_OFF),

  // Sleep out, requires a 100ms delay
  COMMAND(SLEEP_OUT), DELAY(100),

  // Display on, requires a 20ms delay
  COMMAND(DISPLAY_ON), DELAY(20)
};

static void perform_instruction(st7586_t * c, instruction_t * instruction) {
  if (instruction->mode == DELAY_MODE) {
    // FIXME: Should sleep instruction->payload miliseconds
    for (int i = 0; i < 800*instruction->payload; i++) {
    }
  } else {
    c->data_command_pin_write(instruction->mode);
    c->spi_write(&instruction->payload, 1);
  }
}

void st7586_initialize(st7586_t * c) {
  // Falling edge on CSX
  c->chip_select_pin_write(0);

  // Send all the initialisation_sequence
  size_t init_sequence_length = sizeof(initialisation_sequence)/sizeof(initialisation_sequence[0]);
  for (int i=0; i<init_sequence_length;i++) {
    perform_instruction(c, initialisation_sequence+i);
  }

#define FILL_SCREEN_UPON_INIT 0
#if FILL_SCREEN_UPON_INIT
  // This would draw stuff on the screen

  perform_instruction(c, &COMMAND(RAMWR));

  c->data_command_pin_write(DATA_MODE);

  char buffer[256];
  for (int i = 0;i<256;i++) {
    buffer[i] = i;
  }
  for (int i = 0; i<(320*240*2)/256; i++) {
    c->spi_write(buffer, 256);
  }
#endif
}
