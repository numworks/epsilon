#include <ion.h>
#include "display.h"
#include "regs/regs.h"
extern "C" {
#include <assert.h>
}

/* This driver interfaces with the ST7789V LCD controller.
 * This chip keeps a whole frame in SRAM memory and feeds it to the LCD panel as
 * needed. We use the STM32's FSMC to drive the bus between the ST7789V. Once
 * configured, we only need to write in the address space of the MCU to actually
 * send some data to the LCD controller. */

// Public Ion::Display methods

namespace Ion {
namespace Display {

void pushRect(KDRect r, const KDColor * pixels) {
  Device::waitForPendingDMAUploadCompletion();
  Device::setDrawingArea(r, Device::Orientation::Landscape);
  Device::pushPixels(pixels, r.width()*r.height());
}

void pushRectUniform(KDRect r, KDColor c) {
  Device::waitForPendingDMAUploadCompletion();
  Device::setDrawingArea(r, Device::Orientation::Portrait);
  Device::pushColor(c, r.width()*r.height());
}

void pullRect(KDRect r, KDColor * pixels) {
  Device::waitForPendingDMAUploadCompletion();
  Device::setDrawingArea(r, Device::Orientation::Landscape);
  Device::pullPixels(pixels, r.width()*r.height());
}

void waitForVBlank() {
  // We want to return as soon as the TE line is transitionning from "DOWN" to "UP"
  while (Device::TearingEffectPin.group().IDR()->get(Device::TearingEffectPin.pin())) {
    // Loop while high, exit when low
    // Wait for zero
  }
  while (!Device::TearingEffectPin.group().IDR()->get(Device::TearingEffectPin.pin())) {
    // Loop while low, exit when high
  }
}

}
}

// Private Ion::Display::Device methods

namespace Ion {
namespace Display {
namespace Device {

#define SEND_COMMAND(c, ...) {*CommandAddress = Command::c; uint8_t data[] = {__VA_ARGS__}; for (unsigned int i=0;i<sizeof(data);i++) { *DataAddress = data[i];};}

void init() {
  initDMA();
  initGPIO();
  initFSMC();
  initPanel();
}

void shutdown() {
  shutdownPanel();
  shutdownFSMC();
  shutdownGPIO();
}

void initDMA() {
  // Only DMA2 can perform memory-to-memory transfers
  //assert(DMAEngine == DMA2);

  /* In memory-to-memory transfers, the "peripheral" is the source and the
   * "memory" is the destination. In other words, memory is copied from address
   * DMA_SxPAR to address DMA_SxM0AR. */

  DMAEngine.SCR(DMAStream)->setDIR(DMA::SCR::Direction::MemoryToMemory);
  DMAEngine.SM0AR(DMAStream)->set((uint32_t)DataAddress);
  DMAEngine.SCR(DMAStream)->setMSIZE(DMA::SCR::DataSize::HalfWord);
  DMAEngine.SCR(DMAStream)->setPSIZE(DMA::SCR::DataSize::HalfWord);
  //DMAEngine.SCR(DMAStream)->setMBURST(DMA::SCR::Burst::Incremental4);
  //DMAEngine.SCR(DMAStream)->setPBURST(DMA::SCR::Burst::Incremental4);
  DMAEngine.SCR(DMAStream)->setMINC(false);
}

void waitForPendingDMAUploadCompletion() {
  // Loop until DMA engine available
  while (DMAEngine.SCR(DMAStream)->getEN()) {
  }
}

static inline void startDMAUpload(const KDColor * src, bool incrementSrc, uint16_t length) {
  // Reset interruption markers
  DMAEngine.LIFCR()->set(0xF7D0F7D);

  DMAEngine.SNDTR(DMAStream)->set(length);
  DMAEngine.SPAR(DMAStream)->set((uint32_t)src);
  DMAEngine.SCR(DMAStream)->setPINC(incrementSrc);
  DMAEngine.SCR(DMAStream)->setEN(true);
}

void initGPIO() {
  // All the FSMC GPIO pins use the alternate function number 12
  for(const GPIOPin & g : FSMCPins) {
    g.group().MODER()->setMode(g.pin(), GPIO::MODER::Mode::AlternateFunction);
    g.group().AFR()->setAlternateFunction(g.pin(), GPIO::AFR::AlternateFunction::AF12);
  }

  // Turn on the power
  PowerPin.group().MODER()->setMode(PowerPin.pin(), GPIO::MODER::Mode::Output);
  PowerPin.group().ODR()->set(PowerPin.pin(), true);

  // Turn on the reset pin
  ResetPin.group().MODER()->setMode(ResetPin.pin(), GPIO::MODER::Mode::Output);
  ResetPin.group().ODR()->set(ResetPin.pin(), true);

  // Turn on the extended command pin
  ExtendedCommandPin.group().MODER()->setMode(ExtendedCommandPin.pin(), GPIO::MODER::Mode::Output);
  ExtendedCommandPin.group().ODR()->set(ExtendedCommandPin.pin(), true);

  // Turn on the Tearing Effect pin
  TearingEffectPin.group().MODER()->setMode(TearingEffectPin.pin(), GPIO::MODER::Mode::Input);
  TearingEffectPin.group().PUPDR()->setPull(TearingEffectPin.pin(), GPIO::PUPDR::Pull::None);

  msleep(120);
}


void shutdownGPIO() {
  // All the FSMC GPIO pins use the alternate function number 12
  for(const GPIOPin & g : FSMCPins) {
    g.group().MODER()->setMode(g.pin(), GPIO::MODER::Mode::Analog);
    g.group().PUPDR()->setPull(g.pin(), GPIO::PUPDR::Pull::None);
  }

  // Set to true : sleep consumption = 154 uA
  // Set to false : sleep consumption = 92 uA
  ResetPin.group().ODR()->set(ResetPin.pin(), false);

  PowerPin.group().MODER()->setMode(PowerPin.pin(), GPIO::MODER::Mode::Analog);
  PowerPin.group().PUPDR()->setPull(PowerPin.pin(), GPIO::PUPDR::Pull::None);

  ExtendedCommandPin.group().MODER()->setMode(ExtendedCommandPin.pin(), GPIO::MODER::Mode::Analog);
  ExtendedCommandPin.group().PUPDR()->setPull(ExtendedCommandPin.pin(), GPIO::PUPDR::Pull::None);

  TearingEffectPin.group().MODER()->setMode(TearingEffectPin.pin(), GPIO::MODER::Mode::Analog);
}

void initFSMC() {
  // Control register
  FSMC.BCR(FSMCMemoryBank)->setMWID(FSMC::BCR::MWID::SIXTEEN_BITS);
  FSMC.BCR(FSMCMemoryBank)->setWREN(true);
  FSMC.BCR(FSMCMemoryBank)->setMBKEN(true);
  FSMC.BCR(FSMCMemoryBank)->setEXTMOD(true);

  /* From AN2790 Application note - TFT LCD interfacing with the high-density
   * STM32F10xxx FSMC and STM32F412 reference manual:
   * (ADDSET + DATAST) × HCLK = Tcyc(Read cycle time)
   * DATAST × HCLK = Twrlr (Low pulse width for read)
   * DATAST = (((Tacc(Data access time) + Tas(Address setup time)) +
   * (tsu(Data_NE)+ tv(A_NE)))/HCLK) – ADDSET – 4
   * With:
   * tsu(Data_NE): FSMC_NEx low to data valid
   * tv(A_NE): FSMC_NEx low to FSMC_A valid
   *
   * Hence:
   * T(HCLK) = 1*10^(9)/(96MHz*10^6) = 10.42 ns (Cf STM32F412 datasheet)
   * tsu(Data_NE) = T(HCLK) - 1 = 9.42 ns (Cf STM32F412 datasheet)
   * tv(A_NE) = 1.5 ns  (Cf STM32F412 datasheet)
   * Tcyc(read) = 450 ns (Cf ST7789 datasheet)
   * Twrlr = 45 ns (Cf ST7789 datasheet)
   * Tacc(read) = 340 ns (Cf ST7789 datasheet)
   * Tas = 0 ns (Cf ST7789 datasheet)
   *
   * ADDSET(read) = 29
   * DATAST(read) = 5*/

  // Reading timing register
  FSMC.BTR(FSMCMemoryBank)->setADDSET(29);
  FSMC.BTR(FSMCMemoryBank)->setDATAST(5);
  FSMC.BTR(FSMCMemoryBank)->setBUSTURN(0);
  FSMC.BTR(FSMCMemoryBank)->setACCMOD(1);

  /* From AN2790 Application note - TFT LCD interfacing with the high-density
   * STM32F10xxx FSMC and STM32F412 reference manual:
   * (ADDSET+ (DATAST + 1)) × HCLK = Tcyc(Write or read cycle time)
   * DATAST × HCLK = Twrlw (Low pulse width for write)
   * DATAST = (((Tacc(Data access time) + Tas(Address setup time)) +
   * (tsu(Data_NE)+ tv(A_NE)))/HCLK) – ADDSET – 4
   * With:
   * tsu(Data_NE): FSMC_NEx low to data valid
   * tv(A_NE): FSMC_NEx low to FSMC_A valid
   *
   * Hence:
   * T(HCLK) = 1*10^(9)/(96MHz*10^6) = 10.42 ns (Cf STM32F412 datasheet)
   * tsu(Data_NE) = T(HCLK) - 1 = 9.42 ns (Cf STM32F412 datasheet)
   * tv(A_NE) = 1.5 ns  (Cf STM32F412 datasheet)
   * Tcyc(write) = 66 ns (Cf ST7789 datasheet)
   * Twrlw = 15 ns (Cf ST7789 datasheet)
   * Tacc(write) = 80 ns ?
   * Tas = 0 ns (Cf ST7789 datasheet)
   *
   * ADDSET(write) = 4
   * DATAST(write) = 2 */

  // Writing timing register
  FSMC.BWTR(FSMCMemoryBank)->setADDSET(4);
  FSMC.BWTR(FSMCMemoryBank)->setDATAST(2);
  FSMC.BWTR(FSMCMemoryBank)->setBUSTURN(0);
  FSMC.BTR(FSMCMemoryBank)->setACCMOD(1);
}

void shutdownFSMC() {
}

void initPanel() {
  *CommandAddress = Command::Reset;
  msleep(5);

  *CommandAddress = Command::SleepOut;
  msleep(5);

  SEND_COMMAND(PixelFormatSet, 0x05);
  //SEND_COMMAND(MemoryAccessControl, 0xA0);
  SEND_COMMAND(TearingEffectLineOn, 0x00);
  SEND_COMMAND(FrameRateControl, 0x1E); // 40 Hz frame rate

  *CommandAddress = Command::DisplayOn;
}

void shutdownPanel() {
  *CommandAddress = Command::DisplayOff;
  *CommandAddress = Command::SleepIn;
  msleep(5);
}

void setDrawingArea(KDRect r, Orientation o) {
  uint16_t x_start, x_end, y_start, y_end;

  if (o == Orientation::Landscape) {
    SEND_COMMAND(MemoryAccessControl, 0xA0);
    x_start = r.x();
    x_end = r.x() + r.width() - 1;
    y_start = r.y();
    y_end = r.y() + r.height() - 1;
  } else {
    SEND_COMMAND(MemoryAccessControl, 0x00);
    x_start = r.y();
    x_end = r.y() + r.height() - 1;
    y_start = Ion::Display::Width - (r.x() + r.width());
    y_end = Ion::Display::Width - r.x() - 1;
  }

  *CommandAddress  = Command::ColumnAddressSet;
  *DataAddress = (x_start >> 8);
  *DataAddress = (x_start & 0xFF);
  *DataAddress = (x_end >> 8);
  *DataAddress = (x_end & 0xFF);

  *CommandAddress  = Command::PageAddressSet;
  *DataAddress = (y_start >> 8);
  *DataAddress = (y_start & 0xFF);
  *DataAddress = (y_end >> 8);
  *DataAddress = (y_end & 0xFF);
}

void pushPixels(const KDColor * pixels, size_t numberOfPixels) {
  *CommandAddress  = Command::MemoryWrite;
  /* Theoretically, we should not be able to use DMA here. Indeed, we have no
   * guarantee that the content at "pixels" will remain valid once we exit this
   * function call. In practice, we might be able to use DMA here because most
   * of the time we push pixels from static locations. */
#define USE_DMA_FOR_PUSH_PIXELS 0
#if USE_DMA_FOR_PUSH_PIXELS
  startDMAUpload(pixels, true, numberOfPixels);
#else
  while (numberOfPixels > 8) {
    *DataAddress = *pixels++;
    *DataAddress = *pixels++;
    *DataAddress = *pixels++;
    *DataAddress = *pixels++;
    *DataAddress = *pixels++;
    *DataAddress = *pixels++;
    *DataAddress = *pixels++;
    *DataAddress = *pixels++;
    numberOfPixels -= 8;
  }
  while (numberOfPixels--) {
    *DataAddress = *pixels++;
  }
#endif
}

void pushColor(KDColor color, size_t numberOfPixels) {
  *CommandAddress  = Command::MemoryWrite;
  /* The "color" variable lives on the stack. We cannot take its address because
   * it will stop being valid as soon as we return. An easy workaround is to
   * duplicate the content in a static variable, whose value is guaranteed to be
   * kept until the next pushColor call. */
  static KDColor staticColor;
  staticColor = color;
  //startDMAUpload(&staticColor, false, (numberOfPixels > 64000 ? 64000 : numberOfPixels));
  while (numberOfPixels--) {
    *DataAddress = color;
  }
}

void pullPixels(KDColor * pixels, size_t numberOfPixels) {
  if (numberOfPixels == 0) {
    return;
  }
  SEND_COMMAND(PixelFormatSet, 0x06);
  *CommandAddress  = Command::MemoryRead;
  *DataAddress; // First read is dummy data, per datasheet
  while (true) {
    if (numberOfPixels == 0) {
      break;
    }
    uint16_t one = *DataAddress;
    uint16_t two = *DataAddress;
    uint16_t firstPixel  = (one & 0xF800) | (one & 0xFC) << 3 | (two & 0xF800) >> 11;
    *pixels++ = KDColor::RGB16(firstPixel);
    numberOfPixels--;

    if (numberOfPixels == 0) {
      break;
    }
    uint16_t three = *DataAddress;
    uint16_t secondPixel = (two & 0xF8) << 8 | (three & 0xFC00) >> 5 | (three & 0xF8) >> 3;
    *pixels++ = KDColor::RGB16(secondPixel);
    numberOfPixels--;
  }
  SEND_COMMAND(PixelFormatSet, 0x05);
}

}
}
}
