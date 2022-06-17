#ifndef POINCARE_PRINT_H
#define POINCARE_PRINT_H

#include <stddef.h>

namespace Poincare {
namespace Print {

/* Custom printf which supports the following formats:
 * - %c for character insertion
 * - %s for string insertion
 * - %Cs for string insertion capitalizing the first character
 * - %cs for string insertion decapitalizing the first character
 *   ex:
 *     customPrintf("A capitalized string: %Cs", "plouf")
 *       --> "A capitalized string: Plouf"
 *     customPrintf("A decapitalized string: %cs", "Plouf")
 *       --> "A decapitalized string: plouf"
 * - %i for integer insertion
 * - %*.*ef or %*.*ed for respectively float or double insertion.
 *   ex:
 *     customPrintf("A float %*.*ef!",
 *                  0.0123456789f,
 *                  Preferences::PrintFloatMode::Decimal,
 *                  7)
 *       --> "A float 0.01234568!"
 *     customPrintf("A double %*.*ed!",
 *                   0.0123456789,
 *                   Preferences::PrintFloatMode::Scientific,
 *                   4)
 *       --> "A double 1.235E-2!"
 * - %+i or %+*.*ef inserts the plus sign if the value is positive or null
 *
 * In contrast to safeCustomPrintf, customPrintf method asserts that the
 * bufferSize is large enough. If the text does not fit, the buffer is erased
 * and a length equal or bigger than bufferSize is returned.
 */

int customPrintf(char * buffer, size_t bufferSize, const char * format, ...);
int safeCustomPrintf(char * buffer, size_t bufferSize, const char * format, ...);
void capitalize(char * text);
void decapitalize(char * text);

}
}

#endif
