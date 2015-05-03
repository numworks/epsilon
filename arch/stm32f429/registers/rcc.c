// The non-inclusion of rcc.h here is voluntary.
// Since we didn't define a type for RCC_AHB1ENR, we cannot implement it here
// If we don't include rcc.h, we declare a "new" symbol which will happen to match...
//#include <registers/rcc.h>

#define RCC_BASE 0x40023800
void * RCC_AHB1ENR = (void *)(RCC_BASE + 0x30);
