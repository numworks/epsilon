#ifndef REGS_TIM_H
#define REGS_TIM_H

#include "register.h"

template <typename RegisterWidth>
class TIM {
public:
  class CR1 : Register16 {
  public:
    REGS_BOOL_FIELD(CEN, 0);
    REGS_BOOL_FIELD(URS, 1);
    REGS_BOOL_FIELD(UDIS, 1);
    REGS_BOOL_FIELD(ARPE, 7);
  };

  class SR : Register16 {
  public:
    REGS_BOOL_FIELD(CC4OF, 12);
    REGS_BOOL_FIELD(CC3OF, 11);
    REGS_BOOL_FIELD(CC2OF, 10);
    REGS_BOOL_FIELD(CC1OF, 9);
    REGS_BOOL_FIELD(CC4IF, 4);
    REGS_BOOL_FIELD(CC3IF, 3);
    REGS_BOOL_FIELD(CC2IF, 2);
    REGS_BOOL_FIELD(CC1IF, 1);
    REGS_BOOL_FIELD(UIF, 0);
  };

  class EGR : Register16 {
  public:
    REGS_BOOL_FIELD(UG, 0);
  };

  class DIER : Register16 {
  public:
    REGS_BOOL_FIELD(UIE, 0);
  };

  class CCMR : Register64 {
    /* We're declaring CCMR as a 64 bits register. CCMR doesn't exsist per se,
     * it is in fact the consolidation of CCMR1 and CCMR2. Both are 16 bits
     * registers, so one could expect the consolidation to be 32 bits. However,
     * both CCMR1 and CCMR2 live on 32-bits boundaries, so the consolidation has
     * to be 64 bits wide, even though we'll only use 32 bits out of 64. */
  public:
    enum class CC1S : uint8_t {
      OUTPUT = 0,
      INPUT_TI2 = 1,
      INPUT_TI1 = 2,
      INPUT_TRC = 3
    };
    enum class CC4S : uint8_t {
      OUTPUT = 0,
      INPUT_TI4 = 1,
      INPUT_TI3 = 2,
      INPUT_TRC = 3
    };
    enum class OCM : uint8_t {
      Frozen = 0,
      ActiveOnMatch = 1,
      InactiveOnMatch = 2,
      Toggle = 3,
      ForceInactive = 4,
      ForceActive = 5,
      PWM1 = 6,
      PWM2 = 7
    };
    typedef OCM OC1M;
    typedef OCM OC2M;
    typedef OCM OC3M;
    typedef OCM OC4M;
    REGS_BOOL_FIELD(OC1PE, 3);
    REGS_TYPE_FIELD(OC1M, 6, 4);
    REGS_BOOL_FIELD(OC2PE, 11);
    REGS_TYPE_FIELD(OC2M, 14, 12);
    REGS_BOOL_FIELD(OC3PE, 35);
    REGS_TYPE_FIELD(OC3M, 38, 36);
    REGS_TYPE_FIELD(CC4S, 41, 40);
    REGS_FIELD(IC4PSC, uint8_t, 43, 42);
    REGS_BOOL_FIELD(OC4PE, 43);
    REGS_TYPE_FIELD(OC4M, 46, 44);
  };

  class CCER : Register16 {
  public:
    REGS_BOOL_FIELD(CC1E, 0);
    REGS_BOOL_FIELD(CC2E, 4);
    REGS_BOOL_FIELD(CC3E, 8);
    REGS_BOOL_FIELD(CC4E, 12);
  };

  class BDTR : Register16 {
  public:
    REGS_BOOL_FIELD(MOE, 15);
  };

  class CNT : public RegisterWidth {};
  class PSC : public Register16 {};
  class ARR : public RegisterWidth {};
  class CCR1 : public RegisterWidth {};
  class CCR2 : public RegisterWidth {};
  class CCR3 : public RegisterWidth {};
  class CCR4 : public RegisterWidth {};

  class OR : Register16 {
  public:
    enum class TI4_RMP {
      GPIO = 0,
      LSI = 1,
      LSE = 2,
      WAKEUP = 3
    };
    void setTI4RMP(TI4_RMP r) volatile { setBitRange(7, 6, (uint32_t)r); }
  };

  constexpr TIM(int i) : m_index(i) {}
  REGS_REGISTER_AT(CR1, 0x0);
  REGS_REGISTER_AT(DIER, 0xC);
  REGS_REGISTER_AT(SR, 0x10);
  REGS_REGISTER_AT(EGR, 0x14);
  REGS_REGISTER_AT(CCMR, 0x18);
  REGS_REGISTER_AT(CCER, 0x20);
  REGS_REGISTER_AT(CNT, 0x24);
  REGS_REGISTER_AT(PSC, 0x28);
  REGS_REGISTER_AT(ARR, 0x2C);
  REGS_REGISTER_AT(CCR1, 0x34);
  REGS_REGISTER_AT(CCR2, 0x38);
  REGS_REGISTER_AT(CCR3, 0x3C);
  REGS_REGISTER_AT(CCR4, 0x40);
  REGS_REGISTER_AT(BDTR, 0x44);
  REGS_REGISTER_AT(OR, 0x50);
private:
  constexpr uint32_t Base() const {
    return (m_index == 1 ? 0x40010000 :
            (m_index <= 7 ? 0x40000000 + 0x400*(m_index-2) :
             (m_index == 8 ? 0x40010400 :
              (m_index <= 11 ? 0x40014000 + 0x400*(m_index-9) :
               0x40001800 + 0x400*(m_index-12)
              )
             )
            )
           );
  }
  int m_index;
};

constexpr TIM<Register16> TIM1(1);
constexpr TIM<Register32> TIM2(2);
constexpr TIM<Register16> TIM3(3);
constexpr TIM<Register16> TIM4(4);
constexpr TIM<Register32> TIM5(5);
constexpr TIM<Register16> TIM6(6);
constexpr TIM<Register16> TIM7(7);
constexpr TIM<Register16> TIM8(8);
constexpr TIM<Register16> TIM9(9);
constexpr TIM<Register16> TIM10(10);
constexpr TIM<Register16> TIM11(11);
constexpr TIM<Register16> TIM12(12);
constexpr TIM<Register16> TIM13(13);
constexpr TIM<Register16> TIM14(14);

#endif
