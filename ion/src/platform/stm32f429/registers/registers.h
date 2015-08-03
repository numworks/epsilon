#include <stdint.h>

/* Example usage:
 *
 * REGISTER_SET_VALUE(RCC_CR, HSITRIM, 37);
 * RCC_CR = (HSION|HSITRIM_VALUE(15));
 * if (RCC_CR & HSIRDY) { }
 */

#define BIT_MASK(high, low) ((((uint32_t)1<<((high)-(low)+1))-1)<<(low))
#define BIT_VALUE(value, high, low) (((value)<<(low))&BIT_MASK(high, low))

#include "rcc.h"
#include "gpio.h"
#include "spi.h"
#include "ltdc.h"

#define REGISTER_FIELD_MASK(field) (BIT_MASK(HIGH_BIT_##field,LOW_BIT_##field))
#define REGISTER_FIELD_VALUE(field, value) (BIT_VALUE(value,HIGH_BIT_##field,LOW_BIT_##field))

#define REGISTER_SET_VALUE(reg, field, value) (reg = ((reg&(~REGISTER_FIELD_MASK(field)))|REGISTER_FIELD_VALUE(field,value)))
//#define SET(reg, field) (reg |= field)
//#define CLEAR(reg, field) (reg &= ~field)
