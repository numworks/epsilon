#ifndef GRAPH_FUNCTION_PARAM_CONTROLLER_H
#define GRAPH_FUNCTION_PARAM_CONTROLLER_H

#include <escher.h>
#include "../cartesian_function.h"

namespace Graph {

class ValuesController;

class FunctionParameterController : public ViewController, public SimpleListViewDataSource {
public:
  FunctionParameterController(ValuesController * valuesController);

  View * view() override;
  const char * title() const override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  int numberOfRows() override;
  KDCoordinate cellHeight() override;
  TableViewCell * reusableCell(int index) override;
  int reusableCellCount() override;
  void willDisplayCellForIndex(TableViewCell * cell, int index) override;

  void setFunction(CartesianFunction * function);
private:
  constexpr static int k_totalNumberOfCell = 2;
  constexpr static int k_maxNumberOfCharsInTitle = 16;
  char m_pageTitle[k_maxNumberOfCharsInTitle];
  SwitchMenuListCell m_displayDerivativeColumn;
  ChevronMenuListCell m_copyColumn;
  SelectableTableView m_selectableTableView;
  CartesianFunction * m_function;
  ValuesController * m_valuesController;
};

}

#endif
