#include <stdint.h>

// Bit manipulation helpers

// 32 bits value where bits between HIGH and LOW are 1 and others are 0
#define BIT_RANGE_MASK(high, low) ((((uint32_t)1<<((high)-(low)+1))-1)<<(low))

// 32 bits value where bits between HIGH and LOW are set to VALUE and others to 0
#define BIT_RANGE_VALUE(value, high, low) (((value)<<(low))&BIT_RANGE_MASK(high, low))

// Bit mask for a register field defined by LOW_BIT_FIELD and HIGH_BIT_FIELD
#define REGISTER_FIELD_MASK(field) (BIT_RANGE_MASK(HIGH_BIT_##field,LOW_BIT_##field))

// Value ready to be OR-ed to set a FIELD in a register
#define REGISTER_FIELD_VALUE(field, value) (BIT_RANGE_VALUE(value,HIGH_BIT_##field,LOW_BIT_##field))

// Set the value of a field defined by LOW_BIT_FIELD and HIGH_BIT_FIELD
#define REGISTER_SET_VALUE(reg, field, value) (reg = ((reg&(~REGISTER_FIELD_MASK(field)))|REGISTER_FIELD_VALUE(field,value)))

// Get the value of a FIELD defined by LOW_BIT_FIELD and HIGH_BIT_FIELD
#define REGISTER_GET_VALUE(reg, field) ((reg & REGISTER_FIELD_MASK(field) >> LOW_BIT_##field))

#include "rcc.h"
#include "gpio.h"
#include "spi.h"
#include "dma.h"
#include "cm4.h"
//#include "ltdc.h"
