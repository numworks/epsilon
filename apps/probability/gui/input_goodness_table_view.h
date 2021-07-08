#ifndef APPS_PROBABILITY_GUI_INPUT_GOODNESS_TABLE_VIEW_H
#define APPS_PROBABILITY_GUI_INPUT_GOODNESS_TABLE_VIEW_H

#include <apps/shared/parameter_text_field_delegate.h>
#include <escher/even_odd_editable_text_cell.h>
#include <escher/even_odd_message_text_cell.h>
#include <escher/highlight_cell.h>
#include <escher/metric.h>
#include <escher/selectable_table_view.h>
#include <escher/table_view_data_source.h>

#include "probability/models/statistic/chi2_statistic.h"

namespace Probability {

/* This is the table used to input Expected and Observed results. */
class InputGoodnessTableView : public Escher::SelectableTableView,
                               public Escher::TableViewDataSource,
                               public Shared::ParameterTextFieldDelegate {
public:
  InputGoodnessTableView(Escher::Responder * parentResponder,
                         Escher::InputEventHandlerDelegate * inputEventHandlerDelegate,
                         Chi2Statistic * statistic);
  // DataSource
  int numberOfRows() const override { return k_initialNumberOfRows; };
  int numberOfColumns() const override { return k_numberOfColumns; }
  int reusableCellCount(int type) override { return numberOfRows() * numberOfColumns(); }
  Escher::HighlightCell * reusableCell(int i, int type) override;
  int typeAtLocation(int i, int j) override { return 0; }
  void willDisplayCellAtLocation(Escher::HighlightCell * cell, int i, int j) override;

  KDCoordinate columnWidth(int i) override { return k_columnWidth; }
  KDCoordinate rowHeight(int j) override { return k_rowHeight; }

  // TextFieldDelegate
  bool textFieldShouldFinishEditing(Escher::TextField * textField,
                                    Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(Escher::TextField * textField,
                                 const char * text,
                                 Ion::Events::Event event) override;
  bool textFieldDidHandleEvent(Escher::TextField * textField,
                               bool returnValue,
                               bool textSizeDidChange) override;

private:
  constexpr static int k_initialNumberOfRows = 4;
  constexpr static int k_numberOfColumns = 2;
  constexpr static int k_columnWidth = (Ion::Display::Width -
                                        2 * Escher::Metric::CommonLeftMargin) /
                                       2;
  constexpr static int k_rowHeight = 20;

  Chi2Statistic * m_statistic;

  Escher::EvenOddMessageTextCell m_header[k_numberOfColumns];
  Escher::EvenOddEditableTextCell m_cells[8];

  Escher::SelectableTableViewDataSource m_tableSelection;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_GUI_INPUT_GOODNESS_TABLE_VIEW_H */
