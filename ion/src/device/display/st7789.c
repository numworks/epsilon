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

// FIXME: This is ugly and should not live here
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
    delay_ms(instruction.payload);
  } else {
    //c->chip_select_pin_write(0);
#if ST7789_USE_9BIT_SPI
    push_data_on_spi_queue(c, (instruction.mode == DATA_MODE), 1);
    push_data_on_spi_queue(c, instruction.payload, 8);
    flush_spi_buffer(c);
#else
    c->data_command_pin_write(instruction.mode);
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

void st7789_initialize(st7789_t * c) {
#if ST7789_USE_9BIT_SPI
  c->spi_queue = 0;
  c->spi_queue_usage_in_bits = 0;
#endif

  /* The ST7789S has multiple interfaces to receive data.
   * In this driver, we're using the SPI interface. To initialize this
   * interface, the controller expects a falling edge on the CS pin. */
  c->chip_select_pin_write(1);
  if (c->reset_pin_write != NULL) {
    c->reset_pin_write(1);
    delay_ms(10);
    c->reset_pin_write(0);
    delay_ms(10);
    c->reset_pin_write(1);
    delay_ms(100);
  }
  delay_ms(10);
  c->chip_select_pin_write(0);

  /* We were provided the following init sequence by the manufacturer */
  const instruction_t init_sequence[] = {
    COMMAND(SLPOUT),
    COMMAND(NOP), // Just needed because of the delay and the 9-bit thing...
    DELAY(120), //Delay 120ms

    COMMAND(MADCTL), DATA(0x00),
    COMMAND(COLMOD), DATA(0x05),

    COMMAND(PORCTRL),
    DATA(0x0c), DATA(0x0c), DATA(0x00), DATA(0x33), DATA(0x33),

    COMMAND(GCTRL), DATA(0x35), //VGH=13V, VGL=-10.4V
    COMMAND(VCOMS), DATA(0x19),
    COMMAND(LCMCTRL), DATA(0x2c),
    COMMAND(VDVVRHEN), DATA(0x01),
    COMMAND(VRHS), DATA(0x12),
    COMMAND(VDVS), DATA(0x20),
    COMMAND(FRCTRL2), DATA(0x0f),
    COMMAND(PWCTRL1), DATA(0xa4), DATA(0xa1),

    COMMAND(PVGAMCTRL), //gamma setting
    DATA(0xd0), DATA(0x04), DATA(0x0d), DATA(0x11), DATA(0x13), DATA(0x2b),
    DATA(0x3f), DATA(0x54), DATA(0x4c), DATA(0x18), DATA(0x0d), DATA(0x0b),
    DATA(0x1f), DATA(0x23),

    COMMAND(NVGAMCTRL),
    DATA(0xd0), DATA(0x04), DATA(0x0c), DATA(0x11), DATA(0x13), DATA(0x2c),
    DATA(0x3f), DATA(0x44), DATA(0x51), DATA(0x2f), DATA(0x1f), DATA(0x1f),
    DATA(0x20), DATA(0x23),

    COMMAND(DISPON) //display on
  };

  // Send all the initialisation_sequence
  perform_instructions(c, init_sequence, sizeof(init_sequence)/sizeof(init_sequence[0]));
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

  int check_size = 5;

  for (int i=0; i<320; i++) {
    for (int j=0; j<240; j++) {
      char color = (((i/check_size)%2 == 0) ^ ((j/check_size)%2 == 0)) ? 0x00 : i;
      perform_instruction(controller, DATA(color));
      perform_instruction(controller, DATA(color));
    }
  }
  //controller->data_command_pin_write(DATA_MODE);
  //
}
