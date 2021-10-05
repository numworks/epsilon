#ifndef POINCARE_PRINT_H
#define POINCARE_PRINT_H

#include <stddef.h>

namespace Poincare {
namespace Print {

/* Custom printf which supports the following formats:
 * - %c for character insertion
 * - %s for string insertion
 * - %i for integer insertion
 * - %*.*ef or %*.*ed for respectively float or double insertion.
 * This last format uses the syntax exemplified below:
 * Ion::customPrintf("A float %*.*ef!", Preferences::PrintFloatMode::Decimal, 7, 0.0123456789f) --> "A float 0.01234568!"
 * Ion::customPrintf("A double %*.*ed!", Preferences::PrintFloatMode::Scientific, 4, 0.0123456789) --> "A double 1.235E-2!"
 */

int customPrintf(char * buffer, size_t bufferSize, const char * format, ...);

}
}

#endif
