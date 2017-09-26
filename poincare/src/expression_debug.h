#ifndef POICARE_EXPRESSION_DEBUG_H
#define POICARE_EXPRESSION_DEBUG_H

#include <poincare/expression.h>
#include <poincare/integer.h>

namespace Poincare {

void print_expression(const Expression * e, int indentationLevel = 0);
void print_prime_factorization(Integer * outputFactors, Integer * outputCoefficients, int outputLength);

}

#endif // POICARE_EXPRESSION_DEBUG_H
