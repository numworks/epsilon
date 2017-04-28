#ifndef SHARED_LIST_PARAM_CONTROLLER_H
#define SHARED_LIST_PARAM_CONTROLLER_H

#include <escher.h>
#include "function.h"
#include "function_store.h"
#include "../i18n.h"

namespace Shared {

class ListParameterController : public ViewController, public SimpleListViewDataSource, public SelectableTableViewDelegate {
public:
  ListParameterController(Responder * parentResponder, FunctionStore * functionStore, I18n::Message functionColorMessage, I18n::Message deleteFunctionMessage);

  View * view() override;
  const char * title() override;
  bool handleEvent(Ion::Events::Event event) override;
  virtual void setFunction(Function * function);
  void didBecomeFirstResponder() override;
  int numberOfRows() override;
  KDCoordinate cellHeight() override;
  HighlightCell * reusableCell(int index) override;
  int reusableCellCount() override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
protected:
  bool handleEnterOnRow(int rowIndex);
  SelectableTableView m_selectableTableView;
  FunctionStore * m_functionStore;
  Function * m_function;
private:
  /* TODO: implement function color choice */
  constexpr static int k_totalNumberOfCell = 2;//3;
  //MessageTableCellWithChevron m_colorCell;
  MessageTableCellWithSwitch m_enableCell;
  MessageTableCell m_deleteCell;
};

}

#endif
