#pragma once

#include <apps/shared/table_dimensions.h>

#include <algorithm>

#include "inference/controllers/tables/categorical_table_view_data_source.h"
#include "inference/controllers/tables/table_dimensions.h"
#include "inference/models/anova_test.h"

namespace Inference::ANOVA::TableDimensions {

// TODO: shared value with ChiSquare / Homogeneity
constexpr static int k_columnWidth = 82;

constexpr static int k_resultTitleColumnWidth =
    Shared::TableDimensions::k_rowTitleCellColumnWidth;

constexpr static int k_symbolColumnWidth =
    Shared::TableDimensions::k_symbolColumnWidth;

constexpr static TableShape k_inputDataShape = TableShape{
    .inner = InnerTableDimensions{.rows = ANOVATest::k_maxNumberOfRows,
                                  .columns = ANOVATest::k_maxNumberOfColumns},
    .reusable = ReusableCountsOneColumnHeader(ReusableCellDimensions{
        .rows =
            std::min(CategoricalTableViewDataSource::k_maxNumberOfReusableRows,
                     ANOVATest::k_maxNumberOfRows),
        .columns = std::min(Ion::Display::Width / k_columnWidth + 2,
                            ANOVATest::k_maxNumberOfColumns)})};

constexpr static TableShape k_inputStatisticsShape = TableShape{
    .inner = InnerTableDimensions{.rows = 3,
                                  .columns = ANOVATest::k_maxNumberOfColumns},
    .reusable = ReusableCountsTwoColumnHeadersOneRowHeader(
        /* The "reusable" number of rows are the same as the "normal" number of
           rows because the dimension is small.*/
        ReusableCellDimensions{
            .rows = 3,
            .columns = std::min(Ion::Display::Width / k_columnWidth + 3,
                                ANOVATest::k_maxNumberOfColumns)})};

constexpr static TableShape k_resultBetweenWithinShape =
    TableShape{.inner = InnerTableDimensions{.rows = 3, .columns = 2},
               .reusable = ReusableCountsOneRowOneColumnHeader(
                   /* The "reusable" dimension is the same as the "normal"
                      dimension because the dimension is small.*/
                   ReusableCellDimensions{.rows = 3, .columns = 2})};

constexpr static TableShape k_resultGroupStatisticsShape = TableShape{
    .inner = InnerTableDimensions{.rows = 4,
                                  .columns = ANOVATest::k_maxNumberOfColumns},
    .reusable = ReusableCountsTwoColumnHeadersOneRowHeader(
        /* The "reusable" number of rows are the same as the "normal" number of
           rows because the dimension is small.*/
        ReusableCellDimensions{
            .rows = 4,
            .columns = std::min(Ion::Display::Width / k_columnWidth + 2,
                                ANOVATest::k_maxNumberOfColumns)})};

}  // namespace Inference::ANOVA::TableDimensions
