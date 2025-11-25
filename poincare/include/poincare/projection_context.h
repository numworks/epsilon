#pragma once

#include "math_options.h"
#include "preferences.h"
#include "symbol_context.h"

namespace Poincare {

struct ProjectionContext {
#if POINCARE_COMPLEX
  ComplexFormat m_complexFormat = ComplexFormat::Real;
#else
  constexpr static ComplexFormat m_complexFormat = ComplexFormat::Real;
#endif
  AngleUnit m_angleUnit = AngleUnit::Radian;
  Strategy m_strategy = Strategy::Default;
  ReductionTarget m_reductionTarget = ReductionTarget::User;
  Preferences::UnitFormat m_unitFormat = Preferences::UnitFormat::Metric;
  SymbolicComputation m_symbolic = SymbolicComputation::KeepAllSymbols;
  const Poincare::SymbolContext& m_context = k_emptySymbolContext;
  UnitDisplay m_unitDisplay = UnitDisplay::MainOutput;
  // Optional simplification step
  bool m_advanceReduce = true;
};

}  // namespace Poincare
