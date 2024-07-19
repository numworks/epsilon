#ifndef POINCARE_EXPRESSION_DEGREE_H
#define POINCARE_EXPRESSION_DEGREE_H

#include <poincare/src/memory/tree.h>

#include "projection.h"

namespace Poincare::Internal {

class Degree {
 public:
  // Return polynomial degree of any un-projected expression on given symbol.
  static int Get(const Tree* e, const Tree* symbol,
                 ProjectionContext projectionContext);
  static int Get(const Tree* e, const char* symbolName,
                 ProjectionContext projectionContext);
  constexpr static int k_unknown = -1;

 private:
  static int PrivateGet(const Tree* e, const Tree* symbol);
};

}  // namespace Poincare::Internal

#endif
