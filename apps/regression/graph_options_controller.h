#ifndef REGRESSION_GRAPH_OPTIONS_CONTROLLER_H
#define REGRESSION_GRAPH_OPTIONS_CONTROLLER_H

#include <escher.h>
#include "store.h"
#include "../shared/curve_view_cursor.h"
#include "go_to_parameter_controller.h"

namespace Regression {

class GraphController;

class GraphOptionsController : public ViewController, public ListViewDataSource, public SelectableTableViewDataSource {
public:
  GraphOptionsController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, Store * store, Shared::CurveViewCursor * cursor, GraphController * graphController);
  View * view() override;
  const char * title() override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;

  //ListViewDataSource
  int numberOfRows() const override;
  KDCoordinate rowHeight(int j) override;
  KDCoordinate cumulatedHeightFromIndex(int j) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtLocation(int i, int j) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
private:
  constexpr static int k_regressionCellType = 0;
  constexpr static int k_parameterCelltype = 1;
  constexpr static int k_numberOfParameterCells = 2;
  MessageTableCellWithChevron<> m_parameterCells[k_numberOfParameterCells];
  MessageTableCellWithChevronAndExpression m_changeRegressionCell;
  SelectableTableView m_selectableTableView;
  GoToParameterController m_goToParameterController;
  Store * m_store;
  GraphController * m_graphController;
};

}

#endif
