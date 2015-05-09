#ifndef STM32F429_REGISTERS_SPI_H
#define STM32F429_REGISTERS_SPI_H 1

// Serial Peripheral Interface registers

#define SPI1_BASE 0x40013000
#define SPI2_BASE 0x40003800
#define SPI3_BASE 0x40003C00
#define SPI4_BASE 0x40013400
#define SPI5_BASE 0x40015000
#define SPI6_BASE 0x40015400

#define SPI_REGISTER_AT(spi_port,offset) (*(volatile uint16_t *)(spi_port##_BASE+offset))

// SPI control registers 1

#define SPI_CR1(spi_port) SPI_REGISTER_AT(spi_port, 0x00)

#define SPI_BR_DIV_2 0
#define SPI_BR_DIV_4 1
#define SPI_BR_DIV_8 2
#define SPI_BR_DIV_16 3
#define SPI_BR_DIV_32 4
#define SPI_BR_DIV_64 5
#define SPI_BR_DIV_128 6
#define SPI_BR_DIV_256 7

#define SPI_DFF_8_BITS 0
#define SPI_DFF_16_BITS 1

#define SPI_CPHA (1<<0)
#define SPI_CPOL (1<<1)
#define SPI_MSTR (1<<2)
#define LOW_BIT_SPI_BR 3
#define HIGH_BIT_SPI_BR 5
#define SPI_BR(v) REGISTER_FIELD_VALUE(SPI_BR, v)
#define SPI_SPE (1<<6)
#define SPI_LSBFIRST (1<<7)
#define SPI_SSI (1<<8)
#define SPI_SSM (1<<9)
#define SPI_RXONLY (1<<10)
#define SPI_DFF (1<<11)
#define SPI_CRCNEXT (1<<12)
#define SPI_CRCEN (1<<13)
#define SPI_BIDIOE (1<<14)
#define SPI_BIDIMODE (1<<15)

// SPI status registers

#define SPI_SR(spi_port) SPI_REGISTER_AT(spi_port, 0x08)

#define SPI_RXNE (1<<0)
#define SPI_TXE (1<<1)
#define SPI_CHSIDE (1<<2)
#define SPI_UDR (1<<3)
#define SPI_CRCERR (1<<4)
#define SPI_MODF (1<<5)
#define SPI_OVR (1<<6)
#define SPI_BSY (1<<7)
#define SPI_FRE (1<<8)

// SPI data registers

#define SPI_DR(spi_port) SPI_REGISTER_AT(spi_port, 0x0C)

#endif
