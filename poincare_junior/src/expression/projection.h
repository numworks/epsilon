#ifndef POINCARE_EXPRESSION_PROJECTION_H
#define POINCARE_EXPRESSION_PROJECTION_H

#include <poincare/context.h>
#include <poincare_junior/src/expression/dimension.h>
#include <poincare_junior/src/memory/tree_ref.h>

#include "context.h"

namespace PoincareJ {

struct ProjectionContext {
  ComplexFormat m_complexFormat = ComplexFormat::Real;
  AngleUnit m_angleUnit = AngleUnit::Radian;
  Strategy m_strategy = Strategy::Default;
  Dimension m_dimension = Dimension();
  UnitFormat m_unitFormat = UnitFormat::Metric;
  SymbolicComputation m_symbolic = SymbolicComputation::DoNotReplaceAnySymbol;
  Poincare::Context *m_context = nullptr;
};

class Projection {
 public:
  static ProjectionContext ContextFromSettings();
  static bool DeepReplaceUserNamed(Tree *tree, ProjectionContext ctx);
  static bool DeepSystemProject(Tree *reference, ProjectionContext ctx = {});
  EDITION_REF_WRAP_1D(DeepSystemProject, ProjectionContext, {});

  /* Some projections are performed during advanced reduction instead so the
   * metric can cancel it if unecessary. */
  static bool Expand(Tree *tree);

 private:
  static bool ShallowReplaceUserNamed(Tree *reference, void *ctx);
  static bool ShallowSystemProject(Tree *reference, void *ctx);
};

}  // namespace PoincareJ

#endif
