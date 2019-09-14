#ifndef REGS_DMA_H
#define REGS_DMA_H

#include "register.h"

namespace Ion {
namespace Device {
namespace Regs {

class DMA {
public:
  class LIFCR : public Register32 {
  };
  class SCR : Register32 {
  public:
    enum class Burst : uint8_t {
      Single = 0,
      Incremental4 = 1,
      Incremental8 = 2,
      Incremental16 = 3
    };
    enum class DataSize : uint8_t {
      Byte = 0,
      HalfWord = 1,
      Word = 2
    };
    enum class Direction : uint8_t {
      PeripheralToMemory = 0,
      MemoryToPeripheral = 1,
      MemoryToMemory = 2
    };
    REGS_FIELD(CHSEL, uint8_t, 27, 25);
    REGS_FIELD(MBURST, Burst, 24, 23);
    REGS_FIELD(PBURST, Burst, 22, 21);
    REGS_FIELD(MSIZE, DataSize, 14, 13);
    REGS_FIELD(PSIZE, DataSize, 12, 11);
    REGS_BOOL_FIELD(MINC, 10);
    REGS_BOOL_FIELD(PINC, 9);
    REGS_BOOL_FIELD(CIRC, 8);
    REGS_FIELD(DIR, Direction, 7, 6);
    REGS_BOOL_FIELD(EN, 0);
  };
  class SNDTR : public Register32 {
  };
  class SPAR : public Register32 {
  };
  class SM0AR : public Register32 {
  };

  constexpr DMA(int i) : m_index(i) {}
  //constexpr operator int() const { return m_index; }
  REGS_REGISTER_AT(LIFCR, 0x08);
  volatile SCR * SCR(int i ) const { return (class SCR *)(Base() + 0x10 + 0x18*i); };
  volatile SNDTR * SNDTR(int i ) const { return (class SNDTR *)(Base() + 0x14 + 0x18*i); };
  volatile SPAR * SPAR(int i ) const { return (class SPAR *)(Base() + 0x18 + 0x18*i); };
  volatile SM0AR * SM0AR(int i ) const { return (class SM0AR *)(Base() + 0x1C + 0x18*i); };
private:
  constexpr uint32_t Base() const {
    return 0x40026000 + 0x400*m_index;
  };
  int m_index;
};

constexpr DMA DMA1(0);
constexpr DMA DMA2(1);

}
}
}

#endif
