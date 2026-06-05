#pragma once

#include <poincare/float_precision.h>
#include <poincare/src/memory/tree_ref.h>

#include <complex>

#include "integer.h"
#include "projection.h"

namespace Poincare::Internal {

namespace Beautification {
SmallerFloat DegreeForSortingAddition(const Tree* e, bool symbolsOnly);
bool DeepBeautify(Tree* e,
                  const BeautificationParameters& beautificationParameters,
                  const Dimension& dimension = Dimension());

/* Create a Tree to represent a complex value according to the format, for
 * instance 0+1*i => <Constant i> in Cartesian mode. */
template <typename T>
Tree* PushBeautifiedComplex(std::complex<T> value, ComplexFormat complexFormat);
}  // namespace Beautification

}  // namespace Poincare::Internal
