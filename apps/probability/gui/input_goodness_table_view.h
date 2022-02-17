#ifndef PROBABILITY_GUI_INPUT_GOODNESS_TABLE_VIEW_H
#define PROBABILITY_GUI_INPUT_GOODNESS_TABLE_VIEW_H

#include <escher/even_odd_editable_text_cell.h>
#include <escher/even_odd_message_text_cell.h>
#include <escher/highlight_cell.h>
#include <escher/metric.h>
#include <escher/selectable_table_view.h>

#include "bordered_table_view_data_source.h"
#include "probability/abstract/dynamic_cells_data_source.h"
#include "probability/abstract/dynamic_size_table_view_data_source.h"
#include "probability/abstract/homogeneity_data_source.h"
#include "probability/gui/selectable_table_view_with_background.h"
#include "probability/models/statistic/goodness_test.h"

namespace Probability {

/* This is the table used to input Expected and Observed results. */
class InputGoodnessTableView : public SelectableTableViewWithBackground,
                               public BorderedTableViewDataSource,
                               public DynamicCellsDataSource<Escher::EvenOddEditableTextCell, k_inputGoodnessTableNumberOfReusableCells>,
                               public DynamicSizeTableViewDataSource {
public:
  InputGoodnessTableView(Escher::Responder * parentResponder,
                         GoodnessTest * statistic,
                         Escher::TextFieldDelegate * textFieldDelegate,
                         DynamicCellsDataSourceDelegate<Escher::EvenOddEditableTextCell> * dynamicCellsDataSourceDelegate,
                         DynamicSizeTableViewDataSourceDelegate * dynamicSizeTableViewDataSourceDelegate = nullptr,
                         Escher::SelectableTableViewDelegate * scrollDelegate = nullptr);
  // DataSource
  int numberOfRows() const override { return m_numberOfRows; };
  int numberOfColumns() const override { return k_numberOfColumns; }
  int reusableCellCount(int type) override;
  Escher::HighlightCell * reusableCell(int i, int type) override;
  int typeAtLocation(int i, int j) override { return j == 0 ? k_typeOfHeader : k_typeOfInnerCells; }
  void willDisplayCellAtLocation(Escher::HighlightCell * cell, int i, int j) override;

  KDCoordinate columnWidth(int i) override { return k_columnWidth; }
  KDCoordinate verticalBorderWidth() override { return k_borderBetweenColumns; }
  KDCoordinate rowHeight(int j) override { return k_rowHeight; }

  bool recomputeNumberOfRows();

  constexpr static int k_minimumNumberOfRows = 2;
  constexpr static int k_numberOfColumns = 2;
  constexpr static int k_maxNumberOfReusableRows = (Ion::Display::Height - Escher::Metric::TitleBarHeight - Escher::Metric::StackTitleHeight) / HomogeneityTableDataSource::k_rowHeight + 2;
  constexpr static int k_numberOfReusableCells = k_numberOfColumns * k_maxNumberOfReusableRows;
  constexpr static int k_typeOfInnerCells = 0;
  constexpr static int k_typeOfHeader = 1;

  using Escher::SelectableTableView::unhighlightSelectedCell;  // Made public

private:
  constexpr static int k_rowHeight = 20;
  constexpr static int k_borderBetweenColumns = 1;
  constexpr static int k_columnWidth = (Ion::Display::Width - 2 * Escher::Metric::CommonLeftMargin -
                                        k_borderBetweenColumns) /
                                       2;

  int m_numberOfRows;

  GoodnessTest * m_statistic;

  Escher::EvenOddMessageTextCell m_header[k_numberOfColumns];

  Escher::SelectableTableViewDataSource m_tableSelection;
};

}  // namespace Probability

#endif /* PROBABILITY_GUI_INPUT_GOODNESS_TABLE_VIEW_H */
