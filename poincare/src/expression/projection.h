#pragma once

#include <poincare/math_options.h>
#include <poincare/preferences.h>
#include <poincare/src/memory/tree_ref.h>
#include <poincare/symbol_context.h>

#include "dimension.h"

namespace Poincare::Internal {

struct ProjectionContext {
  ComplexFormat m_complexFormat = ComplexFormat::Real;
  AngleUnit m_angleUnit = AngleUnit::Radian;
  Strategy m_strategy = Strategy::Default;
  ReductionTarget m_reductionTarget = ReductionTarget::User;
  Dimension m_dimension = Dimension();
  Preferences::UnitFormat m_unitFormat = Preferences::UnitFormat::Metric;
  SymbolicComputation m_symbolic = SymbolicComputation::KeepAllSymbols;
  const Poincare::SymbolContext& m_context = k_emptySymbolContext;
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
        .m_unitFormat = Preferences::UnitFormat::Metric,
        .m_symbolic = SymbolicComputation::ReplaceDefinedSymbols,
        .m_context = k_emptySymbolContext};
    return projCtx;
  }
  /* Update complexFormat if tree contains i, Re, Im, Arg or Conj. Return true
   * if updated. */
  static bool UpdateComplexFormatWithExpressionInput(
      const Tree* e, ProjectionContext* projectionContext);
  /* User variables are fetched from the Context and replaced according to the
  SymbolicComputation strategy */
  static bool DeepReplaceUserNamed(Tree* e,
                                   const Poincare::SymbolContext& symbolContext,
                                   SymbolicComputation symbolic);
  // All user variables are replaced with Undefined
  static bool DeepReplaceUserNamedWithUndefined(Tree* e);
  static bool DeepSystemProject(Tree* e, ProjectionContext ctx = {},
                                const Dimension& dimension = Dimension());

  /* Some projections are performed during advanced reduction instead so the
   * metric can cancel it if unecessary. */
  static bool Expand(Tree* e);

  /* Return true if node simplification and display is forbidden by current
   * preferences. */
  static bool IsForbidden(const Tree* e);
  // Return true if one of its descendants is forbidden by current preferences.
  static bool HasForbiddenDescendants(const Tree* e);

 private:
  static bool ShallowReplaceUserNamed(
      Tree* e, const Poincare::SymbolContext& symbolContext,
      SymbolicComputation symbolic);
  static bool ShallowSystemProject(Tree* e, void* ctx);
};

}  // namespace Poincare::Internal
