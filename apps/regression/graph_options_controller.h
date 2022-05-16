#ifndef REGRESSION_GRAPH_OPTIONS_CONTROLLER_H
#define REGRESSION_GRAPH_OPTIONS_CONTROLLER_H

#include <apps/shared/curve_view_cursor.h>
#include <apps/shared/button_with_separator.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/expression_table_cell_with_message.h>
#include <escher/expression_table_cell_with_message_with_buffer.h>
#include <escher/message_table_cell_with_chevron_and_message.h>
#include <escher/message_table_cell.h>
#include <escher/message_table_cell_with_chevron.h>
#include "go_to_parameter_controller.h"
#include "message_table_cell_with_chevron_with_separator.h"
#include "residual_plot_controller.h"
#include "store.h"

namespace Regression {

class GraphController;

class GraphOptionsController : public Escher::SelectableListViewController<Escher::MemoizedListViewDataSource> {
public:
  GraphOptionsController(Escher::Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, Store * store, Shared::CurveViewCursor * cursor, GraphController * graphController);
  void removeRegression();

  const char * title() override { return I18n::translate(I18n::Message::RegressionCurve); }
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;

  // MemoizedListViewDataSource
  int numberOfRows() const override { return k_maxNumberOfRows - !displayRegressionEquationCell() - !displayR2Cell() - !displayResidualPlotCell(); }
  Escher::HighlightCell * reusableCell(int index, int type) override;
  // Each cell has its own type
  int reusableCellCount(int type) override { return 1; }
  int typeAtIndex(int index) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
private:
  // Type order is also used for cell order
  constexpr static int k_changeRegressionCellType = 0;
  constexpr static int k_regressionEquationCellType = 1;
  constexpr static int k_r2CellType = 2;
  constexpr static int k_residualPlotCellType = 3;
  constexpr static int k_xParameterCellType = 4;
  constexpr static int k_yParameterCellType = 5;
  constexpr static int k_removeRegressionCellType = 6;
  constexpr static int k_maxNumberOfRows = 7;

  bool displayRegressionEquationCell() const;
  bool displayR2Cell() const;
  bool displayResidualPlotCell() const;

  Escher::MessageTableCellWithChevronAndMessage m_changeRegressionCell;
  Escher::ExpressionTableCellWithMessage m_regressionEquationCell;
  Escher::ExpressionTableCellWithMessageWithBuffer m_r2Cell;
  Escher::MessageTableCell m_residualPlotCell;
  MessageTableCellWithChevronWithSeparator m_xParameterCell;
  Escher::MessageTableCellWithChevron m_yParameterCell;
  Shared::ButtonWithSeparator m_removeRegressionCell;
  GoToParameterController m_goToParameterController;
  ResidualPlotController m_residualPlotCellController;
  Store * m_store;
  GraphController * m_graphController;
};

}

#endif
