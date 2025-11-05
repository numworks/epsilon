#pragma once

#include <apps/i18n.h>
#include <escher/even_odd_buffer_text_cell.h>
#include <escher/solid_color_cell.h>

#include <algorithm>

#include "inference/controllers/tables/categorical_table_view_data_source.h"
#include "inference/controllers/tables/one_header_column_one_header_row_table_data_source.h"
#include "inference/controllers/tables/table_dimensions.h"
#include "inference/models/homogeneity_test.h"

namespace Inference {

namespace HomogeneityTableDimensions {
constexpr static int k_columnWidth = 82;
// 5 - we take into account the potential "Total" column
constexpr static int k_numberOfReusableColumns =
    std::min(Ion::Display::Width / k_columnWidth + 2,
             HomogeneityTest::k_maxNumberOfColumns + 1);
// std::min(12, 9 + 1) - we take into account the potential "Total" row
constexpr static int k_maxNumberOfReusableRows =
    std::min(CategoricalTableViewDataSource::k_maxNumberOfReusableRows,
             HomogeneityTest::k_maxNumberOfRows + 1);
// 5 * 10
constexpr static int k_numberOfInnerReusableCells =
    k_maxNumberOfReusableRows * k_numberOfReusableColumns;

// -1 takes the hidden top left cell into account
// 5 + 10 - 1
constexpr static int k_numberOfHeaderReusableCells =
    k_numberOfReusableColumns + k_maxNumberOfReusableRows - 1;
}  // namespace HomogeneityTableDimensions

/* This class specializes OneHeaderColumnOneHeaderRowTableDataSource with the
 * header data (group names) and dimensions for InputHomogeneity and
 * HomogeneityResults. */
class HomogeneityTableDataSource
    : public OneHeaderColumnOneHeaderRowTableDataSource {
 public:
  // TableViewDataSource

  void fillCellForLocation(Escher::HighlightCell* cell, int column,
                           int row) override;

  constexpr static int k_columnWidth =
      HomogeneityTableDimensions::k_columnWidth;
  constexpr static int k_numberOfReusableColumns =
      HomogeneityTableDimensions::k_numberOfReusableColumns;
  constexpr static int k_maxNumberOfReusableRows =
      HomogeneityTableDimensions::k_maxNumberOfReusableRows;

 protected:
  constexpr static int k_maxNumberOfColumns =
      HomogeneityTest::k_maxNumberOfColumns;
  constexpr static int k_maxNumberOfRows = HomogeneityTest::k_maxNumberOfRows;

  KDCoordinate nonMemoizedColumnWidth(int column) override {
    return k_columnWidth;
  }
  virtual void fillInnerCellForLocation(Escher::HighlightCell* cell, int column,
                                        int row) = 0;

  ReusableCellCounts reusableCellCounts() const override {
    return ReusableCountsOneRowOneColumnHeader(
        ReusableCellDimensions{.rows = k_maxNumberOfReusableRows,
                               .columns = k_numberOfReusableColumns});
  };

 private:
  constexpr static int k_typeOfTopLeftCell = k_typeOfHeaderCells + 1;
  constexpr static int k_headerTranslationBufferSize = 20;
};

}  // namespace Inference
