#ifndef STM32_REGISTERS_DMA_H
#define STM32_REGISTERS_DMA_H 1

#define DMA1_BASE 0x40026000
#define DMA2_BASE 0x40026400

#define DMA_REGISTER_AT(dma,offset) (*(volatile uint32_t *)(dma##_BASE+offset))
#define DMA_STREAM_REGISTER_AT(dma,stream,offset) DMA_REGISTER_AT(dma,offset+(0x18*stream))

// DMA stream configuration register

#define DMA_SCR(dma,stream) DMA_STREAM_REGISTER_AT(dma,stream,0x10)

#define DMA_DIR_PERIPHERAL_TO_MEMORY 0
#define DMA_DIR_MEMORY_TO_PERIPHERAL 1
#define DMA_DIR_MEMORY_TO_MEMORY 2

#define DMA_PSIZE_8_BITS 0
#define DMA_PSIZE_16_BITS 1
#define DMA_PSIZE 32_BITS 2

#define DMA_EN (1<<0)
#define DMA_DMEIE (1<<1)
#define DMA_TEIE (1<<2)
#define DMA_HTIE (1<<3)
#define DMA_TCIE (1<<4)
#define DMA_PFCTRL (1<<5)
#define LOW_BIT_DMA_DIR 6
#define HIGH_BIT_DMA_DIR 7
#define DMA_CIRC (1<<8)
#define DMA_PINC (1<<9)
#define DMA_MINC (1<<10)
#define LOW_BIT_DMA_PSIZE 11
#define HIGH_BIT_DMA_PSIZE 12
#define LOW_BIT_DMA_MSIZE 13
#define HIGH_BIT_DMA_MSIZE 14
#define DMA_PINCOS (1<<15)
#define LOW_BIT_DMA_PL 16
#define HIGH_BIT_DMA_PL 17
#define DMA_DBM (1<<18)
#define DMA_CT (1<<19)
#define LOW_BIT_DMA_PBURST 21
#define HIGH_BIT_DMA_PBURST 22
#define LOW_BIT_DMA_MBURST 23
#define HIGH_BIT_DMA_MBURST 24
#define LOW_BIT_DMA_CHSEL 25
#define HIGH_BIT_DMA_CHSEL 27

// DMA stream number of data register

#define DMA_SNDTR(dma,stream) DMA_STREAM_REGISTER_AT(dma,stream,0x14)
#define LOW_BIT_DMA_SNDTR 0
#define HIGH_BIT_DMA_SNDTR 15

// DMA stream peripheral address register

#define DMA_SPAR(dma,stream) DMA_STREAM_REGISTER_AT(dma,stream,0x18)

// DMA stream memory 0 address register

#define DMA_SMA0R(dma,stream) DMA_STREAM_REGISTER_AT(dma,stream,0x1C)

#endif
