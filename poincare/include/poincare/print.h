#ifndef POINCARE_PRINT_H
#define POINCARE_PRINT_H

#include <stddef.h>

namespace Poincare {
namespace Print {

/* Custom printf which supports the following formats:
 * - %c for character insertion
 * - %s for string insertion
 * - %*s for string insertion with string preprocessing
 *   ex:
 *     customPrintf("A capitalized string: %*s",
 *                  "plouf",
 *                   StringFormat::Capitalized)
 *       --> "A capitalized string: Plouf"
 *     customPrintf("A decapitalized string: %*s",
 *                  "Plouf",
 *                   StringFormat::Decapitalized)
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
 */

enum class StringFormat {
  Capitalized,
  Decapitalized
};

int customPrintf(char * buffer, size_t bufferSize, const char * format, ...);
void capitalize(char * text);
void decapitalize(char * text);

}
}

#endif
