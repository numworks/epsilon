#pragma once

#include <poincare/statistics/data_table.h>

namespace Poincare::Internal::Inference::SignificanceTest {

namespace Chi2 {

double ComputeCriticalValue(const DataTable* contributions);

}  // namespace Chi2

}  // namespace Poincare::Internal::Inference::SignificanceTest
