#ifndef REGRESSION_GRAPH_OPTIONS_CONTROLLER_H
#define REGRESSION_GRAPH_OPTIONS_CONTROLLER_H

#include <apps/shared/curve_view_cursor.h>
#include <escher/buffer_text_view.h>
#include <escher/button_cell.h>
#include <escher/chevron_view.h>
#include <escher/expression_table_cell_with_message.h>
#include <escher/expression_view.h>
#include <escher/menu_cell.h>
#include <escher/message_text_view.h>

#include "../store.h"
#include "go_to_parameter_controller.h"
#include "residual_plot_controller.h"

namespace Regression {

class GraphController;

class GraphOptionsController
    : public Escher::ExplicitSelectableListViewController {
 public:
  GraphOptionsController(
      Escher::Responder* parentResponder,
      Escher::InputEventHandlerDelegate* inputEventHandlerDelegate,
      Shared::InteractiveCurveViewRange* range, Store* store,
      Shared::CurveViewCursor* cursor, GraphController* graphController);
  void removeRegression();

  const char* title() override;
  ViewController::TitlesDisplay titlesDisplay() override {
    return ViewController::TitlesDisplay::DisplayLastTitle;
  }
  bool handleEvent(Ion::Events::Event event) override;
  void viewWillAppear() override;
  void fillCell(Escher::HighlightCell* cell) override;

  // MemoizedListViewDataSource
  int numberOfRows() const override { return k_maxNumberOfRows; }
  Escher::HighlightCell* cell(int index) override;
  KDCoordinate separatorBeforeRow(int index) override {
    return cell(index) == &m_xParameterCell ||
                   cell(index) == &m_removeRegressionCell
               ? k_defaultRowSeparator
               : 0;
  }

 private:
  constexpr static int k_maxNumberOfRows = 8;

  bool displayRegressionEquationCell() const;
  bool displayRCell() const;
  bool displayR2Cell() const;
  bool displayResidualPlotCell() const;

  using RCell =
      Escher::MenuCell<Escher::ExpressionView, Escher::MessageTextView,
                       Escher::BufferTextView>;

  Escher::MenuCell<Escher::MessageTextView, Escher::MessageTextView,
                   Escher::ChevronView>
      m_changeRegressionCell;
  Escher::ExpressionTableCellWithMessage m_regressionEquationCell;
  RCell m_rCell;
  RCell m_r2Cell;
  Escher::MenuCell<Escher::MessageTextView> m_residualPlotCell;
  Escher::MenuCell<Escher::MessageTextView, Escher::EmptyCellWidget,
                   Escher::ChevronView>
      m_xParameterCell;
  Escher::MenuCell<Escher::MessageTextView, Escher::EmptyCellWidget,
                   Escher::ChevronView>
      m_yParameterCell;
  Escher::ButtonCell m_removeRegressionCell;
  GoToParameterController m_goToParameterController;
  ResidualPlotController m_residualPlotCellController;
  Store* m_store;
  GraphController* m_graphController;
};

}  // namespace Regression

#endif
