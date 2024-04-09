#ifndef POINCARE_LAYOUT_CONVERSION_H
#define POINCARE_LAYOUT_CONVERSION_H

#include <poincare/old/layout.h>
#include <poincare/src/memory/tree.h>

namespace Poincare::Internal {

/* Temporary bi-directional conversions between old and new poincare to smooth
 * transition. */

Poincare::OLayout ToPoincareLayout(const Tree* exp);
Tree* FromPoincareLayout(Poincare::OLayout exp);

}  // namespace Poincare::Internal

#endif
