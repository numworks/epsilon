#ifndef POINCARE_EXPRESSION_DEGREE_H
#define POINCARE_EXPRESSION_DEGREE_H

#include <limits.h>
#include <poincare/src/memory/tree.h>

#include "projection.h"

namespace Poincare::Internal {

class Degree {
 public:
  // Return polynomial degree of any projected expression on given symbol.
  /* k_unknown is returned if the degree could not be determined.
   * k_maxPolynomialDegree is returned if the degree is higher than
   * k_maxPolynomialDegree. Note that only small polynomials degrees need to be
   * considered for the current use cases (function properties, solver). */
  static int Get(const Tree* e, const Tree* symbol);
  static int Get(const Tree* e, const char* symbolName);
  constexpr static int k_unknown = -1;
  constexpr static int k_maxPolynomialDegree = 256;
  /* When computing the degree of an expression, the case that could lead to the
   * worst integer overflow is when computing (x^n)^m. All intermediate degree
   * calculations need to return a value for which n*m will be representable by
   * the "int" type.  */
  static_assert(k_maxPolynomialDegree < INT_MAX / k_maxPolynomialDegree);

 private:
  static int PrivateGet(const Tree* e, const Tree* symbol);
};

}  // namespace Poincare::Internal

#endif
