#ifndef POINCARE_JUNIOR_CONVERSION_H
#define POINCARE_JUNIOR_CONVERSION_H

#include <poincare/layout.h>
#include <poincare_junior/src/memory/tree.h>

namespace PoincareJ {

/* Temporary bi-directional conversions between old and new poincare to smooth
 * transition. */

Poincare::OLayout ToPoincareLayout(const Tree* exp);
Tree* FromPoincareLayout(Poincare::OLayout exp);

}  // namespace PoincareJ

#endif
