#ifndef REGS_QUADSPI_H
#define REGS_QUADSPI_H

#include "register.h"

namespace Ion {
namespace Device {
namespace Regs {

// Quad-SPI register map on STM32 microcontroller
// See section 12 of the STM32F412 reference manual

class QUADSPI {
public:
  class CR : public Register32 { // Control register
  public:
    using Register32::Register32;
    REGS_BOOL_FIELD(EN, 0);
    REGS_BOOL_FIELD(ABORT, 1);
    REGS_BOOL_FIELD(TCEN, 3); // Lower-power timeout counter enable in memory-mapped mode
    REGS_BOOL_FIELD(SSHIFT, 4);
    REGS_FIELD(PRESCALER, uint8_t, 31, 24);
  };

  class DCR : public Register32 { // Device configuration register
  public:
    using Register32::Register32;
    REGS_BOOL_FIELD(CKMODE, 0); // MODE 0 or 3
    REGS_FIELD(CSHT, uint8_t, 10, 8); // Chip select minimum high time
    REGS_FIELD(FSIZE, uint8_t, 20, 16);
  };

  class SR : Register32 { // Status register
  public:
    REGS_BOOL_FIELD(TEF, 0); // Transfer error flag
    REGS_BOOL_FIELD(TCF, 1); // Transfer complete flag
    REGS_BOOL_FIELD(BUSY, 5);
  };

  class FCR : Register32 { // Flag clear register
  };

  class DLR : public Register32 { // Data length register
    // In indirect and status-polling modes
    // Number of bytes to be transferred = DLR + 1
  };

  class CCR : public Register32 { // Communication configuration register
  public:
    using Register32::Register32;
    enum class Size : uint8_t {
      OneByte = 0,
      TwoBytes = 1,
      ThreeBytes = 2,
      FourBytes = 3
    };
    enum class OperatingMode : uint8_t {
      NoData = 0,
      Single = 1,
      Dual = 2,
      Quad = 3
    };
    enum class FunctionalMode : uint8_t {
      IndirectWrite = 0,
      IndirectRead = 1,
      StatusPolling = 2,
      MemoryMapped = 3
    };
    REGS_FIELD(INSTRUCTION, uint8_t, 7, 0);
    REGS_FIELD(IMODE, OperatingMode, 9, 8);
    REGS_FIELD(ADMODE, OperatingMode, 11, 10);
    REGS_FIELD(ADSIZE, Size, 13, 12);
    REGS_FIELD(ABMODE, OperatingMode, 15, 14);
    REGS_FIELD(ABSIZE, Size, 17, 16);
    REGS_FIELD(DCYC, uint8_t, 22, 18);
    REGS_FIELD(DMODE, OperatingMode, 25, 24);
    REGS_FIELD(FMODE, FunctionalMode, 27, 26);
    REGS_BOOL_FIELD(SIOO, 28);
  };

  class AR : public Register32 { // Address register
  };

  class ABR : public Register32 { // Alternate bytes register
  };

  class DR : public Register8 { // Data register
    /* In indirect (read or write) mode, any data must be written to or read from
     * the data register, which supports byte, halfword and word access
     * (aligned to the bottom of the register).
     * N bytes read/written remove/add N bytes from/to the FIFO.
     */
  };

  class PSMKR : Register32 { // Polling status mask register
  };

  class PSMAR : Register32 { // Polling status match register
  };

  class PIR : Register32 { // Polling interval register
  };

  class LPTR : public Register16 { // Low-power timeout register
    // Period before putting the Flash memory in lower-power state (in memory-mapped mode)
  };

  constexpr QUADSPI() {};

  REGS_REGISTER_AT(CR, 0x00);
  REGS_REGISTER_AT(DCR, 0x04);
  REGS_REGISTER_AT(SR, 0x08);
  REGS_REGISTER_AT(FCR, 0x0C);
  REGS_REGISTER_AT(DLR, 0x10);
  REGS_REGISTER_AT(CCR, 0x14);
  REGS_REGISTER_AT(AR, 0x18);
  REGS_REGISTER_AT(ABR, 0x1C);
  REGS_REGISTER_AT(DR, 0x20);
  REGS_REGISTER_AT(PSMKR, 0x24);
  REGS_REGISTER_AT(PSMAR, 0x28);
  REGS_REGISTER_AT(PIR, 0x2C);
  REGS_REGISTER_AT(LPTR, 0x30);
private:
  constexpr uint32_t Base() const {
    return 0xA0001000;
  }
};

constexpr QUADSPI QUADSPI;

}
}
}

#endif
