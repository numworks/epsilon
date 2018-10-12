#ifndef SHARED_STORAGE_VALUES_FUNCTION_PARAM_CONTROLLER_H
#define SHARED_STORAGE_VALUES_FUNCTION_PARAM_CONTROLLER_H

#include <escher.h>
#include "storage_function.h"
#include "../i18n.h"

namespace Shared {

class StorageValuesFunctionParameterController : public ViewController, public SimpleListViewDataSource, public SelectableTableViewDataSource {
public:
  StorageValuesFunctionParameterController(char symbol) :
    ViewController(nullptr),
    m_copyColumn(I18n::Message::CopyColumnInList),
    m_selectableTableView(this, this, this),
    m_function(nullptr),
    m_symbol(symbol)
  {}

  View * view() override { return &m_selectableTableView; }
  const char * title() override;
  void didBecomeFirstResponder() override;
  virtual int numberOfRows() override { return 1; }
  KDCoordinate cellHeight() override { return Metric::ParameterCellHeight; }
  virtual HighlightCell * reusableCell(int index) override {
    assert(index == 0);
    return &m_copyColumn;
  }
  virtual int reusableCellCount() override { return 1; }
  virtual void setFunction(StorageFunction * function) { m_function = function; }
protected:
  MessageTableCellWithChevron m_copyColumn;
  SelectableTableView m_selectableTableView;
private:
  constexpr static int k_maxNumberOfCharsInTitle = 16;
  char m_pageTitle[k_maxNumberOfCharsInTitle];
  StorageFunction * m_function;
  char m_symbol;
};

}

#endif

