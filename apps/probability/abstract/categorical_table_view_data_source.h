#ifndef PROBABILITY_ABSTRACT_CATEGORICAL_TABLE_VIEW_DATA_SOURCE_H
#define PROBABILITY_ABSTRACT_CATEGORICAL_TABLE_VIEW_DATA_SOURCE_H

#include <escher/even_odd_cell.h>
#include "probability/abstract/dynamic_size_table_view_data_source.h"
#include "probability/gui/bordered_table_view_data_source.h"
#include "probability/models/statistic/goodness_test.h"
#include "probability/text_helpers.h"

namespace Probability {

class CategoricalTableViewDataSource : public BorderedTableViewDataSource {
public:
  // TableViewDataSource
  template <typename TextHolder> void willDisplayValueCellAtLocation(TextHolder * textHolder, Escher::EvenOddCell * evenOddCell, int column, int row, Chi2Test * test) {
    double p = test->parameterAtPosition(row, column);
    PrintValueInTextHolder(p, textHolder);
    evenOddCell->setEven(row % 2 == 0);
  }
  KDCoordinate rowHeight(int j) override { return k_rowHeight; }

  // BorderedTableViewDataSource
  KDCoordinate verticalBorderWidth() override { return k_borderBetweenColumns; }

  constexpr static int k_rowHeight = 20;
  constexpr static int k_maxNumberOfReusableRows = (Ion::Display::Height - Escher::Metric::TitleBarHeight - Escher::Metric::StackTitleHeight) / k_rowHeight + 2;
  constexpr static int k_typeOfInnerCells = 0;
protected:
  constexpr static int k_typeOfHeaderCells = k_typeOfInnerCells + 1;

  constexpr static int k_borderBetweenColumns = 1;
  constexpr static int k_columnWidth = (Ion::Display::Width - 2 * Escher::Metric::CommonLeftMargin - k_borderBetweenColumns) / 2;
};

}  // namespace Probability

#endif /* PROBABILITY_GUI_CATEGORICAL_TABLE_CELL_H */

