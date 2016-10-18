#ifndef GRAPH_FUNCTION_PARAM_CONTROLLER_H
#define GRAPH_FUNCTION_PARAM_CONTROLLER_H

#include <escher.h>
#include "../function.h"

namespace Graph {
class FunctionParameterController : public ViewController, public ListViewDataSource {
public:
  FunctionParameterController(Responder * parentResponder);

  View * view() override;
  const char * title() const override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;

  void setActiveCell(int index);
  int numberOfRows() override;
  KDCoordinate cellHeight() override;
  View * reusableCell(int index) override;
  int reusableCellCount() override;
  void willDisplayCellForIndex(View * cell, int index) override;

  void setFunction(Function * function);
private:
  constexpr static int k_totalNumberOfCell = 2;
  constexpr static int k_maxNumberOfCharsInTitle = 16;
  char m_pageTitle[k_maxNumberOfCharsInTitle];
  SwitchListViewCell m_displayDerivativeColumn;
  ListViewCell m_copyColumn;
  ListView m_listView;
  int m_activeCell;
  Function * m_function;
};

}

#endif
