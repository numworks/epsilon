#ifndef POINCARE_EXPRESSION_BEAUTIFICATION_H
#define POINCARE_EXPRESSION_BEAUTIFICATION_H

#include <poincare_junior/src/memory/edition_reference.h>

#include "projection.h"

namespace PoincareJ {

class Beautification {
 public:
  static float DegreeForSortingAddition(const Tree *expr, bool symbolsOnly);

  static bool ShallowBeautify(Tree *node, void *context = nullptr);
  EDITION_REF_WRAP_1D(ShallowBeautify, void *, nullptr);
  static bool DeepBeautify(Tree *node,
                           ProjectionContext projectionContext = {});
  EDITION_REF_WRAP_1D(DeepBeautify, ProjectionContext, {});
};

}  // namespace PoincareJ

#endif
