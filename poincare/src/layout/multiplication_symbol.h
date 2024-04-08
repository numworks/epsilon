#ifndef POINCARE_LAYOUT_MULTIPLICATION_SYMBOL_H
#define POINCARE_LAYOUT_MULTIPLICATION_SYMBOL_H

#include <poincare/src/memory/tree.h>

namespace PoincareJ {

/* Take a beautified multiplication and return the appropriate operator symbol
 * between "×", "·" and nothing according to the shape of the children. */
CodePoint MultiplicationSymbol(const Tree* mult);

}  // namespace PoincareJ

#endif
