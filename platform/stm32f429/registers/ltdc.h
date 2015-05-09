#ifndef STM32_REGISTERS_LTDC_H
#define STM32_REGISTERS_LTDC_H 1

// LCD-TFT Controller

#define LTDC_BASE 0x40016800

#define LTDC_REGISTER_AT(offset) (*(volatile uint32_t *)(LTDC_BASE+offset))

// LTDC synchronization size configuration register

#define LTDC_SSCR LTDC_REGISTER_AT(0x08)

#define LOW_BIT_VSH 0
#define HIGH_BIT_VSH 10
#define LTDC_VSH(v) REGISTER_FIELD_VALUE(VSH, v)
#define LOW_BIT_HSW 16
#define HIGH_BIT_HSW 27
#define LTDC_HSW(v) REGISTER_FIELD_VALUE(HSW, v)

// LTDC back porch configuration register

#define LTDC_BPCR LTDC_REGISTER_AT(0x0C)

#define LOW_BIT_AVBP 0
#define HIGH_BIT_AVBP 10
#define LTDC_AVBP(v) REGISTER_FIELD_VALUE(AVBP, v)
#define LOW_BIT_AHBP 16
#define HIGH_BIT_AHBP 27
#define LTDC_AHBP(v) REGISTER_FIELD_VALUE(AHBP, v)

// LTDC active width configuration register

#define LTDC_AWCR LTDC_REGISTER_AT(0x10)

#define LOW_BIT_AAH 0
#define HIGH_BIT_AAH 10
#define LTDC_AAH(v) REGISTER_FIELD_VALUE(AAH, v)
#define LOW_BIT_AAW 16
#define HIGH_BIT_AAW 27
#define LTDC_AAW(v) REGISTER_FIELD_VALUE(AAW, v)

// LTDC total width configuration register

#define LTDC_TWCR LTDC_REGISTER_AT(0x14)

#define LOW_BIT_TOTALH 0
#define HIGH_BIT_TOTALH 10
#define LTDC_TOTALH(v) REGISTER_FIELD_VALUE(TOTALH, v)
#define LOW_BIT_TOTALW 16
#define HIGH_BIT_TOTALW 27
#define LTDC_TOTALW(v) REGISTER_FIELD_VALUE(TOTALW, v)

// LTDC global control register

#define LTDC_GCR LTDC_REGISTER_AT(0x18)

#define LTDC_LTDCEN (1<<0) //FIXME: Prefix in RCC too!
#define LOW_BIT_DBW 4
#define HIGH_BIT_DBW 6
#define LOW_BIT_DGW 8
#define HIGH_BIT_DGW 10
#define LOW_BIT_DRW 12
#define HIGH_BIT_DRW 14
#define LTDC_DEN (1<<16)
#define LTDC_PCPOL (1<<28)
#define LTDC_DEPOL (1<<29)
#define LTDC_VSPOL (1<<30)
#define LTDC_HSPOL (1<<31)

#if 0

typedef struct {
  unsigned int IMR:1;
  unsigned int VBR:1;
  unsigned int :30;
} LTDC_SRCR_t;

extern LTDC_SRCR_t * LTDC_SRCR;

typedef struct {
  unsigned int BCBLUE:8;
  unsigned int BCGREEN:8;
  unsigned int BCRED:8;
  unsigned int :8;
} LTDC_BCCR_t;

extern LTDC_BCCR_t * LTDC_BCCR;

typedef struct {
  unsigned int LIE:1;
  unsigned int FUIE:1;
  unsigned int TERRIE:1;
  unsigned int RRIE:1;
  unsigned int :28;
} LTDC_IER_t;

extern LTDC_IER_t * LTDC_IER;

typedef struct {
  unsigned int LIF:1;
  unsigned int FUIF:1;
  unsigned int TERRIF:1;
  unsigned int RRIF:1;
  unsigned int :28;
} LTDC_ISR_t;

extern LTDC_ISR_t * LTDC_ISR;

typedef struct {
  unsigned int CLIF:1;
  unsigned int CFUIF:1;
  unsigned int CTERRIF:1;
  unsigned int CRIF:1;
  unsigned int :28;
} LTDC_ICR_t;

extern LTDC_ICR_t * LTDC_ICR;

typedef enum {
  LAYER1 = 0,
  LAYER2 = 1,
} LTDC_LAYER_t;

typedef struct {
  unsigned int LEN:1;
  unsigned int COLKEN:1;
  unsigned int :2;
  unsigned int CLUTEN:1;
  unsigned int :27;
} LTDC_LCR_t;

LTDC_LCR_t * LTDC_LCR(LTDC_LAYER_t ltdc_layer);

#endif

#endif
