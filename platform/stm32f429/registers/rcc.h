#ifndef STM32_REGISTERS_RCC_H
#define STM32_REGISTERS_RCC_H 1

/* Sample usage : "RCC_AHB1ENR->GPIOAEN = 1;" will enable the clock on GPIOA.*/
extern struct {
  unsigned int GPIOAEN:1;
  unsigned int GPIOBEN:1;
  unsigned int GPIOCEN:1;
  unsigned int GPIODEN:1;
  unsigned int GPIOEEN:1;
  unsigned int GPIOFEN:1;
  unsigned int GPIOGEN:1;
  unsigned int GPIOHEN:1;
  unsigned int GPIOIEN:1;
  unsigned int GPIOJEN:1;
  unsigned int GPIOKEN:1;
  unsigned int :1;
  unsigned int CRCEN:1;
  unsigned int :5;
  unsigned int BKPSRAMEN:1;
  unsigned int :1;
  unsigned int CCMDATARAMEN:1;
  unsigned int DMA1EN:1;
  unsigned int DMA2EN:1;
  unsigned int DMA2DEN:1;
  unsigned int :1;
  unsigned int ETHMACEN:1;
  unsigned int ETHMACTXEN:1;
  unsigned int ETHMACRXEN:1;
  unsigned int ETHMACPTPEN:1;
  unsigned int OTGHSEN:1;
  unsigned int OTGHSULPIEN:1;
  unsigned int :1;
} * RCC_AHB1ENR;

extern struct {
  unsigned int TIM1EN:1;
  unsigned int TIM8EN:1;
  unsigned int :2;
  unsigned int USART1EN:1;
  unsigned int USART6EN:1;
  unsigned int :2;
  unsigned int ADC1EN:1;
  unsigned int ADC2EN:1;
  unsigned int ADC3EN:1;
  unsigned int SDIOEN:1;
  unsigned int SPI1EN:1;
  unsigned int SPI4EN:1;
  unsigned int SYSCFGEN:1;
  unsigned int :1;
  unsigned int TIM9EN:1;
  unsigned int TIM10EN:1;
  unsigned int TIM11EN:1;
  unsigned int :1;
  unsigned int SPI5EN:1;
  unsigned int SPI6EN:1;
  unsigned int SAI1EN:1;
  unsigned int :3;
  unsigned int LTDCEN:1;
  unsigned int :5;
} * RCC_APB2ENR;

#endif
