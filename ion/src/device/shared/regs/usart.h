#ifndef REGS_USART_H
#define REGS_USART_H

#include "register.h"
#include <regs/config/usart.h>

class USART {
public:
  class SR : Register32 {
  public:
    REGS_BOOL_FIELD(RXNE, 5);
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
    // TODO one value only on 0-15
    REGS_FIELD(DIV_FRAC, uint8_t, 3, 0);
    REGS_FIELD(DIV_MANTISSA, uint16_t, 15, 4);
  };

  class CR1 : Register32 {
  public:
    REGS_BOOL_FIELD(UE, Ion::Device::USART::Config::UEOffset);
    REGS_BOOL_FIELD(TE, 3);
    REGS_BOOL_FIELD(RE, 2);
  };

  constexpr USART(int i) : m_index(i) {}
  constexpr operator int() const { return m_index; }
  REGS_REGISTER_AT(SR, Ion::Device::USART::Config::SROffset);
  REGS_REGISTER_AT(RDR, Ion::Device::USART::Config::RDROffset);
  REGS_REGISTER_AT(TDR, Ion::Device::USART::Config::TDROffset);
  REGS_REGISTER_AT(BRR, Ion::Device::USART::Config::BRROffset);
  REGS_REGISTER_AT(CR1, Ion::Device::USART::Config::CR1Offset);
private:
  constexpr uint32_t Base() const {
    return ((uint32_t []){0x40011000, 0x40004400, 0x40004800, 0x40004C00, 0x40005000, 0x40011400})[m_index-1];
  };
  int m_index;
};

#endif
