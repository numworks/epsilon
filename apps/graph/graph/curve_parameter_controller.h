#ifndef GRAPH_GRAPH_CURVE_PARAMETER_CONTROLLER_H
#define GRAPH_GRAPH_CURVE_PARAMETER_CONTROLLER_H

#include <escher.h>
#include "goto_parameter_controller.h"
#include "../function.h"

namespace Graph {

class CurveParameterController : public ViewController, public SimpleListViewDataSource {
public:
  CurveParameterController(GraphView * graphView);

  View * view() override;
  const char * title() const override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  int numberOfRows() override;
  KDCoordinate cellHeight() override;
  TableViewCell * reusableCell(int index) override;
  int reusableCellCount() override;
  void willDisplayCellForIndex(TableViewCell * cell, int index) override;
  bool displayDerivative() const;
  void setFunction(Function * function);
private:
  GraphView * m_graphView;
  bool m_displayDerivative;
  Function * m_function;
  constexpr static int k_totalNumberOfCells = 3;
  ChevronMenuListCell m_calculationCell;
  ChevronMenuListCell m_goToCell;
  SwitchMenuListCell m_derivativeCell;
  SelectableTableView m_selectableTableView;
  GoToParameterController m_goToParameterController;
};

}

#endif
