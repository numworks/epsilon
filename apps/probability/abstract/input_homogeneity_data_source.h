#ifndef APPS_PROBABILITY_GUI_INPUT_HOMOGENEITY_DATA_SOURCE_H
#define APPS_PROBABILITY_GUI_INPUT_HOMOGENEITY_DATA_SOURCE_H

#include <apps/shared/text_field_delegate.h>
#include <escher/even_odd_editable_text_cell.h>
#include <escher/selectable_table_view.h>
#include <escher/table_view_data_source.h>

#include "probability/abstract/dynamic_data_source.h"
#include "probability/abstract/homogeneity_data_source.h"
#include "probability/models/statistic/homogeneity_statistic.h"

namespace Probability {

using namespace Escher;

class InputHomogeneityDataSource : public TableViewDataSource, public DynamicTableViewDataSource {
public:
  InputHomogeneityDataSource(SelectableTableView * tableView,
                             InputEventHandlerDelegate * inputEventHandlerDelegate,
                             HomogeneityStatistic * statistic,
                             TextFieldDelegate * textFieldDelegate,
                             DynamicTableViewDataSourceDelegate * dataSourceDelegate);
  int numberOfRows() const override { return m_numberOfRows; }
  int numberOfColumns() const override { return m_numberOfColumns; }
  int reusableCellCount(int type) override {
    return HomogeneityTableDataSource::k_numberOfReusableCells;
  }
  int typeAtLocation(int i, int j) override { return 0; }
  HighlightCell * reusableCell(int i, int type) override;

  KDCoordinate columnWidth(int i) override { return HomogeneityTableDataSource::k_columnWidth; }
  KDCoordinate rowHeight(int j) override { return HomogeneityTableDataSource::k_rowHeight; }

  void willDisplayCellAtLocation(Escher::HighlightCell * cell, int column, int row) override;

  // DynamicTableViewDataSource
  void addRow() override;
  void deleteLastRow() override;
  void addColumn() override;
  void deleteLastColumn() override;

private:
  int m_numberOfRows;
  int m_numberOfColumns;

  EvenOddEditableTextCell m_cells[HomogeneityTableDataSource::k_numberOfReusableCells];
  HomogeneityStatistic * m_statistic;
  SelectableTableView * m_table;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_GUI_INPUT_HOMOGENEITY_DATA_SOURCE_H */
