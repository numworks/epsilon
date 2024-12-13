#ifndef POINCARE_EXPRESSION_PROJECTION_H
#define POINCARE_EXPRESSION_PROJECTION_H

#include <poincare/old/context.h>
#include <poincare/src/memory/tree_ref.h>

#include "context.h"
#include "dimension.h"

namespace Poincare::Internal {

struct ProjectionContext {
  ComplexFormat m_complexFormat = ComplexFormat::Real;
  AngleUnit m_angleUnit = AngleUnit::Radian;
  Strategy m_strategy = Strategy::Default;
  ExpansionStrategy m_expansionStrategy = ExpansionStrategy::None;
  /* TODO_PCJ: Default to a None DimensionType to ensure we don't mistake it
   * with a memoized value. */
  Dimension m_dimension = Dimension();
  UnitFormat m_unitFormat = UnitFormat::Metric;
  SymbolicComputation m_symbolic = SymbolicComputation::KeepAllSymbols;
  Poincare::Context* m_context = nullptr;
  UnitDisplay m_unitDisplay = UnitDisplay::MainOutput;
  // Optional simplification step
  bool m_advanceReduce = true;
};

class Projection {
 public:
  /* Update complexFormat if tree contains i, Re, Im, Arg or Conj. Return true
   * if updated. */
  static bool UpdateComplexFormatWithExpressionInput(
      const Tree* e, ProjectionContext* projectionContext);
  static bool DeepReplaceUserNamed(Tree* e, Poincare::Context* context,
                                   SymbolicComputation symbolic);
  static bool DeepSystemProject(Tree* e, ProjectionContext ctx = {});

  /* Some projections are performed during advanced reduction instead so the
   * metric can cancel it if unecessary. */
  static bool Expand(Tree* e);

  /* Return true if node simplification and display is forbidden by current
   * preferences. */
  static bool IsForbidden(const Tree* e);

 private:
  static bool ShallowReplaceUserNamed(Tree* e, Poincare::Context* context,
                                      SymbolicComputation symbolic);
  static bool ShallowSystemProject(Tree* e, void* ctx);
};

}  // namespace Poincare::Internal

#endif
