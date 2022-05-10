#ifndef REGRESSION_GRAPH_OPTIONS_CONTROLLER_H
#define REGRESSION_GRAPH_OPTIONS_CONTROLLER_H

#include <escher/selectable_list_view_controller.h>
#include <escher/message_table_cell_with_chevron.h>
#include <escher/message_table_cell_with_chevron_and_message.h>
#include "store.h"
#include "../shared/curve_view_cursor.h"
#include "go_to_parameter_controller.h"

namespace Regression {

class GraphController;

class GraphOptionsController : public Escher::SelectableListViewController<Escher::RegularListViewDataSource> {
public:
  GraphOptionsController(Escher::Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, Store * store, Shared::CurveViewCursor * cursor, GraphController * graphController);
  const char * title() override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;

  // MemoizedListViewDataSource
  int numberOfRows() const override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtIndex(int index) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
private:
  constexpr static int k_regressionCellType = 0;
  constexpr static int k_parameterCelltype = 1;
  constexpr static int k_numberOfParameterCells = 2;
  Escher::MessageTableCellWithChevron m_parameterCells[k_numberOfParameterCells];
  Escher::MessageTableCellWithChevronAndMessage m_changeRegressionCell;
  GoToParameterController m_goToParameterController;
  Store * m_store;
  GraphController * m_graphController;
};

}

#endif
