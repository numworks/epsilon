#ifndef REGS_SPI_H
#define REGS_SPI_H

#include "register.h"

namespace Ion {
namespace Device {
namespace Regs {

class SPI {
public:
  class CR1 : Register16 {
  public:
    REGS_BOOL_FIELD(MSTR, 2);
    REGS_BOOL_FIELD(SPE, 6);
    REGS_BOOL_FIELD(LSBFIRST, 7);
    REGS_BOOL_FIELD(SSI, 8);
    REGS_BOOL_FIELD(SSM, 9);
    REGS_BOOL_FIELD(RXONLY, 10);
    REGS_BOOL_FIELD(DFF, 11);
  };
  class CR2 : Register16 {
  public:
    REGS_BOOL_FIELD(RXDMAEN, 0);
  };
  class SR : Register16 {
  public:
    REGS_BOOL_FIELD(RXNE, 0);
    REGS_BOOL_FIELD(TXE, 1);
  };
  class DR : public Register16 {
  };

  constexpr SPI(int i) : m_index(i) {}
  constexpr operator int() const { return m_index; }
  REGS_REGISTER_AT(CR1, 0x00);
  REGS_REGISTER_AT(CR2, 0x04);
  REGS_REGISTER_AT(SR, 0x08);
  REGS_REGISTER_AT(DR, 0x0C);
private:
  constexpr uint32_t Base() const {
    return ((uint32_t []){0x40013000, 0x40003800, 0x40003C00})[m_index-1];
  };
  int m_index;
};

constexpr SPI SPI1(1);

}
}
}

#endif
