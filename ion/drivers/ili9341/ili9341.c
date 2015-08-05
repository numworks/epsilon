#include "ili9341.h"

enum {
  COMMAND_MODE = 0,
  DATA_MODE = 1,
  DELAY_MODE = 2
};

enum {
  NOP = 0x00,     // No operation
  SWRESET = 0x01, // Software reset
  RDDIDIF = 0x04, // Read display identification information
  RDDST = 0x09,   // Read display status
  RDDPM = 0x0A,   // Read display power mode
  SLPOUT = 0x11,  // Sleep out
  DISPOFF = 0x28, // Display off
  DISPON = 0x29,  // Display on
  RAMWR = 0x2C,   // Memory write
  MADCTL = 0x36,  // Memory access control
  PIXSET = 0x3A,  // Pixel format set
  IFMODE = 0xB0,  // RGB interface signal control
  FRMCTR1 = 0xB1, // Frame rate control in normal/full-color mode
  IFCTL = 0xF6    // Interface control
};

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

typedef struct {
  char mode;
  char payload;
} instruction_t;

#define COMMAND(c) (instruction_t){.mode = (char)COMMAND_MODE, .payload = (char)c}
#define DATA(d) (instruction_t){.mode = (char)DATA_MODE, .payload = (char)d}
#define DELAY(m) (instruction_t){.mode = (char)DELAY_MODE, .payload = (char)m}

static instruction_t initialisation_sequence[] = {
  COMMAND(SWRESET), DELAY(5), // Software reset, then wait 5ms
  COMMAND(DISPOFF),

  // FIXME: Ad-hoc skipping extended registers, we're not using it

  // Pixel format: 16bpp, both on RGB and SPI
  COMMAND(PIXSET), DATA(0x55),

  /* FIXME: Useless: extended
  // Framerate:
  COMMAND(FRMCTR1), DATA(0x00), DATA(0x1B),
  */

  // Gamma //TODO

  /* RGB Interface mode
   *  DE is on "high enable"
   *  DOTCLK polarity: data fetched at rising time
   *  HSYNC polarity: low-level sync clock
   *  VSYNC polarity: low-level sync clock
   *  RCM: valid data is determined by DE signal */
  COMMAND(IFMODE), DATA(IFMODE_BYPASS | IFMODE_RCM_DE),


  /* Interface control
   *  DM(1): RGB interface mode
   *  RM(1): RGB interface for writing to GRAM
   *  RIM(0): 16/18bits RGB interface (1 transfer/pixel) */
  COMMAND(IFCTL), DATA(0x01), DATA(0x00), DATA(0x06),

  /* Memory access control */
  COMMAND(MADCTL), DATA(MADCTL_MX),

  // Sleep out, requires a 100ms delay
  COMMAND(SLPOUT), DELAY(100),

  // Display on, requires a 20ms delay
  COMMAND(DISPON), DELAY(20)
};

void perform_instruction(ili9341_t * c, instruction_t * instruction) {
  if (instruction->mode == DELAY_MODE) {
    // FIXME: Should sleep instruction->payload miliseconds
    for (int i = 0; i < 800*instruction->payload; i++) {
    }
  } else {
    c->data_command_pin_write(instruction->mode);
    c->spi_write(&instruction->payload, 1);
  }
}

void ili9341_initialize(ili9341_t * c) {
  // Falling edge on CSX
  c->chip_select_pin_write(0);

  // Send all the initialisation_sequence
  size_t init_sequence_length = sizeof(initialisation_sequence)/sizeof(initialisation_sequence[0]);
  for (int i=0; i<init_sequence_length;i++) {
    perform_instruction(c, initialisation_sequence+i);
  }

#define FILL_SCREEN_UPON_INIT 1
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

void ili9341_set_gamma(ili9341_t * c) {
}
