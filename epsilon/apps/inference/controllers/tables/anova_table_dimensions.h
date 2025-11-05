#pragma once

#include <algorithm>

#include "categorical_table_view_data_source.h"
#include "inference/models/anova_test.h"
#include "table_dimensions.h"

namespace Inference {

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

constexpr static int k_resultTitleColumnWidth = 200;

constexpr static TableShape k_resultBetweenWithinShape =
    TableShape{.inner = InnerTableDimensions{.rows = 3, .columns = 2},
               .reusable = ReusableCountsOneRowOneColumnHeader(
                   /* The "reusable" dimension is the same as the "normal"
                      dimension because the dimension is small.*/
                   ReusableCellDimensions{.rows = 3, .columns = 2})};

}  // namespace ANOVATableDimensions

}  // namespace Inference
