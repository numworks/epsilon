#ifndef POINCARE_PRINT_INT_H
#define POINCARE_PRINT_INT_H

#include <stdint.h>

namespace Poincare {

namespace PrintInt {

/* PrintInt prints a uint32_t in a given buffer, without the null-terminating
 * char.
 *  - Left prints on the left of the buffer and returns the wanted length to
 *      print the integer.
 *  - Right prints on the right, padding on the left with zeroes if needed. It
 *      returns the number of chars written (bufferLength) */

int Left(uint32_t integer, char* buffer, int bufferLength);
int Right(uint32_t integer, char* buffer, int bufferLength);

}  // namespace PrintInt

}  // namespace Poincare

#endif
