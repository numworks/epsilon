#ifndef STM32_REGISTERS_LTDC_H
#define STM32_REGISTERS_LTDC_H 1

// LCD-TFT Controller

#define LTDC_BASE 0x40016800

#define LTDC_LAYER1 0
#define LTDC_LAYER2 1

#define LTDC_REGISTER_AT(offset) (*(volatile uint32_t *)(LTDC_BASE+offset))
#define LTDC_LAYER_REGISTER_AT(layer, offset) LTDC_REGISTER_AT(offset+(0x80*layer))

// LTDC synchronization size configuration register

#define LTDC_SSCR LTDC_REGISTER_AT(0x08)

#define LOW_BIT_VSH 0
#define HIGH_BIT_VSH 10
#define LTDC_VSH(v) REGISTER_FIELD_VALUE(VSH,v)
#define LOW_BIT_HSW 16
#define HIGH_BIT_HSW 27
#define LTDC_HSW(v) REGISTER_FIELD_VALUE(HSW,v)

// LTDC back porch configuration register

#define LTDC_BPCR LTDC_REGISTER_AT(0x0C)

#define LOW_BIT_AVBP 0
#define HIGH_BIT_AVBP 10
#define LTDC_AVBP(v) REGISTER_FIELD_VALUE(AVBP,v)
#define LOW_BIT_AHBP 16
#define HIGH_BIT_AHBP 27
#define LTDC_AHBP(v) REGISTER_FIELD_VALUE(AHBP,v)

// LTDC active width configuration register

#define LTDC_AWCR LTDC_REGISTER_AT(0x10)

#define LOW_BIT_AAH 0
#define HIGH_BIT_AAH 10
#define LTDC_AAH(v) REGISTER_FIELD_VALUE(AAH,v)
#define LOW_BIT_AAW 16
#define HIGH_BIT_AAW 27
#define LTDC_AAW(v) REGISTER_FIELD_VALUE(AAW,v)

// LTDC total width configuration register

#define LTDC_TWCR LTDC_REGISTER_AT(0x14)

#define LOW_BIT_TOTALH 0
#define HIGH_BIT_TOTALH 10
#define LTDC_TOTALH(v) REGISTER_FIELD_VALUE(TOTALH,v)
#define LOW_BIT_TOTALW 16
#define HIGH_BIT_TOTALW 27
#define LTDC_TOTALW(v) REGISTER_FIELD_VALUE(TOTALW,v)

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
// When PCPOL=0, data is on the rising-edge
// When PCPOL=1, data is on the falling-edge
#define LTDC_DEPOL (1<<29)
#define LTDC_VSPOL (1<<30)
#define LTDC_HSPOL (1<<31)

// LTDC shadow reload configuration register

#define LTDC_SRCR LTDC_REGISTER_AT(0x24)

#define LTDC_IMR (1<<0)
#define LTDC_VBR (1<<1)

// LTDC background color configuration register

#define LTDC_BCCR LTDC_REGISTER_AT(0x2C)

#define LOW_BIT_BCBLUE 0
#define HIGH_BIT_BCBLUE 7
#define LOW_BIT_BCGREEN 8
#define HIGH_BIT_BCGREEN 15
#define LOW_BIT_BCRED 16
#define HIGH_BIT_BCRED 23

// LTDC layer control registers

#define LTDC_LCR(layer) LTDC_LAYER_REGISTER_AT(layer,0x84)

#define LTDC_LEN (1<<0)
#define LTDC_COLKEN (1<<1)
#define LTDC_CLUTEN (1<<4)

// LTDC layer window horizontal position configuration registers

#define LTDC_LWHPCR(layer) LTDC_LAYER_REGISTER_AT(layer,0x88)

#define LOW_BIT_LTDC_WHSTPOS 0
#define HIGH_BIT_LTDC_WHSTPOS 11
#define LTDC_WHSTPOS(v) REGISTER_FIELD_VALUE(LTDC_WHSTPOS,v)
#define LOW_BIT_LTDC_WHSPPOS 16
#define HIGH_BIT_LTDC_WHSPPOS 27
#define LTDC_WHSPPOS(v) REGISTER_FIELD_VALUE(LTDC_WHSPPOS,v)

// LTDC layer window vertical position configuration registers

#define LTDC_LWVPCR(layer) LTDC_LAYER_REGISTER_AT(layer,0x8C)

#define LOW_BIT_LTDC_WVSTPOS 0
#define HIGH_BIT_LTDC_WVSTPOS 11
#define LTDC_WVSTPOS(v) REGISTER_FIELD_VALUE(LTDC_WVSTPOS,v)
#define LOW_BIT_LTDC_WVSPPOS 16
#define HIGH_BIT_LTDC_WVSPPOS 27
#define LTDC_WVSPPOS(v) REGISTER_FIELD_VALUE(LTDC_WVSPPOS,v)

// LTDC layer pixel format configuration registers

#define LTDC_LPFCR(layer) LTDC_LAYER_REGISTER_AT(layer,0x94)

#define LTDC_PF_ARGB8888 0
#define LTDC_PF_RGB888   1
#define LTDC_PF_RGB565   2
#define LTDC_PF_ARGB1555 3
#define LTDC_PF_ARGB4444 4
#define LTDC_PF_L8       5
#define LTDC_PF_AL44     6
#define LTDC_PF_AL88     7

#define LOW_BIT_LTDC_PF 0
#define HIGH_BIT_LTDC_PF 2

// LTDC layer color frame buffer address registers

#define LTDC_LCFBAR(layer) LTDC_LAYER_REGISTER_AT(layer,0xAC)

// LTDC layer color frame buffer length registers

#define LTDC_LCFBLR(layer) LTDC_LAYER_REGISTER_AT(layer,0xB0)

#define LOW_BIT_LTDC_CFBLL 0
#define HIGH_BIT_LTDC_CFBLL 12
#define LTDC_CFBLL(v) REGISTER_FIELD_VALUE(LTDC_CFBLL,v)
#define LOW_BIT_LTDC_CFBP 16
#define HIGH_BIT_LTDC_CFBP 28
#define LTDC_CFBP(v) REGISTER_FIELD_VALUE(LTDC_CFBP,v)

// LTDC layer color frame buffer line registers

#define LTDC_LCFBLNR(layer) LTDC_LAYER_REGISTER_AT(layer,0xB4)

#define LOW_BIT_LTDC_CFBLNR 0
#define HIGH_BIT_LTDC_CFBLNR 10
#define LTDC_CFBLNR(v) REGISTER_FIELD_VALUE(LTDC_CFBLNR,v)

#if 0
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
