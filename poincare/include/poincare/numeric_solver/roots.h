#pragma once

#include <poincare/analysis_expression.h>
#include <poincare/system_expression.h>

namespace Poincare::Roots {

SystemExpression Linear(const AnalysisExpression& a,
                        const AnalysisExpression& b);

SystemExpression Quadratic(const AnalysisExpression& a,
                           const AnalysisExpression& b,
                           const AnalysisExpression& c);

}  // namespace Poincare::Roots
