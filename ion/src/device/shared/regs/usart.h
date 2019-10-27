#ifndef REGS_USART_H
#define REGS_USART_H

#include "register.h"
#include <regs/config/usart.h>

namespace Ion {
namespace Device {
namespace Regs {

class USART {
public:
  class SR : Register32 {
  public:
    REGS_BOOL_FIELD(RXNE, 5);
    REGS_BOOL_FIELD(TC, 6);
    REGS_BOOL_FIELD(TXE, 7);
  };

  // RDR and TDR are the same DR register on some models.
  class RDR : Register32 {
  public:
    uint16_t get() volatile {
      return (uint16_t)getBitRange(8, 0);
    }
  };

  class TDR : Register32 {
  public:
    void set(uint16_t v) volatile {
      setBitRange(8, 0, v);
    }
  };

  class BRR : Register32 {
  public:
    void set(uint16_t v) volatile {
      setBitRange(15, 0, v);
    }
  };

  class CR1 : Register32 {
  public:
    REGS_BOOL_FIELD(UE, REGS_USART_CR1_UE_BIT);
    REGS_BOOL_FIELD(TE, 3);
    REGS_BOOL_FIELD(RE, 2);
  };

  constexpr USART(int i) : m_index(i) {}
  constexpr operator int() const { return m_index; }
  REGS_REGISTER_AT(SR, REGS_USART_SR_OFFSET);
  REGS_REGISTER_AT(RDR, REGS_USART_RDR_OFFSET);
  REGS_REGISTER_AT(TDR, REGS_USART_TDR_OFFSET);
  REGS_REGISTER_AT(BRR, REGS_USART_BRR_OFFSET);
  REGS_REGISTER_AT(CR1, REGS_USART_CR1_OFFSET);
private:
  constexpr uint32_t Base() const {
    return ((uint32_t []){0x40011000, 0x40004400, 0x40004800, 0x40004C00, 0x40005000, 0x40011400})[m_index-1];
  };
  int m_index;
};

}
}
}

#endif
