#ifndef APPS_PROBABILITY_CONTROLLERS_INPUT_HOMOGENEITY_CONTROLLER_H
#define APPS_PROBABILITY_CONTROLLERS_INPUT_HOMOGENEITY_CONTROLLER_H

#include <apps/shared/parameter_text_field_delegate.h>
#include <escher/even_odd_buffer_text_cell.h>
#include <escher/even_odd_editable_text_cell.h>
#include <escher/highlight_cell.h>
#include <escher/selectable_table_view.h>
#include <escher/solid_color_view.h>
#include <escher/table_view_data_source.h>
#include <kandinsky/color.h>
#include <kandinsky/coordinate.h>

#include "homogeneity_results_controller.h"
#include "probability/abstract/button_delegate.h"
#include "probability/abstract/chained_selectable_table_view_delegate.h"
#include "probability/abstract/homogeneity_data_source.h"
#include "probability/abstract/input_categorical_controller.h"
#include "probability/gui/input_categorical_view.h"
#include "probability/gui/page_controller.h"
#include "probability/gui/selectable_table_view_with_background.h"
#include "probability/gui/solid_color_cell.h"
#include "probability/models/statistic/homogeneity_statistic.h"

namespace Probability {

class InputHomogeneityDataSource : public TableViewDataSource, public Shared::TextFieldDelegate {
public:
  InputHomogeneityDataSource(SelectableTableView * tableView,
                             InputEventHandlerDelegate * inputEventHandlerDelegate,
                             HomogeneityStatistic * statistic);
  int numberOfRows() const override { return m_numberOfRows; }
  int numberOfColumns() const override { return m_numberOfColumns; }
  int reusableCellCount(int type) override { return numberOfRows() * numberOfColumns(); }
  int typeAtLocation(int i, int j) override { return 0; }
  HighlightCell * reusableCell(int i, int type) override;

  KDCoordinate columnWidth(int i) override { return HomogeneityTableDataSource::k_columnWidth; }
  KDCoordinate rowHeight(int j) override { return HomogeneityTableDataSource::k_rowHeight; }

  void willDisplayCellAtLocation(Escher::HighlightCell * cell, int column, int row) override;
  // TextFieldDelegate
  bool textFieldShouldFinishEditing(Escher::TextField * textField,
                                    Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(Escher::TextField * textField,
                                 const char * text,
                                 Ion::Events::Event event) override;

private:
  int m_numberOfRows;
  int m_numberOfColumns;
  // TODO reusable
  EvenOddEditableTextCell m_cells[HomogeneityTableDataSource::k_maxNumberOfInnerCells];
  HomogeneityStatistic * m_statistic;
  SelectableTableView * m_table;
};

class InputHomogeneityController : public InputCategoricalController,
                                   public ChainedSelectableTableViewDelegate {
public:
  InputHomogeneityController(StackViewController * parent,
                             HomogeneityResultsController * homogeneityResultsController,
                             InputEventHandlerDelegate * inputEventHandlerDelegate,
                             HomogeneityStatistic * statistic);

  const char * title() override { return "x2-test: Homogeneity/Independence"; }

  void tableViewDidChangeSelectionAndDidScroll(SelectableTableView * t,
                                               int previousSelectedCellX,
                                               int previousSelectedCellY,
                                               bool withinTemporarySelection = false) override;

private:
  InputHomogeneityDataSource m_innerTableData;
  HomogeneityTableDataSource m_tableData;
  SelectableTableViewWithBackground m_table;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_CONTROLLERS_INPUT_HOMOGENEITY_CONTROLLER_H */
