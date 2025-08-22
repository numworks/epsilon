#pragma once

#include <poincare/src/memory/tree.h>

namespace Poincare::Internal {

/* Take a beautified multiplication and return the appropriate operator symbol
 * between "×", "·" and nothing according to the shape of the children. */
CodePoint MultiplicationSymbol(const Tree* mult, bool linearMode);

}  // namespace Poincare::Internal
