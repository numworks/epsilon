#ifndef POINCARE_FUNCTION_PROPERTIES_HELPER_H
#define POINCARE_FUNCTION_PROPERTIES_HELPER_H

#include <poincare/src/expression/projection.h>
#include <poincare/src/memory/tree.h>

namespace Poincare::Internal {

bool IsFractionOfPolynomials(const Tree* e, const char* symbol,
                             ProjectionContext projectionContext);

void RemoveConstantTermsInAddition(Tree* e, const char* symbol,
                                   ProjectionContext projectionContext);

typedef bool (*PatternTest)(const Tree*, const char*, ProjectionContext);
bool IsLinearCombinationOfFunction(const Tree* e, const char* symbol,
                                   ProjectionContext projectionContext,
                                   PatternTest testFunction);

/* Detect if expression is of the form a·cos(b·x+c) + k
 * with c between 0 and 2π
 * k can be non-null only if acceptAddition = true
 * TODO: factorize with IsLinearCombinationOfFunction? */
bool DetectLinearPatternOfTrig(const Tree* e,
                               ProjectionContext projectionContext,
                               const char* symbol, double* a, double* b,
                               double* c, bool acceptConstantTerm);

}  // namespace Poincare::Internal

#endif
