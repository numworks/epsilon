#pragma once

#include <poincare/properties.h>
#include <poincare/src/memory/tree.h>

namespace Poincare::Internal {

Properties GetProperties(const Tree* e);
ComplexProperties GetComplexProperties(const Tree* e);

/* Properties of e1 - e2 so that e1 < e2 <=> ComplexPropertiesOfDifference(e1,
 * e2) < 0 and so on. Beware that the difference may be real while the trees
 * were complexes. */
ComplexProperties ComplexPropertiesOfDifference(const Tree* e1, const Tree* e2);

/* Return the Tree properties if they are known, otherwise return the properties
 * of the approximated value */
ComplexProperties PropertiesOfTreeOrApproximation(const Tree* e);

}  // namespace Poincare::Internal
