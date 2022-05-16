#ifndef PROBABILITY_GUI_GOODNESS_TABLE_CELL_H
#define PROBABILITY_GUI_GOODNESS_TABLE_CELL_H

#include <escher/even_odd_editable_text_cell.h>
#include <escher/even_odd_message_text_cell.h>
#include <escher/metric.h>
#include "inference/statistic/chi_square_and_slope/categorical_table_cell.h"
#include "inference/statistic/chi_square_and_slope/categorical_table_view_data_source.h"
#include "inference/shared/dynamic_cells_data_source.h"
#include "inference/models/statistic/goodness_test.h"

namespace Inference {

class InputGoodnessController;

class GoodnessTableCell : public EditableCategoricalTableCell, public CategoricalTableViewDataSource, public DynamicCellsDataSource<Escher::EvenOddEditableTextCell, k_inputGoodnessTableNumberOfReusableCells> {
public:
  GoodnessTableCell(Escher::Responder * parentResponder, DynamicSizeTableViewDataSourceDelegate * dynamicSizeTableViewDataSourceDelegate, Escher::SelectableTableViewDelegate * selectableTableViewDelegate, GoodnessTest * test, InputGoodnessController * inputGoodnessController);

  // EditableCategoricalTableCell
  CategoricalTableViewDataSource * tableViewDataSource() override { return this; }

  // DataSource
  int numberOfRows() const override { return m_numberOfRows + 1; } // Add header
  int numberOfColumns() const override { return m_numberOfColumns; }
  int reusableCellCount(int type) override;
  Escher::HighlightCell * reusableCell(int i, int type) override;
  int typeAtLocation(int i, int j) override { return j == 0 ? k_typeOfHeaderCells : k_typeOfInnerCells; }
  void willDisplayCellAtLocation(Escher::HighlightCell * cell, int i, int j) override;
  KDCoordinate columnWidth(int i) override { return k_columnWidth; }

  // Responder
  bool textFieldDidFinishEditing(Escher::TextField * textField, const char * text, Ion::Events::Event event) override;

  // DynamicSizeTableViewDataSource
  bool recomputeDimensions(Chi2Test * test) override;

  // DynamicCellsDataSource
  Escher::SelectableTableView * tableView() override { return &m_selectableTableView; }

  constexpr static int k_numberOfReusableCells = GoodnessTest::k_maxNumberOfColumns * k_maxNumberOfReusableRows;

private:
  static constexpr I18n::Message k_columnHeaders[GoodnessTest::k_maxNumberOfColumns] = {I18n::Message::Observed, I18n::Message::Expected};

  // ClearColumnHelper
  int fillColumnName(int column, char * buffer) override;

  // CategoricalTableViewDataSource
  int relativeColumnIndex(int columnIndex) const override { return columnIndex; }

  GoodnessTest * statistic() { return static_cast<GoodnessTest *>(m_statistic); }

  Escher::EvenOddMessageTextCell m_header[GoodnessTest::k_maxNumberOfColumns];
  InputGoodnessController * m_inputGoodnessController;
};

}  // namespace Inference

#endif /* PROBABILITY_GUI_CATEGORICAL_TABLE_CELL_H */
