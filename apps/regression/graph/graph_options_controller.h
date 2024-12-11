#ifndef REGRESSION_GRAPH_OPTIONS_CONTROLLER_H
#define REGRESSION_GRAPH_OPTIONS_CONTROLLER_H

#include <apps/shared/curve_view_cursor.h>
#include <escher/buffer_text_view.h>
#include <escher/button_cell.h>
#include <escher/chevron_view.h>
#include <escher/layout_view.h>
#include <escher/menu_cell.h>
#include <escher/message_text_view.h>
#include <escher/scrollable_layout_view.h>

#include "../store.h"
#include "go_to_parameter_controller.h"
#include "residual_plot_controller.h"

namespace Regression {

class GraphController;

class GraphOptionsController
    : public Escher::ExplicitSelectableListViewController {
 public:
  GraphOptionsController(Escher::Responder* parentResponder,
                         Shared::InteractiveCurveViewRange* range, Store* store,
                         Shared::CurveViewCursor* cursor,
                         GraphController* graphController);
  void removeRegression();

  const char* title() const override;
  ViewController::TitlesDisplay titlesDisplay() const override {
    return ViewController::TitlesDisplay::DisplayLastTitle;
  }
  bool handleEvent(Ion::Events::Event event) override;
  void viewWillAppear() override;

  // MemoizedListViewDataSource
  int numberOfRows() const override { return k_maxNumberOfRows; }
  Escher::HighlightCell* cell(int row) override;
  KDCoordinate separatorBeforeRow(int row) override {
    return cell(row) == &m_xParameterCell ||
                   cell(row) == &m_removeRegressionCell
               ? k_defaultRowSeparator
               : 0;
  }

 private:
  constexpr static int k_maxNumberOfRows = 8;

  bool displayRegressionEquationCell() const;
  bool displayRCell() const;
  bool displayR2Cell() const;
  bool displayResidualPlotCell() const;

  using RCell = Escher::MenuCell<Escher::LayoutView, Escher::EmptyCellWidget,
                                 Escher::FloatBufferTextView<>>;

  Escher::MenuCell<Escher::MessageTextView, Escher::MessageTextView,
                   Escher::ChevronView>
      m_changeRegressionCell;
  Escher::MenuCell<Escher::ScrollableLayoutView, Escher::MessageTextView>
      m_regressionEquationCell;
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
