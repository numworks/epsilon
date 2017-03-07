#ifndef REGS_USART_H
#define REGS_USART_H

#include "register.h"

class USART {
public:
  class SR : Register32 {
  public:
    REGS_BOOL_FIELD(RXNE, 5);
    REGS_BOOL_FIELD(TXE, 7);
  };

  class DR : Register32 {
  public:
    uint16_t get() volatile {
      return (uint16_t)getBitRange(8, 0);
    }
    void set(uint16_t v) volatile {
      setBitRange(8, 0, v);
    }
  };

  class BRR : Register32 {
  public:
    REGS_FIELD(DIV_FRAC, uint8_t, 3, 0);
    REGS_FIELD(DIV_MANTISSA, uint16_t, 15, 4);
  };

  class CR1 : Register32 {
  public:
    REGS_BOOL_FIELD(UE, 13);
    REGS_BOOL_FIELD(TE, 3);
    REGS_BOOL_FIELD(RE, 2);
  };

  constexpr USART(int i) : m_index(i) {}
  constexpr operator int() const { return m_index; }
  REGS_REGISTER_AT(SR, 0x00);
  REGS_REGISTER_AT(DR, 0x04);
  REGS_REGISTER_AT(BRR, 0x08);
  REGS_REGISTER_AT(CR1, 0x0C);
private:
  constexpr uint32_t Base() const {
    return ((uint32_t []){0x40011000, 0x40004400, 0x40004800})[m_index-1];
  };
  int m_index;
};

#endif
