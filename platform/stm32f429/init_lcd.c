/* LCD Initialisation code
 *
 * The LCD panel is connected via two interfaces: RGB and SPI. The SPI interface
 * is used to configure the panel, and can be used to send pixel data.
 * For higher performances, the RGB interface can be used to send pixel data.
 *
 * Here is the connection this file assumes:
 *
 *  LCD_SPI | STM32 | Role
 * ---------+-------+-----
 *    RESET | NRST  |
 *      CSX | PC2   | Chip enable input
 *      DCX | PD13  | Selects "command" or data mode
 *      SCL | PF7   | SPI clock
 *  SDI/SDO | PF9   | SPI data
 *
 * The entry point is init_lcd();
 *
 * Some info regarding the built-in LCD panel of the STM32F429I Discovery:
 *  -> The pin EXTC of the ili9341 is not connected to Vdd. It reads as "0", and
 *  therefore extended registers are not available. Those are 0xB0-0xCF and 0xE0
 *  - 0xFF. Apparently this means we cannot read the display ID (RDDIDIF).
 *  That's wat ST says in stm32f429i_discovery_lcd.c.
 */

/*#include "registers/rcc.h"
#include "registers/gpio.h"
#include "registers/spi.h"
#include "registers/ltdc.h"*/
#include "registers.h"
#include <platform/ili9341/ili9341.h>

static void init_spi_interface();
static void init_rgb_interface();
static void init_panel();

void init_lcd() {
  /* This routine is responsible for initializing the LCD panel.
   * Its interface with the outer world is the framebuffer: after execution of
   * this routine, everyone can expect to write to the LCD by writing to the
   * framebuffer. */

  init_spi_interface();

  init_rgb_interface();

  init_panel();
}

// SPI interface

static void init_spi_gpios();
static void init_spi_port();

static void init_spi_interface() {
  init_spi_gpios();
  init_spi_port();
}

static void init_spi_gpios() {
  // The LCD panel is connected on GPIO pins. Let's configure them

  // We are using groups C, D, and F. Let's enable their clocks
  RCC_AHB1ENR |= (GPIOCEN | GPIODEN | GPIOFEN);

  // PC2 and PD13 are controlled directly
  REGISTER_SET_VALUE(GPIO_MODER(GPIOC), MODER(2), GPIO_MODE_OUTPUT);
  REGISTER_SET_VALUE(GPIO_MODER(GPIOD), MODER(13), GPIO_MODE_OUTPUT);

  // PF7 and PF9 are used for an alternate function (in that case, SPI)
  REGISTER_SET_VALUE(GPIO_MODER(GPIOF), MODER(7), GPIO_MODE_ALTERNATE_FUNCTION);
  REGISTER_SET_VALUE(GPIO_MODER(GPIOF), MODER(9), GPIO_MODE_ALTERNATE_FUNCTION);

  // More precisely, PF7 and PF9 are doing SPI-SCL and SPI-SDO/SDO.
  // This corresponds to Alternate Function 5 using SPI port 5
  // (See STM32F429 p78)
  REGISTER_SET_VALUE(GPIO_AFR(GPIOF, 7), AFR(7), 5);
  REGISTER_SET_VALUE(GPIO_AFR(GPIOF, 9), AFR(9), 5);
}

static void init_spi_port() {
  // Enable the SPI5 clock (SPI5 lives on the APB2 bus)
  RCC_APB2ENR |= SPI5EN;

  // Configure the SPI port
  SPI_CR1(SPI5) = (SPI_BIDIMODE | SPI_BIDIOE | SPI_MSTR | SPI_DFF_8_BITS | SPI_BR(SPI_BR_DIV_2) | SPI_SSM | SPI_SSI | SPI_SPE);
}

// RGB interface

static void init_rgb_gpios();
static void init_rgb_clocks();
static void init_rgb_timings();

static void init_rgb_interface() {
  init_rgb_gpios();
  init_rgb_clocks();
  init_rgb_timings();
}

struct gpio_pin {
  char group;
  char number;
};

#define RGB_PIN_COUNT 77

struct gpio_pin rgb_pins[RGB_PIN_COUNT] = {
  {GPIOA, 3}, {GPIOA, 4}, {GPIOA, 6}, {GPIOA, 8}, {GPIOA, 11}, {GPIOA, 12},
  {GPIOB, 8}, {GPIOB, 9}, {GPIOB, 10}, {GPIOB, 11},
  {GPIOC, 6}, {GPIOC, 7}, {GPIOC, 10},
  {GPIOD, 3}, {GPIOD, 6}, {GPIOD, 10},
  {GPIOE, 4}, {GPIOE, 5}, {GPIOE, 6}, {GPIOE, 11}, {GPIOE, 12}, {GPIOE, 13},
  {GPIOE, 14}, {GPIOE, 15}, {GPIOF, 10},
  {GPIOG, 6}, {GPIOG, 7}, {GPIOG, 10}, {GPIOG, 11}, {GPIOG, 12},
  {GPIOH, 2}, {GPIOH, 3}, {GPIOH, 8}, {GPIOH, 9}, {GPIOH, 10}, {GPIOH, 11},
  {GPIOH, 12}, {GPIOH, 13}, {GPIOH, 14}, {GPIOH, 15},
  {GPIOI, 0}, {GPIOI, 1}, {GPIOI, 2}, {GPIOI, 4}, {GPIOI, 5}, {GPIOI, 6},
  {GPIOI, 7}, {GPIOI, 9}, {GPIOI, 10}, {GPIOI, 12}, {GPIOI, 13}, {GPIOI, 14},
  {GPIOI, 15},
  {GPIOJ, 0}, {GPIOJ, 1}, {GPIOJ, 2}, {GPIOJ, 3}, {GPIOJ, 4}, {GPIOJ, 5},
  {GPIOJ, 6}, {GPIOJ, 7}, {GPIOJ, 8}, {GPIOJ, 9}, {GPIOJ, 10}, {GPIOJ, 11},
  {GPIOJ, 12}, {GPIOJ, 13}, {GPIOJ, 14}, {GPIOJ, 15}, {GPIOK, 0}, {GPIOK, 1},
  {GPIOK, 2}, {GPIOK, 3}, {GPIOK, 4}, {GPIOK, 5}, {GPIOK, 6}, {GPIOK, 7}
};

static void init_rgb_gpios() {
  // The RGB interface uses GPIO pins in all groups!
  RCC_AHB1ENR |= (
      GPIOAEN | GPIOBEN | GPIOCEN | GPIODEN |
      GPIOEEN | GPIOFEN | GPIOGEN | GPIOHEN |
      GPIOIEN | GPIOJEN | GPIOJEN
      );

  // The LTDC is always mapped to AF14
  for (int i=0; i<RGB_PIN_COUNT; i++) {
    struct gpio_pin * pin = rgb_pins+i;
    REGISTER_SET_VALUE(GPIO_AFR(pin->group, pin->number), AFR(pin->number), 14);
  }

  //FIXME: Apprently DMA should be enabled?
  RCC_AHB1ENR |= (DMA1EN | DMA2EN | DMA2DEN);
}

static void init_rgb_clocks() {
  // STEP 1 : Enable the LTDC clock in the RCC register
  //
  // TFT-LCD lives on the APB2 bus, so we'll want to play with RCC_APB2ENR
  // (RCC stands for "Reset and Clock Control)
  RCC_APB2ENR |= LTDCEN;

  // STEP 2 : Configure the required Pixel clock following the panel datasheet
  //
  // We're setting PLLSAIN = 192, PLLSAIR = 4, and PLLSAIDIVR = 0x2 meaning divide-by-8
  // So with a f(PLLSAI clock input) = 1MHz
  // we get f(VCO clock) = PLLSAIN * fPPLSAI = 192 MHz
  // and f(PLL LCD clock) = fVCO / PLLSAIR = 48 MHz
  // and eventually f(LCD_CLK) = fPLLLCD/8 = 6 MHz

  int pllsain = 192;
  int pllsair = 4;
  int pllsaidivr = 0x2; // This value means "divide by 8"
  //FIXME: A macro here

  REGISTER_SET_VALUE(RCC_PLLSAICFGR, PLLSAIR, pllsair);
  REGISTER_SET_VALUE(RCC_PLLSAICFGR, PLLSAIN, pllsain);
  REGISTER_SET_VALUE(RCC_DCKCFGR, PLLSAIDIVR, pllsaidivr);

  // Now let's enable the PLL/PLLSAI clocks
  RCC_CR |= (PLLSAION | PLLON);
}

static void init_rgb_timings() {
  // Configure the Synchronous timings: VSYNC, HSYNC, Vertical and Horizontal
  // back porch, active data area and the front porch timings following the
  // panel datasheet

  // We'll use the typical configuration from the ILI9341 datasheet since it
  // seems to match our hardware. Here are the values of interest:
  int lcd_panel_hsync = 10;
  int lcd_panel_hbp = 20;
  int lcd_panel_hadr = 240;
  int lcd_panel_hfp = 10;
  int lcd_panel_vsync = 2;
  int lcd_panel_vbp = 2;
  int lcd_panel_vadr = 320;
  int lcd_panel_vfp = 4;

   // The LCD-TFT programmable synchronous timings are:
   // NOTE: we are only allowed to touch certain bits (0-14 and 16-27)

   /*- HSYNC and VSYNC Width: Horizontal and Vertical Synchronization width configured by
       programming a value of HSYNC Width - 1 and VSYNC Width - 1 in the LTDC_SSCR register. */

  LTDC_SSCR =
    LTDC_VSH(lcd_panel_vsync-1) |
    LTDC_HSW(lcd_panel_hsync-1);

  /*– HBP and VBP: Horizontal and Vertical Synchronization back porch width configured by
      programming the accumulated value HSYNC Width + HBP - 1 and the accumulated
      value VSYNC Width + VBP - 1 in the LTDC_BPCR register. */

  LTDC_BPCR =
    LTDC_AVBP(lcd_panel_vsync+lcd_panel_vbp-1) |
    LTDC_AHBP(lcd_panel_hsync+lcd_panel_hbp-1);

  /*– Active Width and Active Height: The Active Width and Active Height are configured by
      programming the accumulated value HSYNC Width + HBP + Active Width - 1 and the accumulated
      value VSYNC Width + VBP + Active Height - 1 in the LTDC_AWCR register (only up to 1024x768 is supported). */

  LTDC_AWCR =
    LTDC_AAH(lcd_panel_vsync+lcd_panel_vbp+lcd_panel_vadr-1) |
    LTDC_AAW(lcd_panel_hsync+lcd_panel_hbp+lcd_panel_hadr-1);

  /*– Total Width: The Total width is configured by programming the accumulated
      value HSYNC Width + HBP + Active Width + HFP - 1 in the LTDC_TWCR register.
      The HFP is the Horizontal front porch period.
    – Total Height: The Total Height is configured by programming the accumulated
      value VSYNC Height + VBP + Active Height + VFP - 1 in the LTDC_TWCR register.
      The VFP is the Vertical front porch period
    */

  LTDC_TWCR =
    LTDC_TOTALH(lcd_panel_vsync+lcd_panel_vbp+lcd_panel_vadr+lcd_panel_vfp-1) |
    LTDC_TOTALW(lcd_panel_hsync+lcd_panel_hbp+lcd_panel_hadr+lcd_panel_hfp-1);

  /* STEP 4 : Configure the synchronous signals and clock polarity in the LTDC_GCR register */

  LTDC_GCR = LTDC_LTDCEN;
  // Not setting the "Active low" bits since they are 0 by default, which we want
  // Same for the pixel clock, we don't want it inverted
}

static void init_rgb_layers() {
#if 0
    /* STEP 7: Configure the Layer1/2 parameters by programming:
– The Layer window horizontal and vertical position in the LTDC_LxWHPCR and LTDC_WVPCR registers. The layer window must be in the active data area.
– The pixel input format in the LTDC_LxPFCR register
– The color frame buffer start address in the LTDC_LxCFBAR register
– The line length and pitch of the color frame buffer in the LTDC_LxCFBLR register
– The number of lines of the color frame buffer in the LTDC_LxCFBLNR register
– if needed, load the CLUT with the RGB values and its address in the LTDC_LxCLUTWR register
– If needed, configure the default color and the blending factors respectively in the LTDC_LxDCCR and LTDC_LxBFCR registers
*/

  long * LTDC_L1WHPCR = (long *)(LCD_TFT_BASE + 0x88); // Window horizontal position config
  *LTDC_L1WHPCR = set_bits(*LTDC_L1WHPCR, 11, 0, lcd_panel_hsync+lcd_panel_hbp);
  *LTDC_L1WHPCR = set_bits(*LTDC_L1WHPCR, 27, 16, lcd_panel_hsync+lcd_panel_hbp+lcd_panel_hadr);

  long * LTDC_L1WVPCR = (long *)(LCD_TFT_BASE + 0x8C); // Window vertical position config
  *LTDC_L1WVPCR = set_bits(*LTDC_L1WVPCR, 11, 0, lcd_panel_vsync+lcd_panel_vbp);
  *LTDC_L1WVPCR = set_bits(*LTDC_L1WVPCR, 27, 16, lcd_panel_vsync+lcd_panel_vbp+lcd_panel_vadr);

  long * LTDC_L1PFCR = (long *)(LCD_TFT_BASE + 0x94); // Frame buffer pixel format
  *LTDC_L1PFCR = set_bits(*LTDC_L1PFCR, 2, 0, 0x0); // 0x0 = ARGB8888

  long * LTDC_L1CFBAR = (long *)(LCD_TFT_BASE + 0xAC); // Frame buffer address
  *LTDC_L1CFBAR = 0x2000000;

  long * LTDC_L1CFBLR = (long *)(LCD_TFT_BASE + 0xB0); // Frame buffer length
  *LTDC_L1CFBLR = set_bits(*LTDC_L1CFBLR, 28, 16, 960); // Number of bytes per lines in the framebuffer. 240 * 4 (RGBA888)
  *LTDC_L1CFBLR = set_bits(*LTDC_L1CFBLR, 12, 0, 963); // The doc says "length + 3". Here goes...

  long * LTDC_L1CFBLNR = (long *)(LCD_TFT_BASE + 0xB4);
  *LTDC_L1CFBLNR = set_bits(*LTDC_L1CFBLNR, 10, 0, 320); // Number of lines

  /*
  long * LTDC_L2WHPCR = (long *)(LCD_TFT_BASE + 0x108);
  *LTDC_L2WHPCR = set_bits(*LTDC_L2WHPCR, 11, 0, lcd_panel_hsync+lcd_panel_hbp);
  *LTDC_L2WHPCR = set_bits(*LTDC_L2WHPCR, 27, 16, lcd_panel_hsync+lcd_panel_hbp+lcd_panel_hadr);
  long * LTDC_L2WWPCR = (long *)(LCD_TFT_BASE + 0x10C);
  *LTDC_L2WVPCR = set_bits(*LTDC_L2WVPCR, 11, 0, lcd_panel_vsync+lcd_panel_vbp);
  *LTDC_L2WVPCR = set_bits(*LTDC_L2WVPCR, 27, 16, lcd_panel_vsync+lcd_panel_vbp+lcd_panel_vadr);
  long * LTDC_L2PFCR = (long *)(LCD_TFT_BASE + 0x114);
  long * LTDC_L2CFBAR = (long *)(LCD_TFT_BASE + 0x12C);
  long * LTDC_L2CFBLNR = (long *)(LCD_TFT_BASE + 0x134);
  long * LTDC_L2CR = (long *)(LCD_TFT_BASE + 0x104);
  */

  // STEP 8 : Enable layer 1
  long * LTDC_L1CR = (long *)(LCD_TFT_BASE + 0x84);
  *LTDC_L1CR = set_bits(*LTDC_L1CR, 4, 4, 0x0); // bit 4 = CLUTEN: Disable color look-up table
  *LTDC_L1CR = set_bits(*LTDC_L1CR, 1, 1, 0x0); // bit 1 = COLKEN: Color keying, disabledd
  *LTDC_L1CR = set_bits(*LTDC_L1CR, 0, 0, 0x1); // bit 1 = LEN, LayerENable: Enable

  // STEP 9 : If needed, enable color keing and dithering

  // STEP 10: Reload the shadow register
  long * LTDC_SRCR = (long *)(LCD_TFT_BASE + 0x24);
  *LTDC_SRCR = set_bits(*LTDC_SRCR, 1, 1, 0x1); // Ask for reload on next VBLANK
#endif
}

// Panel

static void spi_5_write(char * data, size_t size);
static void gpio_c2_write(bool pin_state);
static void gpio_d13_write(bool pin_state);

static ili9341_t panel = {
  .chip_select_pin_write = gpio_c2_write,
  .data_command_pin_write = gpio_d13_write,
  .spi_write = spi_5_write
};

static void init_panel() {
  ili9341_initialize(&panel);
}

static void spi_5_write(char * data, size_t size) {
  while (SPI_SR(SPI5) & SPI_BSY) {
  }
  for (size_t i=0; i<size; i++) {
    SPI_DR(SPI5) = data[i];
    while (!(SPI_SR(SPI5) & SPI_TXE)) {
    }
  }
  while (SPI_SR(SPI5) & SPI_BSY) {
  }
}

void gpio_c2_write(bool pin_state) {
  REGISTER_SET_VALUE(GPIO_ODR(GPIOC), ODR(2), pin_state);
}

void gpio_d13_write(bool pin_state) {
  REGISTER_SET_VALUE(GPIO_ODR(GPIOD), ODR(13), pin_state);
}
