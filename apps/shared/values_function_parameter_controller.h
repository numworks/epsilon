#ifndef SHARED_VALUES_FUNCTION_PARAM_CONTROLLER_H
#define SHARED_VALUES_FUNCTION_PARAM_CONTROLLER_H

#include <apps/i18n.h>
#include <escher/message_table_cell_with_chevron.h>
#include <escher/selectable_table_view_data_source.h>
#include <escher/selectable_list_view_controller.h>
#include "function.h"

namespace Shared {

class ValuesFunctionParameterController : public Escher::SelectableListViewController {
public:
  ValuesFunctionParameterController() :
    Escher::SelectableListViewController(nullptr),
    m_copyColumn(I18n::Message::CopyColumnInList),
    m_record()
  {}

  const char * title() override;
  void viewWillAppear() override;
  void didBecomeFirstResponder() override;
  int numberOfRows() const override { return 1; }
  Escher::HighlightCell * reusableCell(int index, int type) override {
    assert(index == 0);
    return &m_copyColumn;
  }
  void setRecord(Ion::Storage::Record record) { m_record = record; }
protected:
  Escher::MessageTableCellWithChevron m_copyColumn;
  Ion::Storage::Record m_record;
private:
  char m_pageTitle[Function::k_maxNameWithArgumentSize];
};

}

#endif

