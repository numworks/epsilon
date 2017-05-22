#ifndef SHARED_VALUES_FUNCTION_PARAM_CONTROLLER_H
#define SHARED_VALUES_FUNCTION_PARAM_CONTROLLER_H

#include <escher.h>
#include "function.h"
#include "../i18n.h"

namespace Shared {

class ValuesFunctionParameterController : public ViewController, public SimpleListViewDataSource, public SelectableTableViewDataSource {
public:
  ValuesFunctionParameterController(char symbol);

  View * view() override;
  const char * title() override;
  void didBecomeFirstResponder() override;
  virtual int numberOfRows() override;
  KDCoordinate cellHeight() override;
  virtual HighlightCell * reusableCell(int index) override;
  virtual int reusableCellCount() override;
  virtual void setFunction(Function * function);
protected:
  MessageTableCellWithChevron m_copyColumn;
  SelectableTableView m_selectableTableView;
private:
  constexpr static int k_maxNumberOfCharsInTitle = 16;
  char m_pageTitle[k_maxNumberOfCharsInTitle];
  Function * m_function;
  char m_symbol;
};

}

#endif

