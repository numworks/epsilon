#include "../registers/registers.h"
#include "../framebuffer.h"

// DMA 1, channel 0, stream 4 = SPI2_TX
#define LCD_DMA_CHANNEL 0
#define LCD_DMA_STREAM 4

void display_dma_init() {
// 0 - ENable DMA clock!
  RCC_AHB1ENR |= DMA1EN;


  // 1 - Reset DMA
  // Stuff like this, and a wait loop...
  //DMA_SCR(DMA1,4) |= DMA_EN;
  //DMA_SCR(DMA1,4) ~= DMA_EN;

  // 2 - Set the peripheral address
  DMA_SPAR(DMA1,LCD_DMA_STREAM) = (uint32_t)&SPI_DR(SPI2);

  // 3 - Set the memory address
  DMA_SMA0R(DMA1,LCD_DMA_STREAM) = (uint32_t)FRAMEBUFFER_ADDRESS;

  // 4 - Number of data items
  REGISTER_SET_VALUE(DMA_SNDTR(DMA1,LCD_DMA_STREAM), DMA_SNDTR, FRAMEBUFFER_LENGTH);

  // 5 - Select the DMA channel
  REGISTER_SET_VALUE(DMA_SCR(DMA1,LCD_DMA_STREAM), DMA_CHSEL, LCD_DMA_CHANNEL);

  // 6 - Set peripheral flow control
  // Does not apply, only for SD/MMC

  // 7 - Stream priority
  // We don't care yet

  // 8 - Configure FIFO usage
  // I think we don't care

  // 9 - Data transfer direction, peripheral/memory increment, single burst or transaction, peripheral and memory data width
    REGISTER_SET_VALUE(DMA_SCR(DMA1,LCD_DMA_STREAM), DMA_DIR, DMA_DIR_MEMORY_TO_PERIPHERAL);

    DMA_SCR(DMA1,LCD_DMA_STREAM) |= DMA_CIRC;

    // Memory address is incremented
    DMA_SCR(DMA1,LCD_DMA_STREAM) |= DMA_MINC;

    // Peripheral expects 8 bits values, default
    //REGISTER_SET_VALUE(DMA_SCR(DMA1,LCD_DMA_STREAM), DMA_DIR, DMA_DIR_MEMORY_TO_PERIPHERAL);

    // 10 - Enable DMA transfer!
    DMA_SCR(DMA1,LCD_DMA_STREAM) |= DMA_EN;

    // 11 - Bonux: enable DMA requests on SPI
    SPI_CR2(SPI2) |= SPI_TXDMAEN;

    /*while (DMA_SCR(DMA1,LCD_DMA_STREAM) & DMA_EN) {
    }
    DMA_SCR(DMA1,LCD_DMA_STREAM) |= DMA_EN;
    */
}
