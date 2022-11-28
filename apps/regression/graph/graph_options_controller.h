#ifndef REGRESSION_GRAPH_OPTIONS_CONTROLLER_H
#define REGRESSION_GRAPH_OPTIONS_CONTROLLER_H

#include <apps/shared/curve_view_cursor.h>
#include <apps/shared/button_with_separator.h>
#include <escher/button_cell.h>
#include <escher/explicit_selectable_list_view_controller.h>
#include <escher/expression_table_cell_with_message.h>
#include <escher/expression_table_cell_with_message_with_buffer.h>
#include <escher/message_table_cell_with_chevron_and_message.h>
#include <escher/message_table_cell.h>
#include <escher/message_table_cell_with_chevron.h>
#include <escher/spacer_cell.h>
#include "go_to_parameter_controller.h"
#include "residual_plot_controller.h"
#include "../store.h"

namespace Regression {

class GraphController;

class GraphOptionsController : public Escher::ExplicitSelectableListViewController {
public:
  GraphOptionsController(Escher::Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, Store * store, Shared::CurveViewCursor * cursor, GraphController * graphController);
  void removeRegression();

  const char * title() override;
  ViewController::TitlesDisplay titlesDisplay() override { return ViewController::TitlesDisplay::DisplayLastTitle; }
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;
  void fillCell(Escher::HighlightCell * cell) override;

  // MemoizedListViewDataSource
  int numberOfRows() const override { return k_maxNumberOfRows; }
  Escher::HighlightCell * cell(int index) override;
private:
  constexpr static int k_maxNumberOfRows = 10;

  bool displayRegressionEquationCell() const;
  bool displayRCell() const;
  bool displayR2Cell() const;
  bool displayResidualPlotCell() const;

  Escher::MessageTableCellWithChevronAndMessage m_changeRegressionCell;
  Escher::ExpressionTableCellWithMessage m_regressionEquationCell;
  Escher::ExpressionTableCellWithMessageWithBuffer m_rCell;
  Escher::ExpressionTableCellWithMessageWithBuffer m_r2Cell;
  Escher::MessageTableCell m_residualPlotCell;
  Escher::SpacerCell m_spacerCell1;
  Escher::MessageTableCellWithChevron m_xParameterCell;
  Escher::MessageTableCellWithChevron m_yParameterCell;
  Escher::SpacerCell m_spacerCell2;
  Escher::ButtonCell m_removeRegressionCell;
  GoToParameterController m_goToParameterController;
  ResidualPlotController m_residualPlotCellController;
  Store * m_store;
  GraphController * m_graphController;
};

}

#endif
