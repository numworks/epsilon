#ifndef POINCARE_EXPRESSION_DEGREE_H
#define POINCARE_EXPRESSION_DEGREE_H

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

 private:
  static int PrivateGet(const Tree* e, const Tree* symbol);
};

}  // namespace Poincare::Internal

#endif
