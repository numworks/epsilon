#ifndef STM32_REGISTERS_RCC_H
#define STM32_REGISTERS_RCC_H 1

// Reset and clock control

#define RCC_BASE 0x40023800

#define RCC_REGISTER_AT(offset) (*(volatile uint32_t *)(RCC_BASE+offset))

// RCC clock control register

#define RCC_CR RCC_REGISTER_AT(0x00)

#define HSION (1<<0)
#define HSIRDY (1<<1)
#define LOW_BIT_HSITRIM 3
#define HIGH_BIT_HSITRIM 7
#define LOW_BIT_HSICAL 8
#define HIGH_BIT_HSICAL 15
#define HSEON (1<<16)
#define HSERDY (1<<17)
#define HSEBYP (1<<18)
#define CSSON (1<<19)
#define PLLON (1<<24)
#define PLLRDY (1<<25)
#define PLLI2SON (1<<26)
#define PLLI2SRDY (1<<27)
#define PLLSAION (1<<28)
#define PLLSAIRDY (1<<29)

// RCC AHB1 peripheral clock enable register

#define RCC_AHB1ENR RCC_REGISTER_AT(0x30)

#define GPIOAEN (1<<0)
#define GPIOBEN (1<<1)
#define GPIOCEN (1<<2)
#define GPIODEN (1<<3)
#define GPIOEEN (1<<4)
#define GPIOFEN (1<<5)
#define GPIOGEN (1<<6)
#define GPIOHEN (1<<7)
#define GPIOIEN (1<<8)
#define GPIOJEN (1<<9)
#define GPIOKEN (1<<10)
#define CRCEN (1<<12)
#define BKPSRAMEN (1<<18)
#define CCMDATARAMEN (1<<20)
#define DMA1EN (1<<21)
#define DMA2EN (1<<22)
#define DMA2DEN (1<<23)
#define ETHMACEN (1<<25)
#define ETHMACTXEN (1<<26)
#define ETHMACRXEN (1<<27)
#define ETHMACPTPEN (1<<28)
#define OTGHSEN (1<<29)
#define OTGHSULPIEN (1<<30)

// RCC APB1 peripheral clock enable register

#define RCC_APB1ENR RCC_REGISTER_AT(0x40)

#define TIM2EN (1<<0)
#define TIM3EN (1<<1)
#define TIM4EN (1<<2)
#define TIM5EN (1<<3)
#define TIM6EN (1<<4)
#define TIM7EN (1<<5)
#define TIM12EN (1<<6)
#define TIM13EN (1<<7)
#define TIM14EN (1<<8)
#define WWDGEN (1<<11)
#define SPI2EN (1<<14)
#define SPI3EN (1<<15)
#define USART2EN (1<<17)
#define USART3EN (1<<18)
#define UART4EN (1<<19)
#define UART5EN (1<<20)
#define I2C1EN (1<<21)
#define I2C2EN (1<<22)
#define I2C3EN (1<<23)
#define CAN1EN (1<<25)
#define CAN2EN (1<<26)
#define PWREN (1<<28)
#define DACEN (1<<29)
#define UART7EN (1<<30)
#define UART8EN (1<<31)

// RCC APB2 peripheral clock enable register

#define RCC_APB2ENR RCC_REGISTER_AT(0x44)

#define TIM1EN (1<<0)
#define TIM8EN (1<<1)
#define USART1EN (1<<4)
#define USART6EN (1<<5)
#define ADC1 (1<<8)
#define ADC2 (1<<9)
#define ADC3 (1<<10)
#define SDIOEN (1<<11)
#define SPI1EN (1<<12)
#define SPI4EN (1<<13)
#define SYSCFGEN (1<<14)
#define TIM9EN (1<<16)
#define TIM10EN (1<<17)
#define TIM11EN (1<<18)
#define SPI5EN (1<<20)
#define SPI6EN (1<<21)
#define SAI1EN (1<<22)
#define LTDCEN (1<<26)

// RCC PLL configuration register

#define RCC_PLLSAICFGR RCC_REGISTER_AT(0x88)

#define LOW_BIT_PLLSAIN 6
#define HIGH_BIT_PLLSAIN 14
#define RCC_PLLSAIN(v) REGISTER_FIELD_VALUE(PLLSAIN, v)
#define LOW_BIT_PLLSAIQ 24
#define HIGH_BIT_PLLSAIQ 27
#define RCC_PLLSAIQ(v) REGISTER_FIELD_VALUE(PLLSAIQ, v)
#define LOW_BIT_PLLSAIR 28
#define HIGH_BIT_PLLSAIR 30
#define RCC_PLLSAIR(v) REGISTER_FIELD_VALUE(PLLSAIR, v)

// RCC dedicated clock configuration register

#define RCC_DCKCFGR RCC_REGISTER_AT(0x8C)

#define RCC_PLLSAIDIVR_DIV2 0
#define RCC_PLLSAIDIVR_DIV4 1
#define RCC_PLLSAIDIVR_DIV8 2
#define RCC_PLLSAIDIVR_DIV16 3

#define LOW_BIT_PLLIS2DIVQ 0
#define HIGH_BIT_PLLIS2DIVQ 4
#define LOW_BIT_PLLSAIDIVQ 8
#define HIGH_BIT_PLLSAIDIVQ 12
#define LOW_BIT_PLLSAIDIVR 16
#define HIGH_BIT_PLLSAIDIVR 17
#define LOW_BIT_SAI1ASRC 20
#define HIGH_BIT_SAI1ASRC 21
#define LOW_BIT_SAI1BSRC 22
#define HIGH_BIT_SAI1BSRC 23
#define TIMPRE (1<<24)

#endif
