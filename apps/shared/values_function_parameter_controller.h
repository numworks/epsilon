#ifndef SHARED_VALUES_FUNCTION_PARAM_CONTROLLER_H
#define SHARED_VALUES_FUNCTION_PARAM_CONTROLLER_H

#include <escher.h>
#include "function.h"
#include <apps/i18n.h>

namespace Shared {

class ValuesFunctionParameterController : public ViewController, public SimpleListViewDataSource, public SelectableTableViewDataSource {
public:
  ValuesFunctionParameterController() :
    ViewController(nullptr),
    m_copyColumn(I18n::Message::CopyColumnInList),
    m_selectableTableView(this, this, this),
    m_record()
  {}

  View * view() override { return &m_selectableTableView; }
  const char * title() override;
  void viewWillAppear() override;
  void didBecomeFirstResponder() override;
  int numberOfRows() const override { return 1; }
  KDCoordinate cellHeight() override { return Metric::ParameterCellHeight; }
  HighlightCell * reusableCell(int index) override {
    assert(index == 0);
    return &m_copyColumn;
  }
  int reusableCellCount() const override { return 1; }
  void setRecord(Ion::Storage::Record record) { m_record = record; }
protected:
  MessageTableCellWithChevron<> m_copyColumn;
  SelectableTableView m_selectableTableView;
  Ion::Storage::Record m_record;
private:
  char m_pageTitle[Function::k_maxNameWithArgumentSize];
};

}

#endif

