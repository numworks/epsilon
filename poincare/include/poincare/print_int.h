#ifndef POINCARE_PRINT_INT_H
#define POINCARE_PRINT_INT_H

#include <stdint.h>

namespace Poincare {

namespace PrintInt {

bool Left(uint32_t integer, char * buffer, int bufferLength);
bool Right(uint32_t integer, char * buffer, int bufferLength);

}

}

#endif
