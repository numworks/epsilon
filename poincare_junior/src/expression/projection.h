#ifndef POINCARE_EXPRESSION_PROJECTION_H
#define POINCARE_EXPRESSION_PROJECTION_H

#include <poincare_junior/src/expression/dimension.h>
#include <poincare_junior/src/memory/edition_reference.h>

#include "context.h"

namespace PoincareJ {

struct ProjectionContext {
  ComplexFormat m_complexFormat = ComplexFormat::Real;
  AngleUnit m_angleUnit = AngleUnit::Radian;
  Strategy m_strategy = Strategy::Default;
  Dimension m_dimension = Dimension();
};

class Projection {
 public:
  static bool DeepSystemProjection(Tree *reference,
                                   ProjectionContext projectionContext = {});
  EDITION_REF_WRAP_1D(DeepSystemProjection, ProjectionContext, {});

  static bool ShallowSystemProjection(Tree *reference,
                                      void *projectionContext = nullptr);
  EDITION_REF_WRAP_1D(ShallowSystemProjection, void *, nullptr);
};

}  // namespace PoincareJ

#endif
