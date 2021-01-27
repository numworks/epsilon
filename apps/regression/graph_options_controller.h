#ifndef REGRESSION_GRAPH_OPTIONS_CONTROLLER_H
#define REGRESSION_GRAPH_OPTIONS_CONTROLLER_H

#include <escher/message_table_cell_with_chevron.h>
#include <escher/message_table_cell_with_chevron_and_expression.h>
#include "store.h"
#include "../shared/curve_view_cursor.h"
#include "go_to_parameter_controller.h"

namespace Regression {

class GraphController;

class GraphOptionsController : public Escher::ViewController, public Escher::SimpleListViewDataSource, public Escher::SelectableTableViewDataSource {
public:
  GraphOptionsController(Escher::Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, Store * store, Shared::CurveViewCursor * cursor, GraphController * graphController);
  Escher::View * view() override;
  const char * title() override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;

  // SimpleListViewDataSource
  int numberOfRows() const override;
  KDCoordinate cellWidth() override { return m_selectableTableView.columnWidth(0); }
  Escher::HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtIndex(int index) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
private:
  constexpr static int k_regressionCellType = 0;
  constexpr static int k_parameterCelltype = 1;
  constexpr static int k_numberOfParameterCells = 2;
  Escher::MessageTableCellWithChevron m_parameterCells[k_numberOfParameterCells];
  Escher::MessageTableCellWithChevronAndExpression m_changeRegressionCell;
  Escher::SelectableTableView m_selectableTableView;
  GoToParameterController m_goToParameterController;
  Store * m_store;
  GraphController * m_graphController;
};

}

#endif
