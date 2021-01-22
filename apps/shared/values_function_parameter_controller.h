#ifndef SHARED_VALUES_FUNCTION_PARAM_CONTROLLER_H
#define SHARED_VALUES_FUNCTION_PARAM_CONTROLLER_H

#include <apps/i18n.h>
#include <escher/message_table_cell_with_chevron.h>
#include <escher/selectable_table_view.h>
#include <escher/selectable_table_view_data_source.h>
#include <escher/simple_list_view_data_source.h>
#include <escher/view_controller.h>
#include "function.h"

namespace Shared {

class ValuesFunctionParameterController : public Escher::ViewController, public Escher::SimpleListViewDataSource, public Escher::SelectableTableViewDataSource {
public:
  ValuesFunctionParameterController() :
    Escher::ViewController(nullptr),
    m_copyColumn(I18n::Message::CopyColumnInList),
    m_selectableTableView(this, this, this),
    m_record()
  {}

  Escher::View * view() override { return &m_selectableTableView; }
  const char * title() override;
  void viewWillAppear() override;
  void didBecomeFirstResponder() override;
  int numberOfRows() const override { return 1; }
  KDCoordinate cellWidth() override {
    assert(m_selectableTableView.columnWidth(0) > 0);
    return m_selectableTableView.columnWidth(0);
  }
  Escher::HighlightCell * reusableCell(int index, int type) override {
    assert(index == 0);
    return &m_copyColumn;
  }
  void setRecord(Ion::Storage::Record record) { m_record = record; }
protected:
  Escher::MessageTableCellWithChevron m_copyColumn;
  Escher::SelectableTableView m_selectableTableView;
  Ion::Storage::Record m_record;
private:
  char m_pageTitle[Function::k_maxNameWithArgumentSize];
};

}

#endif

