#ifndef POINCARE_EXPRESSION_BEAUTIFICATION_H
#define POINCARE_EXPRESSION_BEAUTIFICATION_H

#include <poincare/src/memory/tree_ref.h>

#include "context.h"
#include "projection.h"

namespace Poincare::Internal {

namespace Beautification {
float DegreeForSortingAddition(const Tree* e, bool symbolsOnly);
bool AddUnits(Tree* e, ProjectionContext projectionContext);
bool DeepBeautify(Tree* e, ProjectionContext projectionContext = {});

/* Create a Tree to represent a complex value according to the format, for
 * instance 0+1*i => <Constant i> in Cartesian mode. */
template <typename T>
Tree* PushBeautifiedComplex(std::complex<T> value, ComplexFormat complexFormat);
}  // namespace Beautification

}  // namespace Poincare::Internal

#endif
