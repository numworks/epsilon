#ifndef STM32_REGISTERS_TIM_H
#define STM32_REGISTERS_TIM_H 1

#define TIM1_BASE 0x40010000
#define TIM2_BASE 0x40000000
#define TIM3_BASE 0x40000400
#define TIM4_BASE 0x40000800

#define TIM_REGISTER_AT(timer,offset) (*(volatile uint16_t *)(timer##_BASE+offset))

// TIM control registers 1

#define TIM_CR1(timer) TIM_REGISTER_AT(timer, 0x00)

#define TIM_CEN (1<<0)
#define TIM_UDIS (1<<1)
#define TIM_URS (1<<2)
#define TIM_OPM (1<<3)
#define TIM_DIR (1<<4)
//#define TIM_CMS
#define TIM_ARPE (1<<7)
#define TIM_CKD_1 0
#define TIM_CKD_2 1
#define TIM_CKD_4 2

// TIM control registers 2

#define TIM_CR2(timer) TIM_REGISTER_AT(timer, 0x04)

#define TIM_CDDS (1<<3)
#define TIM_MMS_RESET 0
#define TIM_MMS_ENABLE 1
#define TIM_MMS_UPDATE 2
#define TIM_MMS_COMPARE_PULSE 3
#define TIM_TI1S (1<<7)

// TIM capture/compare mode register 1

#define TIM_CCMR1(timer) TIM_REGISTER_AT(timer, 0x18)

//#define LOW_BIT_TIM_CC1S 0
//#define HIGH_BIT_TIM_CC1S 1
#define TIM_OC1PE (1<<3)
#define LOW_BIT_TIM_OC1M 4
#define HIGH_BIT_TIM_OC1M 6
#define TIM_OC2PE (1<<11)
#define LOW_BIT_TIM_OC2M 12
#define HIGH_BIT_TIM_OC2M 14

#define TIM_CCMR2(timer) TIM_REGISTER_AT(timer, 0x1C)

#define LOW_BIT_TIM_OC3M 4
#define HIGH_BIT_TIM_OC3M 6

// TIM capture/compare enable register
#define TIM_CCER(timer) TIM_REGISTER_AT(timer, 0x20)

#define TIM_CC1E (1<<0)
#define TIM_CC1P (1<<1)
#define TIM_CC1NE (1<<2)
#define TIM_CC2E (1<<4)
#define TIM_CC2P (1<<5)
#define TIM_CC2NE (1<<6)

#define TIM_CNT(timer) TIM_REGISTER_AT(timer, 0x24)
#define TIM_PSC(timer) TIM_REGISTER_AT(timer, 0x28)

// TIM auto-reload register
#define TIM_ARR(timer) TIM_REGISTER_AT(timer, 0x2C)

// TIM capture/compare register 1
#define TIM_CCR1(timer) TIM_REGISTER_AT(timer, 0x34)

// TIM capture/compare register 2
#define TIM_CCR2(timer) TIM_REGISTER_AT(timer, 0x38)


// TIM break and dead-time register

#define TIM_BDTR(timer) TIM_REGISTER_AT(timer, 0x44)


#define TIM_MOE (1<<15)

#endif
