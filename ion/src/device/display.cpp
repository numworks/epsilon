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

#define USE_DMA_FOR_PUSH_PIXELS 0
#define USE_DMA_FOR_PUSH_COLOR 0

#define USE_DMA (USE_DMA_FOR_PUSH_PIXELS|USE_DMA_FOR_PUSH_COLOR)

// Public Ion::Display methods

namespace Ion {
namespace Display {

void pushRect(KDRect r, const KDColor * pixels) {
#if USE_DMA
  Device::waitForPendingDMAUploadCompletion();
#endif
  Device::setDrawingArea(r, Device::Orientation::Landscape);
  Device::pushPixels(pixels, r.width()*r.height());
}

void pushRectUniform(KDRect r, KDColor c) {
#if USE_DMA
  Device::waitForPendingDMAUploadCompletion();
#endif
  Device::setDrawingArea(r, Device::Orientation::Portrait);
  Device::pushColor(c, r.width()*r.height());
}

void pullRect(KDRect r, KDColor * pixels) {
#if USE_DMA
  Device::waitForPendingDMAUploadCompletion();
#endif
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

static inline void send_data(uint16_t d) {
  *DataAddress = d;
}

static inline uint16_t receive_data() {
  return *DataAddress;
}

template<typename... Args>
static inline void send_data(uint16_t d, Args... other) {
  send_data(d);
  send_data(other...);
}

static inline void send_command(Command c) {
  *CommandAddress = c;
}

template<typename... Args>
static inline void send_command(Command c, Args... d) {
  send_command(c);
  send_data(d...);
}

void init() {
#if USE_DMA
  initDMA();
#endif
  initGPIO();
  initFSMC();
  initPanel();
}

void shutdown() {
  shutdownPanel();
  shutdownFSMC();
  shutdownGPIO();
}

#if USE_DMA
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
  DMAEngine.SCR(DMAStream)->setMBURST(DMA::SCR::Burst::Incremental4);
  DMAEngine.SCR(DMAStream)->setPBURST(DMA::SCR::Burst::Incremental4);
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
#endif

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

  ResetPin.group().MODER()->setMode(ResetPin.pin(), GPIO::MODER::Mode::Analog);
  ResetPin.group().PUPDR()->setPull(ResetPin.pin(), GPIO::PUPDR::Pull::None);

  PowerPin.group().MODER()->setMode(PowerPin.pin(), GPIO::MODER::Mode::Analog);
  PowerPin.group().PUPDR()->setPull(PowerPin.pin(), GPIO::PUPDR::Pull::None);

  ExtendedCommandPin.group().MODER()->setMode(ExtendedCommandPin.pin(), GPIO::MODER::Mode::Analog);
  ExtendedCommandPin.group().PUPDR()->setPull(ExtendedCommandPin.pin(), GPIO::PUPDR::Pull::None);

  TearingEffectPin.group().MODER()->setMode(TearingEffectPin.pin(), GPIO::MODER::Mode::Analog);
}

void initFSMC() {
  /* Set up the FSMC control registers.
   * We address the LCD panel as if it were an SRAM module, using a 16bits wide
   * bus, non-multiplexed.
   * The STM32 FSMC supports two kinds of memory access modes :
   *  - Base modes (1 and 2), which use the same timings for reads and writes
   *  - Extended modes (named A to D), which can be customized further.
   *  The LCD panel can be written to faster than it can be read from, therefore
   *  we want to use one of the extended modes. */
  FSMC.BCR(FSMCMemoryBank)->setEXTMOD(true);
  FSMC.BCR(FSMCMemoryBank)->setWREN(true);
  FSMC.BCR(FSMCMemoryBank)->setMWID(FSMC::BCR::MWID::SIXTEEN_BITS);
  FSMC.BCR(FSMCMemoryBank)->setMTYP(FSMC::BCR::MTYP::SRAM);
  FSMC.BCR(FSMCMemoryBank)->setMUXEN(false);
  FSMC.BCR(FSMCMemoryBank)->setMBKEN(true);

  /* We now need to set the actual timings. First, the FSMC and LCD specs don't
   * use the same names. Here's the mapping:
   *
   * FSMC | LCD
   * -----+-----
   *  NOE | RDX
   *  NWE | WRX
   *  NE1 | CSX
   *  A16 | D/CX
   *  Dn  | Dn
   *
   * We need to set the values of the BTR and BWTR which gives the timings for
   * reading and writing. Note that the STM32 datasheet doesn't take into
   * account the time needed to actually switch from one logic state to another,
   * whereas the ST7789V one does, so we'll add T(R) and T(F) as needed.
   * Last but not least, timings on the STM32 have to be expressed in terms of
   * HCLK = 1/96MHz = 10.42ns.
   *  - We'll pick Mode A which corresponds to SRAM with OE toggling
   *  - ADDSET = T(AST) + T(F) = 0ns + 15ns = 2 HCLK
   *  - ADDHLD is unused in this mode, set to 0
   *  - DATAST(read) = T(RDLFM) + T(R) = 355ns + 15ns = 36 HCLK
   *    DATAST(write) = T(WRL) + T(R) = 15ns + 15ns = 3 HCLK
   *  - BUSTURN(read) = T(RDHFM) + T(F) = 90ns + 15ns = 10 HCLK
   *    BUSTURN(write) = T(RDHFM) + T(F) = 15ns + 15ns = 3 HCLK
   */

  // Read timing from the LCD
  FSMC.BTR(FSMCMemoryBank)->setADDSET(2);
  FSMC.BTR(FSMCMemoryBank)->setADDHLD(0);
  FSMC.BTR(FSMCMemoryBank)->setDATAST(36);
  FSMC.BTR(FSMCMemoryBank)->setBUSTURN(10);
  FSMC.BTR(FSMCMemoryBank)->setACCMOD(FSMC::BTR::ACCMOD::A);

  // Write timings for the LCD
  FSMC.BWTR(FSMCMemoryBank)->setADDSET(2);
  FSMC.BWTR(FSMCMemoryBank)->setADDHLD(0);
  FSMC.BWTR(FSMCMemoryBank)->setDATAST(3);
  FSMC.BWTR(FSMCMemoryBank)->setBUSTURN(3);
  FSMC.BWTR(FSMCMemoryBank)->setACCMOD(FSMC::BWTR::ACCMOD::A);
}

void shutdownFSMC() {
}

void initPanel() {
  send_command(Command::Reset);
  msleep(5);

  send_command(Command::SleepOut);
  msleep(5);

  send_command(Command::PixelFormatSet, 0x05);
  send_command(Command::TearingEffectLineOn, 0x00);
  send_command(Command::FrameRateControl, 0x1E); // 40 Hz frame rate

  send_command(Command::DisplayOn);
}

void shutdownPanel() {
  send_command(Command::DisplayOff);
  send_command(Command::SleepIn);
  msleep(5);
}

void setDrawingArea(KDRect r, Orientation o) {
  uint16_t x_start, x_end, y_start, y_end;

  if (o == Orientation::Landscape) {
    send_command(Command::MemoryAccessControl, 0xA0);
    x_start = r.x();
    x_end = r.x() + r.width() - 1;
    y_start = r.y();
    y_end = r.y() + r.height() - 1;
  } else {
    send_command(Command::MemoryAccessControl, 0x00);
    x_start = r.y();
    x_end = r.y() + r.height() - 1;
    y_start = Ion::Display::Width - (r.x() + r.width());
    y_end = Ion::Display::Width - r.x() - 1;
  }

  send_command(
    Command::ColumnAddressSet,
    (x_start >> 8),
    (x_start & 0xFF),
    (x_end >> 8),
    (x_end & 0xFF)
  );

  send_command(
    Command::PageAddressSet,
    (y_start >> 8),
    (y_start & 0xFF),
    (y_end >> 8),
    (y_end & 0xFF)
  );
}

void pushPixels(const KDColor * pixels, size_t numberOfPixels) {
  send_command(Command::MemoryWrite);
  /* Theoretically, we should not be able to use DMA here. Indeed, we have no
   * guarantee that the content at "pixels" will remain valid once we exit this
   * function call. In practice, we might be able to use DMA here because most
   * of the time we push pixels from static locations. */
#if USE_DMA_FOR_PUSH_PIXELS
  startDMAUpload(pixels, true, numberOfPixels);
#else
  while (numberOfPixels > 8) {
    send_data(*pixels++);
    send_data(*pixels++);
    send_data(*pixels++);
    send_data(*pixels++);
    send_data(*pixels++);
    send_data(*pixels++);
    send_data(*pixels++);
    send_data(*pixels++);
    numberOfPixels -= 8;
  }
  while (numberOfPixels--) {
    send_data(*pixels++);
  }
#endif
}

void pushColor(KDColor color, size_t numberOfPixels) {
  send_command(Command::MemoryWrite);
#if USE_DMA_FOR_PUSH_COLOR
  /* The "color" variable lives on the stack. We cannot take its address because
   * it will stop being valid as soon as we return. An easy workaround is to
   * duplicate the content in a static variable, whose value is guaranteed to be
   * kept until the next pushColor call. */
  static KDColor staticColor;
  staticColor = color;
  startDMAUpload(&staticColor, false, (numberOfPixels > 64000 ? 64000 : numberOfPixels));
#else
  while (numberOfPixels--) {
    send_data(color);
  }
#endif
}

void pullPixels(KDColor * pixels, size_t numberOfPixels) {
  if (numberOfPixels == 0) {
    return;
  }
  send_command(Command::PixelFormatSet, 0x06);
  send_command(Command::MemoryRead);

  receive_data(); // First read is dummy data, per datasheet
  while (true) {
    if (numberOfPixels == 0) {
      break;
    }
    uint16_t one = receive_data();
    uint16_t two = receive_data();
    uint16_t firstPixel  = (one & 0xF800) | (one & 0xFC) << 3 | (two & 0xF800) >> 11;
    *pixels++ = KDColor::RGB16(firstPixel);
    numberOfPixels--;

    if (numberOfPixels == 0) {
      break;
    }
    uint16_t three = receive_data();
    uint16_t secondPixel = (two & 0xF8) << 8 | (three & 0xFC00) >> 5 | (three & 0xF8) >> 3;
    *pixels++ = KDColor::RGB16(secondPixel);
    numberOfPixels--;
  }
  send_command(Command::PixelFormatSet, 0x05);
}

}
}
}
