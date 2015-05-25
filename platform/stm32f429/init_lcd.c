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
 *
 *  It doesn't seem right though, because some extended commands do work...
 */

#include <assert.h>
#include "registers.h"
#include <platform/platform.h>
#include <platform/ili9341/ili9341.h>

extern char _framebuffer_start;
extern char _framebuffer_end;

static void init_spi_interface();
static void init_rgb_interface();
static void init_panel();
static void check_framebuffer();

void init_lcd() {
  /* This routine is responsible for initializing the LCD panel.
   * Its interface with the outer world is the framebuffer: after execution of
   * this routine, everyone can expect to write to the LCD by writing to the
   * framebuffer. */

  Platform.framebuffer_width = 240;
  Platform.framebuffer_height = 320;
  Platform.framebuffer_bits_per_pixel = 8;
  Platform.framebuffer_address = &_framebuffer_start;

  init_spi_interface();

  init_rgb_interface();

  init_panel();

#ifndef NDEBUG
  check_framebuffer();
#endif
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
//static void init_rgb_layers();

static void init_rgb_interface() {
  init_rgb_gpios();
  init_rgb_clocks();
  init_rgb_timings();
//  init_rgb_layers();

}

// FIXME: Apparently many of those aren't needed. E.g the GPIOE ones
gpio_pin_t rgb_pins[] = {
  {GPIOA, 3}, {GPIOA, 4}, {GPIOA, 6}, {GPIOA, 8}, {GPIOA, 11}, {GPIOA, 12},
  {GPIOB, 8}, {GPIOB, 9}, {GPIOB, 10}, {GPIOB, 11},
  {GPIOC, 6}, {GPIOC, 7}, {GPIOC, 10},
  {GPIOD, 3}, {GPIOD, 6}, {GPIOD, 10},
  //{GPIOE, 4}, {GPIOE, 5}, {GPIOE, 6}, {GPIOE, 11}, {GPIOE, 12}, {GPIOE, 13},
  //{GPIOE, 14}, {GPIOE, 15},
  {GPIOF, 10},
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
      GPIOIEN | GPIOJEN | GPIOKEN
      );

  // The LTDC is always mapped to AF14
  size_t rgb_pin_count = sizeof(rgb_pins)/sizeof(rgb_pins[0]);
  for (int i=0; i<rgb_pin_count; i++) {
    gpio_pin_t * pin = rgb_pins+i;
    REGISTER_SET_VALUE(GPIO_AFR(pin->group, pin->number), AFR(pin->number), 14);
    REGISTER_SET_VALUE(GPIO_MODER(pin->group), MODER(pin->number), GPIO_MODE_ALTERNATE_FUNCTION);
  }

  //FIXME: Apprently DMA should be enabled?
  //RCC_AHB1ENR |= (DMA1EN | DMA2EN | DMA2DEN);
}

static void init_rgb_clocks() {
  // STEP 1 : Enable the LTDC clock in the RCC register
  //
  // TFT-LCD lives on the APB2 bus, so we'll want to play with RCC_APB2ENR
  // (RCC stands for "Reset and Clock Control)
  RCC_APB2ENR |= LTDCEN;

  // STEP 2 : Configure the required Pixel clock following the panel datasheet
  //
  // The pixel clock derives from the PLLSAI clock through various multipliers/dividers.
  // Here is the exact sequence :
  // PXL = PLL_LCD/RCC_DCKCFGR.PLLSAIDIVR;
  // PLL_LCD = VCO/RCC_PLLSAICFGR.PLLSAIR;
  // VCO = PLLSAI * (RCC_PLLSAICFG.PLLSAIN / RCC_PLLCFGR.PLLM);
  // PLLSAI = HSE or HSI
  //
  // The multipliers have the following constraints :
  // 2 <= PLLM <= 63
  // 49 <= PLLSAIN <= 432
  // 2 <= PLLSAIR <= 7
  // 2 ≤ PLLSAIDIVR ≤ 16, (set as a power of two, use macro)
  //
  // By default, PLLSAI = HSI = 16MHz and RCC_PLLCFGR.PLLM = 16. This gives, in MHZ:
  // PXL = SAIN/(SAIR*SAIDIVR);
  //
  // // FIXME: Maybe make this calculation dynamic?
  // Per the panel doc, we want a clock of 6 MHz.
  // 6 = 192/(4*8), hence:

  REGISTER_SET_VALUE(RCC_PLLSAICFGR, PLLSAIN, 192);
  REGISTER_SET_VALUE(RCC_PLLSAICFGR, PLLSAIR, 4);
  REGISTER_SET_VALUE(RCC_DCKCFGR, PLLSAIDIVR, RCC_PLLSAIDIVR_DIV8);

  // Now let's enable the PLL/PLLSAI clocks
  RCC_CR |= (PLLSAION | PLLON);

  // And wait 'till they are ready!
  while ((RCC_CR & PLLSAIRDY) == 0 || (RCC_CR & PLLRDY) == 0) {
  }
}

static void init_rgb_timings() {
  // Configure the Synchronous timings: VSYNC, HSYNC, Vertical and Horizontal
  // back porch, active data area and the front porch timings following the
  // panel datasheet

  // We'll use the typical configuration from the ILI9341 datasheet since it
  // seems to match our hardware. Here are the values of interest:
  int lcd_panel_hsync = 10;
  int lcd_panel_hbp = 20;
  int lcd_panel_hadr = Platform.framebuffer_width;
  int lcd_panel_hfp = 10;
  int lcd_panel_vsync = 2;
  int lcd_panel_vbp = 2;
  int lcd_panel_vadr = Platform.framebuffer_height;
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

  LTDC_GCR |= LTDC_PCPOL;

  // FIXME: Later:LTDC_GCR = LTDC_LTDCEN;
  // Not setting the "Active low" bits since they are 0 by default, which we want
  // Same for the pixel clock, we don't want it inverted

  LTDC_BCCR = 0x00FF00FF;


  /*
}

static void init_rgb_layers() {
*/



    /* STEP 7: Configure the Layer1/2 parameters by programming:
– The Layer window horizontal and vertical position in the LTDC_LxWHPCR and LTDC_WVPCR registers. The layer window must be in the active data area.
– The pixel input format in the LTDC_LxPFCR register
– The color frame buffer start address in the LTDC_LxCFBAR register
– The line length and pitch of the color frame buffer in the LTDC_LxCFBLR register
– The number of lines of the color frame buffer in the LTDC_LxCFBLNR register
– if needed, load the CLUT with the RGB values and its address in the LTDC_LxCLUTWR register
– If needed, configure the default color and the blending factors respectively in the LTDC_LxDCCR and LTDC_LxBFCR registers
*/

  LTDC_LWHPCR(LTDC_LAYER1) =
    LTDC_WHSTPOS(lcd_panel_hsync+lcd_panel_hbp) |
    LTDC_WHSPPOS(lcd_panel_hsync+lcd_panel_hbp+lcd_panel_hadr-1); //FIXME: Why -1?

  LTDC_LWVPCR(LTDC_LAYER1) =
    LTDC_WVSTPOS(lcd_panel_vsync+lcd_panel_vbp) |
    LTDC_WVSPPOS(lcd_panel_vsync+lcd_panel_vbp+lcd_panel_vadr-1);

  LTDC_LPFCR(LTDC_LAYER1) = LTDC_PF_L8;

  LTDC_LCFBAR(LTDC_LAYER1) = Platform.framebuffer_address;

  LTDC_LCFBLR(LTDC_LAYER1) =
    LTDC_CFBLL(Platform.framebuffer_width + 3) | // Number of bytes per lines in the framebuffer. 240 * 4 (RGBA888). +3, per doc;
    LTDC_CFBP(Platform.framebuffer_width); // Width of a line in bytes

  LTDC_LCFBLNR(LTDC_LAYER1) = LTDC_CFBLNR(Platform.framebuffer_height); // Number of lines

  // STEP 8 : Enable layer 1
  // Don't enable color keying nor color look-up table
  LTDC_LCR(LTDC_LAYER1) = LTDC_LEN;

  // STEP 9 : If needed, enable color keing and dithering

  // STEP 10: Reload the shadow register
  // Ask for immediate reload
  LTDC_SRCR = LTDC_IMR;

  // Now let's actually enable the LTDC
  LTDC_GCR |= LTDC_LTDCEN;
}

// Panel

static void spi_5_write(char * data, size_t size);
static void gpio_c2_write(bool pin_state);
static void gpio_d13_write(bool pin_state);

static void init_panel() {
  ili9341_t * panel = &(Platform.display);
  panel->chip_select_pin_write = gpio_c2_write;
  panel->data_command_pin_write = gpio_d13_write;
  panel->spi_write = spi_5_write;
  ili9341_initialize(panel);
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

// Framebuffer checks

static void check_framebuffer() {
  assert(&_framebuffer_start == Platform.framebuffer_address);
  char * fb_end = &_framebuffer_start + (Platform.framebuffer_width*Platform.framebuffer_height*Platform.framebuffer_bits_per_pixel/8);
  assert(&_framebuffer_end == fb_end);
}
