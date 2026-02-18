#pragma once

#include <poincare/src/memory/tree.h>

#include "projection.h"

namespace Poincare::Internal {

namespace Simplification {
bool Simplify(Tree* e, const SimplificationParameters& simplificationParameters,
              bool beautify = true);

// Simplification steps
Dimension ProjectAndReduce(
    Tree* e, const SimplificationParameters& simplificationParameters);
bool BeautifyReduced(Tree* e,
                     const SimplificationParameters& simplificationParameters,
                     const Dimension& dimension);
bool PrepareForProjection(
    Tree* e, const SimplificationParameters& simplificationParameters);

struct ToSystemOutput {
  bool changed;
  Dimension dimension;
};
ToSystemOutput ToSystem(
    Tree* e, const SimplificationParameters& simplificationParameters);
#if ASSERTIONS
bool IsSystem(const Tree* e);
#endif
bool ReduceSystem(Tree* e, bool advanced,
                  ReductionTarget reductionTarget = ReductionTarget::User);
}  // namespace Simplification

}  // namespace Poincare::Internal
