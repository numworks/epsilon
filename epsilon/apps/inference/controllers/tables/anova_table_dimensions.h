#pragma once

#include <algorithm>

#include "categorical_table_view_data_source.h"
#include "inference/models/anova_test.h"

namespace Inference {

/* Dimensions of "reusable cells", which are the number of cells that can appear
 * on screen at the same time. */
struct ReusableCellDimensions {
  int rows;
  int columns;
};

// Number of reusable cells of each type ("inner" or "header" cells)
struct ReusableCellCategories {
  int header;
  int inner;
};

struct ReusableCellCounts {
  ReusableCellDimensions dimensions;
  ReusableCellCategories categories;
};

// Dimensions of the "inner table", not counting row or column headers.
struct InnerTableDimensions {
  int rows;
  int columns;
};

struct TableShape {
  InnerTableDimensions inner;
  ReusableCellCounts reusable;
};

constexpr ReusableCellCounts ReusableCountsOneColumnHeader(
    ReusableCellDimensions dimensions) {
  return ReusableCellCounts{.dimensions = dimensions,
                            .categories = ReusableCellCategories{
                                .header = dimensions.columns,
                                .inner = dimensions.columns * dimensions.rows}};
}

namespace ANOVATableDimensions {
// TODO: shared value with ChiSquare / Homogeneity
constexpr static int k_columnWidth = 82;

constexpr static TableShape k_inputShape = TableShape{
    .inner = InnerTableDimensions{.rows = ANOVATest::k_maxNumberOfRows,
                                  .columns = ANOVATest::k_maxNumberOfColumns},
    .reusable = ReusableCountsOneColumnHeader(ReusableCellDimensions{
        .rows =
            std::min(CategoricalTableViewDataSource::k_maxNumberOfReusableRows,
                     ANOVATest::k_maxNumberOfRows),
        .columns = std::min(Ion::Display::Width / k_columnWidth + 2,
                            ANOVATest::k_maxNumberOfColumns)})};

constexpr static int k_numberOfResultInnerRows = 3;
constexpr static int k_numberOfResultInnerColumns = 2;
constexpr static int k_numberOfResultsInnerCells =
    k_numberOfResultInnerRows * k_numberOfResultInnerColumns;
constexpr static int k_numberOfResultsHeaderCells =
    k_numberOfResultInnerColumns + k_numberOfResultInnerRows;

constexpr static int k_resultTitleColumnWidth = 200;
}  // namespace ANOVATableDimensions

}  // namespace Inference
