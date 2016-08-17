#ifndef REGS_GPIO_H
#define REGS_GPIO_H

#include "register.h"

class GPIO {
public:
  class MODER : Register32 {
  public:
    enum class MODE {
      Input = 0,
      Output = 1,
      AlternateFunction = 2,
      Analog = 3
    };
    MODE getMODER(int index) { return (MODE)get(2*index+1, 2*index); };
    void setMODER(int index, MODE mode) volatile { set(2*index+1, 2*index, (uint32_t)mode); };
  };

  class ODR : Register32 {
  public:
    bool getODR(int index) { return (bool)get(index, index); }
    void setODR(int index, bool state) volatile { set(index, index, state); }
  };

  class AFR : Register64 {
    /* The AFR register doesn't exist per-se in the documentation. It is the
     * consolidation of AFRL and AFRH which are two 32 bits registers. */
  public:
    enum class AlternateFunction {
      AF0 = 0,   AF1 = 1,   AF2 = 2,   AF3 = 3,
      AF4 = 4,   AF5 = 5,   AF6 = 6,   AF7 = 7,
      AF8 = 8,   AF9 = 9,   AF10 = 10, AF11 = 11,
      AF12 = 12, AF13 = 13, AF14 = 14, AF15 = 15
    };
    AlternateFunction getAFR(int index) {
      return (AlternateFunction)get(4*index+3, 4*index);
    };
    void setAFR(int index, AlternateFunction af) volatile {
      set(4*index+3, 4*index, (uint64_t)af);
    };
  };

  constexpr GPIO(int i) : m_index(i) {}
  REGS_REGISTER_AT(MODER, 0x00);
  REGS_REGISTER_AT(ODR, 0x14);
  REGS_REGISTER_AT(AFR, 0x20);
private:
  constexpr uint32_t Base() const {
    return 0x40020000 + 0x400*m_index;
  };
  int m_index;
};

constexpr GPIO GPIOA(0);
constexpr GPIO GPIOB(1);
constexpr GPIO GPIOC(2);
constexpr GPIO GPIOD(3);
constexpr GPIO GPIOE(4);
constexpr GPIO GPIOF(5);
constexpr GPIO GPIOG(6);
constexpr GPIO GPIOH(7);

#endif
