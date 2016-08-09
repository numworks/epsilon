#include <stdint.h>

template <typename T>
class Register {
public:
  Register() = delete;
  void set(T value) volatile {
    m_value = value;
  };
  T get() volatile {
    return m_value;
  };
protected:
  static constexpr T bit_range_mask(uint8_t high, uint8_t low) {
    return ((((T)1)<<(high-low+1))-1)<<low;
  };
  static constexpr T bit_range_value(T value, uint8_t high, uint8_t low) {
    return (value<<low) & bit_range_mask(high,low);
  }
  static constexpr T bit_range_set_value(uint8_t high, uint8_t low, T originalValue, T targetValue) {
    return (originalValue & ~bit_range_mask(high,low))|bit_range_value(targetValue, high, low);
  };
  void set(uint8_t high, uint8_t low, T value) volatile {
    m_value = bit_range_set_value(high, low, m_value, value);
  };
  T get(uint8_t high, uint8_t low) volatile {
    return (m_value & bit_range_mask(high,low)) >> low;
  }
private:
  T m_value;
};

typedef Register<uint16_t> Register16;
typedef Register<uint32_t> Register32;

// RCC

#define boolField(name,bit) bool get##name() volatile { return get(bit,bit); }; void set##name(bool enable) volatile { set(bit,bit,enable); };
#define typeField(name,high,low) name get##name() volatile { return (name)get(high,low); }; void set##name(name v) volatile { set(high, low, (uint8_t)v); };

#define registerAt(name, offset) volatile name * name() const { return (class name *)(Base() + offset); };

// FIXME: uint8_t above will not work if the field is >8bits

constexpr uint32_t RCC_BASE = 0x40023800;

class RCC_AHB1ENR : Register32 {
public:
  boolField(GPIOAEN, 0);
  boolField(GPIOBEN, 1);
  boolField(GPIOCEN, 2);
  boolField(GPIODEN, 3);
  boolField(GPIOEEN, 4);
  boolField(GPIOFEN, 5);
  boolField(GPIOGEN, 6);
  boolField(GPIOHEN, 7);
  boolField(CRCEN, 12);
  boolField(DMA1EN, 21);
  boolField(DMA2EN, 22);
};

static inline RCC_AHB1ENR * RCC_AHB1ENR() { return (class RCC_AHB1ENR *)(RCC_BASE + 0x30); };

class RCC_APB2ENR : Register32 {
public:
  boolField(TIM1EN, 0);
};

static inline RCC_APB2ENR * RCC_APB2ENR() { return (class RCC_APB2ENR *)(RCC_BASE + 0x44); };

/// GPIO

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

  class AFR : Register<uint64_t> {
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

  constexpr GPIO(int i) : m_index(i) {};
  registerAt(MODER, 0x00);
  registerAt(AFR, 0x20);
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

// TIMER

class TIM {
public:
  class CR1 : Register16 {
  public:
    boolField(CEN, 0);
    boolField(ARPE, 7);
  };

  class CCMR1 : Register16 {
  public:
    enum class CC1S : uint8_t {
      OUTPUT = 0,
      INPUT_TI2 = 1,
      INPUT_TI1 = 2,
      INPUT_TRC = 3
    };
    enum class OC1M : uint8_t {
      Frozen = 0,
      ActiveOnMatch = 1,
      InactiveOnMatch = 2,
      Toggle = 3,
      ForceInactive = 4,
      ForceActive = 5,
      PWM1 = 6,
      PWM2 = 7
    };
    typedef OC1M OC2M;
    typeField(CC1S, 1, 0);
    boolField(OC1PE, 3);
    typeField(OC1M, 6, 4);
    boolField(OC2PE, 11);
    typeField(OC2M, 14, 12);
  };

  class CCER : Register16 {
  public:
    boolField(CC1E, 0);
    boolField(CC2E, 4);
  };

  class BDTR : Register16 {
  public:
    boolField(MOE, 15);
  };

  class PSC : public Register16 {};
  class ARR : public Register16 {};
  class CCR1 : public Register16 {};
  class CCR2 : public Register16 {};

  constexpr TIM(int i) : m_index(i) {};
  registerAt(CR1, 0x0);
  registerAt(CCMR1, 0x18);
  registerAt(CCER, 0x20);
  registerAt(PSC, 0x28);
  registerAt(ARR, 0x2C);
  registerAt(CCR1, 0x34);
  registerAt(CCR2, 0x38);
  registerAt(BDTR, 0x44);
private:
  constexpr uint32_t Base() const {
    return (m_index == 1 ? 0x40010000 : 0x40000000 + 0x400*m_index);
  };
  int m_index;
};

constexpr TIM TIM1(1);


