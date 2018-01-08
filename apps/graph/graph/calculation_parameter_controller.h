#ifndef GRAPH_CALCULATION_PARAMETER_CONTROLLER_H
#define GRAPH_CALCULATION_PARAMETER_CONTROLLER_H

#include <escher.h>
#include "../../shared/function.h"
#include "../../i18n.h"

namespace Graph {

class GraphController;

class CalculationParameterController : public ViewController, public SimpleListViewDataSource, public SelectableTableViewDataSource {
public:
  CalculationParameterController(Responder * parentResponder, GraphController * graphController);
  View * view() override;
  const char * title() override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  int numberOfRows() override;
  KDCoordinate cellHeight() override;
  HighlightCell * reusableCell(int index) override;
  int reusableCellCount() override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  void setFunction(Shared::Function * function);
private:
  constexpr static int k_totalNumberOfCells = 6;
  MessageTableCell m_cells[k_totalNumberOfCells];
  SelectableTableView m_selectableTableView;
  Shared::Function * m_function;
  GraphController * m_graphController;
};

}

#endif

