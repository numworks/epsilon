#ifndef UTILS_PRINT_H
#define UTILS_PRINT_H

#include <omg/bit_helper.h>
#include <omg/enums.h>
#include <stdint.h>

namespace OMG {

/* TODO:
 * - merge Poincare::PrintInt into OMG::UInt32(Base::Decimal),
 * - move Poincare::PrintFloat here
 * - move Poincare::Print here
 */

namespace Print {

inline constexpr bool IsLowercaseLetter(char c) { return 'a' <= c && c <= 'z'; }

inline constexpr bool IsDigit(char c) { return '0' <= c && c <= '9'; }

}  // namespace Print

}  // namespace OMG

#endif
