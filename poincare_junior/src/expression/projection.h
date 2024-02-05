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
  UnitFormat m_unitFormat = UnitFormat::Metric;
};

class Projection {
 public:
  static ProjectionContext ContextFromSettings();
  static bool DeepSystemProject(Tree *reference,
                                ProjectionContext projectionContext = {});
  EDITION_REF_WRAP_1D(DeepSystemProject, ProjectionContext, {});

  static bool ShallowSystemProject(Tree *reference,
                                   void *projectionContext = nullptr);
  EDITION_REF_WRAP_1D(ShallowSystemProject, void *, nullptr);

  /* Some projections are performed during advanced reduction instead so the
   * metric can cancel it if unecessary. */
  static bool Expand(Tree *tree);
};

}  // namespace PoincareJ

#endif
