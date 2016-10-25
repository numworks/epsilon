#ifndef GRAPH_LIST_PARAM_CONTROLLER_H
#define GRAPH_LIST_PARAM_CONTROLLER_H

#include <escher.h>
#include "../function.h"
#include "../function_store.h"

namespace Graph {

class ParameterController : public ViewController, public SimpleListViewDataSource {
public:
  ParameterController(Responder * parentResponder, FunctionStore * functionStore);

  View * view() override;
  const char * title() const override;
  bool handleEvent(Ion::Events::Event event) override;
  void setFunction(Function * function);
  void didBecomeFirstResponder() override;
  int numberOfRows() override;
  KDCoordinate cellHeight() override;
  TableViewCell * reusableCell(int index) override;
  int reusableCellCount() override;
  void willDisplayCellForIndex(TableViewCell * cell, int index) override;
private:
  bool handleEnter();
  constexpr static int k_totalNumberOfCell = 3;
  ListViewCell m_colorCell;
  SwitchListViewCell m_enableCell;
  ListViewCell m_deleteCell;
  SelectableTableView m_selectableTableView;
  Function * m_function;
  FunctionStore * m_functionStore;
};

}

#endif
