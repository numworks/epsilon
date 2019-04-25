#ifndef POINCARE_PRINT_INT_H
#define POINCARE_PRINT_INT_H

#include <stdint.h>

namespace Poincare {

namespace PrintInt {

bool PadIntInBuffer(uint32_t integer, char * buffer, int bufferLength);

}

}

#endif
