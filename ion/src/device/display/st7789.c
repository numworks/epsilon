#include <assert.h>
#include "st7789.h"

enum {
  COMMAND_MODE = 0,
  DATA_MODE = 1,
  DELAY_MODE = 2
};

enum {
  NOP = 0x00,
  SWRESET = 0x01, // Software reset
  SLPOUT = 0x11, // Sleep out
  DISPOFF = 0x28,
  DISPON = 0x29,
  CASET = 0x2A,
  RASET = 0x2B,
  RAMRW = 0x2C,
  MADCTL = 0x36, // Memory data access control
  COLMOD = 0x3A, // Interface pixel format
  PORCTRL = 0xB2, // Porch setting
  GCTRL = 0xB7, // Gate control
  VCOMS = 0xBB, // VCOM setting
  LCMCTRL = 0xC0, // LCM control
  VDVVRHEN = 0xC2, // VDV and VRH command enable
  VRHS = 0xC3, // VRH set
  VDVS = 0xC4,
  VCMOFSET = 0xC5,
  FRCTRL2 = 0xC6,
  PWCTRL1 = 0xD0,
  PVGAMCTRL = 0xE0,
  NVGAMCTRL = 0xE1,
};

typedef struct {
  char mode;
  char payload;
} instruction_t;

#define COMMAND(c) (instruction_t){.mode = (char)COMMAND_MODE, .payload = (char)c}
#define DATA(d) (instruction_t){.mode = (char)DATA_MODE, .payload = (char)d}
#define DELAY(m) (instruction_t){.mode = (char)DELAY_MODE, .payload = (char)m}

static inline void delay_ms(long ms) {
  for (long i=0; i<1040*ms; i++) {
      __asm volatile("nop");
  }
}

#if ST7789_USE_9BIT_SPI
static void push_data_on_spi_queue(st7789_t * c, uint8_t data, uint8_t numberOfBits) {
  c->spi_queue = (c->spi_queue << numberOfBits) | data;
  c->spi_queue_usage_in_bits += numberOfBits;
  assert(c->spi_queue_usage_in_bits < 8*sizeof(c->spi_queue));
}

static void flush_spi_buffer(st7789_t * c) {
  while (c->spi_queue_usage_in_bits >= 8) {
    char byte = (c->spi_queue >> (c->spi_queue_usage_in_bits - 8));
    c->spi_write(&byte, 1);
    c->spi_queue = c->spi_queue & ((1>>(c->spi_queue_usage_in_bits-8))-1); // Clear the 8 first bytes
    c->spi_queue_usage_in_bits = c->spi_queue_usage_in_bits-8;
  }
  // At this point, there remain c->spi_queue_usage_in_bits in the queue
}
#endif

static void perform_instruction(st7789_t * c, instruction_t instruction) {
  if (instruction.mode == DELAY_MODE) {
    // FIXME: Should sleep instruction->payload miliseconds
    delay_ms(instruction.payload);
  } else {
    c->data_command_pin_write(instruction.mode);
    //c->chip_select_pin_write(0);
#if ST7789_USE_9BIT_SPI
    push_data_on_spi_queue(c, (instruction.mode == DATA_MODE), 1);
    push_data_on_spi_queue(c, instruction.payload, 8);
    flush_spi_buffer(c);
#else
    c->spi_write(&instruction.payload, 1);
#endif
    //c->chip_select_pin_write(1);
  }
}

static void perform_instructions(st7789_t * c, const instruction_t * instructions, size_t length) {
  for (size_t i=0; i<length; i++) {
    perform_instruction(c, instructions[i]);
  }
}

/*
static void enable_cs(st7789_t * controller) {
  // Falling edge on CSX
  controller->chip_select_pin_write(1);
  delay_ms(120);
  controller->chip_select_pin_write(0);
  delay_ms(120);
}
*/

void st7789_initialize(st7789_t * controller) {
#if ST7789_USE_9BIT_SPI
  controller->spi_queue = 0;
  controller->spi_queue_usage_in_bits = 0;
#endif

  /*
  enable_cs(controller);

  const instruction_t debug_sequence[] = {
    COMMAND(SWRESET),
    DELAY(120),
    COMMAND(SLPOUT),
    DELAY(120),
    COMMAND(DISPON)
  };

  perform_instructions(controller, debug_sequence, sizeof(debug_sequence)/sizeof(debug_sequence[0]));

  while (1) {
  }

  return;
  */

  // Falling edge on CSX
  //enable_cs(controller);

  const instruction_t init_sequence[] = {
#if 0
    COMMAND(SLPOUT),
    COMMAND(NOP),
    DELAY(120),
    COMMAND(DISPON),
    COMMAND(NOP)
#else
    COMMAND(0x11),
    COMMAND(NOP),
    DELAY(120), //Delay 120ms
//----------------------------------Display Setting-----------------------------------------------//
COMMAND(0x36),
DATA(0x00),
COMMAND(0x3a),
DATA(0x05),
//--------------------------------ST7789S Frame rate setting----------------------------------//
COMMAND(0xb2),
DATA(0x0c),
DATA(0x0c),
DATA(0x00),
DATA(0x33),
DATA(0x33),
COMMAND(0xb7),
DATA(0x35), //VGH=13V, VGL=-10.4V
//----------------------------------------------------------------------------------------------------//
COMMAND(0xbb),
DATA(0x19),
COMMAND(0xc0),
DATA(0x2c),
COMMAND(0xc2),
DATA(0x01),
COMMAND(0xc3),
DATA(0x12),
COMMAND(0xc4),
DATA(0x20),
COMMAND(0xc6),
DATA(0x0f),
COMMAND(0xd0),
DATA(0xa4),
DATA(0xa1),
//----------------------------------------------------------------------------------------------------//
COMMAND(0xe0), //gamma setting
DATA(0xd0),
DATA(0x04),
DATA(0x0d),
DATA(0x11),
DATA(0x13),
DATA(0x2b),
DATA(0x3f),
DATA(0x54),
DATA(0x4c),
DATA(0x18),
DATA(0x0d),
DATA(0x0b),
DATA(0x1f),
DATA(0x23),
COMMAND(0xe1),
DATA(0xd0),
DATA(0x04),
DATA(0x0c),
DATA(0x11),
DATA(0x13),
DATA(0x2c),
DATA(0x3f),
DATA(0x44),
DATA(0x51),
DATA(0x2f),
DATA(0x1f),
DATA(0x1f),
DATA(0x20),
DATA(0x23),
COMMAND(0x29) //display on
#endif
  };

  // Send all the initialisation_sequence
  perform_instructions(controller, init_sequence, sizeof(init_sequence)/sizeof(init_sequence[0]));
}

void st7789_set_display_area(st7789_t * controller, uint16_t x_start, uint16_t x_length, uint16_t y_start, uint16_t y_length) {
  uint16_t x_end = x_start + x_length;
  uint16_t y_end = y_start + y_length;

  const instruction_t sequence[] = {
    COMMAND(CASET),
    DATA(x_start >> 8),
    DATA(x_start & 0xFF),
    DATA(x_end >> 8),
    DATA(x_end & 0xFF),

    COMMAND(RASET),
    DATA(y_start >> 8),
    DATA(y_start & 0xFF),
    DATA(y_end >> 8),
    DATA(y_end & 0xFF)
  };

  perform_instructions(controller, sequence, sizeof(sequence)/sizeof(sequence[0]));
}

void st7789_enable_frame_data_upload(st7789_t * controller) {
  // Put the screen in "receive frame data"
  perform_instruction(controller, COMMAND(RAMRW));
  controller->data_command_pin_write(DATA_MODE);
}
