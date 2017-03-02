#ifndef SHARED_LIST_PARAM_CONTROLLER_H
#define SHARED_LIST_PARAM_CONTROLLER_H

#include <escher.h>
#include "function.h"
#include "function_store.h"

namespace Shared {

class ListParameterController : public ViewController, public SimpleListViewDataSource {
public:
  ListParameterController(Responder * parentResponder, FunctionStore * functionStore);

  View * view() override;
  const char * title() const override;
  bool handleEvent(Ion::Events::Event event) override;
  virtual void setFunction(Function * function);
  void didBecomeFirstResponder() override;
  int numberOfRows() override;
  KDCoordinate cellHeight() override;
  HighlightCell * reusableCell(int index) override;
  int reusableCellCount() override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
protected:
  SelectableTableView m_selectableTableView;
  FunctionStore * m_functionStore;
  Function * m_function;
private:
  constexpr static int k_totalNumberOfCell = 3;
  PointerTableCellWithChevron m_colorCell;
  PointerTableCellWithSwitch m_enableCell;
  PointerTableCell m_deleteCell;
};

}

#endif
