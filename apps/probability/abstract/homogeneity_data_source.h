#ifndef APPS_PROBABILITY_ABSTRACT_HOMOGENEITY_DATA_SOURCE_H
#define APPS_PROBABILITY_ABSTRACT_HOMOGENEITY_DATA_SOURCE_H

#include <apps/i18n.h>
#include <apps/shared/parameter_text_field_delegate.h>
#include <escher/even_odd_buffer_text_cell.h>
#include <escher/input_event_handler_delegate.h>
#include <escher/selectable_table_view.h>
#include <escher/table_view_data_source.h>
#include <escher/text_field_delegate.h>

#include "probability/gui/bordered_table_view_data_source.h"
#include "probability/gui/solid_color_cell.h"
#include "probability/models/statistic/homogeneity_statistic.h"

using namespace Escher;

namespace Probability {

/* This class wraps a TableViewDataSource by adding a Row & Column header around it.
 * Specifically meant for InputHomogeneity and HomogeneityResults. */
// TODO memoize
class HomogeneityTableDataSource : public BorderedTableViewDataSource,
                                   public SelectableTableViewDataSource,
                                   public SelectableTableViewDelegate {
public:
  HomogeneityTableDataSource(TableViewDataSource * contentTable,
                             I18n::Message headerPrefix = I18n::Message::Group);
  int numberOfRows() const override { return m_contentTable->numberOfRows() + 1; }
  int numberOfColumns() const override { return m_contentTable->numberOfColumns() + 1; }
  int reusableCellCount(int type) override {
    return type == 0 ? m_contentTable->reusableCellCount(0) + numberOfColumns() + numberOfRows() - 1
                     : m_contentTable->reusableCellCount(type);
  }
  int typeAtLocation(int i, int j) override;
  HighlightCell * reusableCell(int i, int type) override;
  void willDisplayCellAtLocation(Escher::HighlightCell * cell, int column, int row) override {
    if (row > 0 && column > 0) {
      m_contentTable->willDisplayCellAtLocation(cell, column - 1, row - 1);
    }
  }

  KDCoordinate columnWidth(int i) override { return k_columnWidth; }
  KDCoordinate verticalBorderWidth() override { return k_borderBetweenColumns; }
  KDCoordinate rowHeight(int j) override { return k_rowHeight; }

  void tableViewDidChangeSelection(SelectableTableView * t,
                                   int previousSelectedCellX,
                                   int previousSelectedCellY,
                                   bool withinTemporarySelection = false) override {
    if (t->selectedRow() == 0 && t->selectedColumn() == 0) {
      t->selectRow(previousSelectedCellY);
      t->selectColumn(previousSelectedCellX);
    }
  }

  constexpr static int k_columnWidth = 80;
  constexpr static int k_borderBetweenColumns = 1;
  constexpr static int k_rowHeight = 20;

  constexpr static int k_initialNumberOfRows = 3;
  constexpr static int k_initialNumberOfColumns = 3;
  constexpr static int k_maxNumberOfColumns = 6;
  constexpr static int k_maxNumberOfRows = 6;
  constexpr static int k_maxNumberOfInnerCells = (k_maxNumberOfColumns - 1) *
                                                 (k_maxNumberOfRows - 1);

private:
  int indexForEditableCell(int i);

  TableViewDataSource * m_contentTable;

  SolidColorCell m_topLeftCell;
  EvenOddBufferTextCell m_rowHeader[k_maxNumberOfRows];
  EvenOddBufferTextCell m_colHeader[k_maxNumberOfColumns];
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_ABSTRACT_HOMOGENEITY_DATA_SOURCE_H */
