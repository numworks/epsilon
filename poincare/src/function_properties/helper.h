#ifndef POINCARE_FUNCTION_PROPERTIES_HELPER_H
#define POINCARE_FUNCTION_PROPERTIES_HELPER_H

#include <poincare/old/junior_expression.h>

namespace Poincare {

double PositiveModulo(double i, double n);

void RemoveConstantTermsInAddition(Internal::Tree* e, const char* symbol);

bool DetectLinearPatternOfTrig(const Internal::Tree* e, const char* symbol,
                               double* a, double* b, double* c,
                               bool acceptConstantTerm);

}  // namespace Poincare

#endif
