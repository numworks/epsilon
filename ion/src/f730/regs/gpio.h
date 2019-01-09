#ifndef REGS_GPIO_H
#define REGS_GPIO_H

#include "register.h"

class GPIO {
public:
  class MODER : public Register32 {
  public:
    enum class Mode {
      Input = 0,
      Output = 1,
      AlternateFunction = 2,
      Analog = 3
    };
    Mode getMode(int index) { return (Mode)getBitRange(2*index+1, 2*index); }
    void setMode(int index, Mode mode) volatile { setBitRange(2*index+1, 2*index, (uint32_t)mode); }
  };

  class OTYPER : Register32 {
  public:
    enum class Type {
      PushPull = 0,
      OpenDrain = 1
    };
    Type getType(int index) { return (Type)getBitRange(index, index); }
    void setType(int index, Type type) volatile { setBitRange(index, index, (uint32_t)type); }
  };

  class PUPDR : public Register32 {
  public:
    enum class Pull {
      None = 0,
      Up = 1,
      Down = 2,
      Reserved = 3
    };
    Pull getPull(int index) { return (Pull)getBitRange(2*index+1, 2*index); }
    void setPull(int index, Pull pull) volatile { setBitRange(2*index+1, 2*index, (uint32_t)pull); }
  };

  class IDR : public Register32 {
  public:
    bool get(int index) volatile { return (bool)getBitRange(index, index); }
  };

  class ODR : public Register32 {
  public:
    bool get(int index) volatile { return (bool)getBitRange(index, index); }
    void set(int index, bool state) volatile { setBitRange(index, index, state); }
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
    AlternateFunction getAlternateFunction(int index) {
      return (AlternateFunction)getBitRange(4*index+3, 4*index);
    }
    void setAlternateFunction(int index, AlternateFunction af) volatile {
      setBitRange(4*index+3, 4*index, (uint64_t)af);
    }
  };

  constexpr GPIO(int i) : m_index(i) {}
  constexpr operator int() const { return m_index; }
  REGS_REGISTER_AT(MODER, 0x00);
  REGS_REGISTER_AT(OTYPER, 0x04);
  REGS_REGISTER_AT(PUPDR, 0x0C);
  REGS_REGISTER_AT(IDR, 0x10);
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

class GPIOPin {
public:
  constexpr GPIOPin(GPIO group, uint8_t pin) : m_data(((group&0xF) << 4) | (pin&0xF)) {}
  GPIO group() const { return GPIO(m_data>>4); }
  uint8_t pin() const { return m_data & 0xF; }
private:
  uint8_t m_data;
};

#endif
