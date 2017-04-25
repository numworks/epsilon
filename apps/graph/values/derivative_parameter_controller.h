#ifndef GRAPH_DERIVATIVE_PARAM_CONTROLLER_H
#define GRAPH_DERIVATIVE_PARAM_CONTROLLER_H

#include <escher.h>
#include "../cartesian_function.h"

namespace Graph {

class ValuesController;

class DerivativeParameterController : public ViewController, public SimpleListViewDataSource {
public:
  DerivativeParameterController(ValuesController * valuesController);

  View * view() override;
  const char * title() override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  int numberOfRows() override;
  KDCoordinate cellHeight() override;
  HighlightCell * reusableCell(int index) override;
  int reusableCellCount() override;

  void setFunction(CartesianFunction * function);
private:
  constexpr static int k_totalNumberOfCell = 1;//2;
  constexpr static int k_maxNumberOfCharsInTitle = 16;
  char m_pageTitle[k_maxNumberOfCharsInTitle];
  MessageTableCell m_hideColumn;
//  MessageTableCellWithChevron m_copyColumn;
  SelectableTableView m_selectableTableView;
  CartesianFunction * m_function;
  ValuesController * m_valuesController;
};

}

#endif
