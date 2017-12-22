#ifndef SHARED_LIST_PARAM_CONTROLLER_H
#define SHARED_LIST_PARAM_CONTROLLER_H

#include <escher.h>
#include "function.h"
#include "function_store.h"
#include "../i18n.h"

namespace Shared {

class ListParameterController : public ViewController, public SimpleListViewDataSource, public SelectableTableViewDataSource {
public:
  ListParameterController(Responder * parentResponder, FunctionStore * functionStore, I18n::Message functionColorMessage, I18n::Message deleteFunctionMessage, SelectableTableViewDelegate * tableDelegate = nullptr);

  View * view() override;
  const char * title() override;
  bool handleEvent(Ion::Events::Event event) override;
  virtual void setFunction(Function * function);
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;
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
#if FUNCTION_COLOR_CHOICE
  constexpr static int k_totalNumberOfCell = 3;
  MessageTableCellWithChevron m_colorCell;
#else
  constexpr static int k_totalNumberOfCell = 2;
#endif
  MessageTableCellWithSwitch m_enableCell;
  MessageTableCell m_deleteCell;
};

}

#endif
