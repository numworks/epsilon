#pragma once

#include <algorithm>

#include "categorical_table_view_data_source.h"
#include "inference/models/anova_test.h"

namespace Inference {

namespace ANOVATableDimensions {
// TODO: shared value with ChiSquare / Homogeneity
constexpr static int k_columnWidth = 82;
constexpr static int k_numberOfReusableColumns = std::min(
    Ion::Display::Width / k_columnWidth + 2, ANOVATest::k_maxNumberOfColumns);
constexpr static int k_maxNumberOfReusableRows =
    std::min(CategoricalTableViewDataSource::k_maxNumberOfReusableRows,
             ANOVATest::k_maxNumberOfRows);
constexpr static int k_numberOfInnerReusableCells =
    k_maxNumberOfReusableRows * k_numberOfReusableColumns;
constexpr static int k_numberOfInputHeaderReusableCells =
    k_numberOfReusableColumns;
constexpr static int k_numberOfResultRows = 3;
constexpr static int k_numberOfResultsHeaderReusableCells =
    k_numberOfReusableColumns + k_numberOfResultRows + 1;
}  // namespace ANOVATableDimensions

}  // namespace Inference
