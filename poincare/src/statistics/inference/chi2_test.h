#pragma once

#include <poincare/statistics/data_table.h>

namespace Poincare::Internal::Inference::SignificanceTest::Chi2 {

double ComputeCriticalValue(const DataTable* contributions);

}  // namespace Poincare::Internal::Inference::SignificanceTest::Chi2
