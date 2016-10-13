#ifndef GRAPH_LIST_PARAM_CONTROLLER_H
#define GRAPH_LIST_PARAM_CONTROLLER_H

#include <escher.h>
#include "../function.h"
#include "../function_store.h"

namespace Graph {

class ParameterController : public ViewController, public ListViewDataSource {
public:
  ParameterController(Responder * parentResponder, FunctionStore * functionStore);

  View * view() override;
  const char * title() const override;
  bool handleEvent(Ion::Events::Event event) override;
  void setFunction(Function * function);
  void didBecomeFirstResponder() override;

  void setActiveCell(int index);
  int numberOfRows() override;
  KDCoordinate cellHeight() override;
  View * reusableCell(int index) override;
  int reusableCellCount() override;
  void willDisplayCellForIndex(View * cell, int index) override;
private:
  bool handleEnter();
  constexpr static int k_totalNumberOfCell = 3;
  TableViewCell m_colorCell;
  SwitchTableViewCell m_enableCell;
  TableViewCell m_deleteCell;
  ListView m_listView;
  int m_activeCell;
  Function * m_function;
  FunctionStore * m_functionStore;
};

}

#endif
