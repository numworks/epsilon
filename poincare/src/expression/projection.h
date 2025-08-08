#ifndef POINCARE_EXPRESSION_PROJECTION_H
#define POINCARE_EXPRESSION_PROJECTION_H

#include <poincare/old/context.h>
#include <poincare/old/empty_context.h>
#include <poincare/src/memory/tree_ref.h>

#include "context.h"
#include "dimension.h"

namespace Poincare::Internal {

struct ProjectionContext {
  ComplexFormat m_complexFormat = ComplexFormat::Real;
  AngleUnit m_angleUnit = AngleUnit::Radian;
  Strategy m_strategy = Strategy::Default;
  ReductionTarget m_reductionTarget = ReductionTarget::User;
  Dimension m_dimension = Dimension();
  UnitFormat m_unitFormat = UnitFormat::Metric;
  SymbolicComputation m_symbolic = SymbolicComputation::KeepAllSymbols;
  const Poincare::Context& m_context = k_emptyContext;
  UnitDisplay m_unitDisplay = UnitDisplay::MainOutput;
  // Optional simplification step
  bool m_advanceReduce = true;
};

class Projection {
 public:
  static ProjectionContext DefaultProjectionContextForAnalysis() {
    ProjectionContext projCtx{
        .m_complexFormat = ComplexFormat::Cartesian,
        .m_angleUnit = AngleUnit::Radian,
        .m_unitFormat = UnitFormat::Metric,
        .m_symbolic = SymbolicComputation::ReplaceDefinedSymbols,
        .m_context = k_emptyContext};
    return projCtx;
  }
  /* Update complexFormat if tree contains i, Re, Im, Arg or Conj. Return true
   * if updated. */
  static bool UpdateComplexFormatWithExpressionInput(
      const Tree* e, ProjectionContext* projectionContext);
  /* User variables are fetched from the Context and replaced according to the
  SymbolicComputation strategy */
  static bool DeepReplaceUserNamed(Tree* e, Poincare::Context* context,
                                   SymbolicComputation symbolic);
  // All user variables are replaced with Undefined
  static bool DeepReplaceUserNamedWithUndefined(Tree* e);
  static bool DeepSystemProject(Tree* e, ProjectionContext ctx = {});

  /* Some projections are performed during advanced reduction instead so the
   * metric can cancel it if unecessary. */
  static bool Expand(Tree* e);

  /* Return true if node simplification and display is forbidden by current
   * preferences. */
  static bool IsForbidden(const Tree* e);
  // Return true if one of its descendants is forbidden by current preferences.
  static bool HasForbiddenDescendants(const Tree* e);

 private:
  static bool ShallowReplaceUserNamed(Tree* e, const Poincare::Context& context,
                                      SymbolicComputation symbolic);
  static bool ShallowSystemProject(Tree* e, void* ctx);
};

}  // namespace Poincare::Internal

#endif
