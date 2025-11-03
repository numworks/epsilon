#pragma once

#include <algorithm>

#include "categorical_table_view_data_source.h"
#include "inference/models/anova_test.h"

namespace Inference {

namespace ANOVATableDimensions {
// TODO: shared value with ChiSquare / Homogeneity
constexpr static int k_columnWidth = 82;
constexpr static int k_numberOfInputColumns = std::min(
    Ion::Display::Width / k_columnWidth + 2, ANOVATest::k_maxNumberOfColumns);
constexpr static int k_numberOfInputInnerRows =
    std::min(CategoricalTableViewDataSource::k_maxNumberOfReusableRows,
             ANOVATest::k_maxNumberOfRows);
constexpr static int k_numberOfInputInnerCells =
    k_numberOfInputInnerRows * k_numberOfInputColumns;
constexpr static int k_numberOfInputHeaderCells = k_numberOfInputColumns;

constexpr static int k_numberOfResultInnerRows = 3;
constexpr static int k_numberOfResultInnerColumns = 2;
constexpr static int k_numberOfResultsInnerCells =
    k_numberOfResultInnerRows * k_numberOfResultInnerColumns;
constexpr static int k_numberOfResultsHeaderCells =
    k_numberOfResultInnerColumns + k_numberOfResultInnerRows;

constexpr static int k_resultTitleColumnWidth = 200;
}  // namespace ANOVATableDimensions

}  // namespace Inference
