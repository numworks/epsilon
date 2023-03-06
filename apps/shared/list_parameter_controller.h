#ifndef SHARED_LIST_PARAMETER_CONTROLLER_H
#define SHARED_LIST_PARAMETER_CONTROLLER_H

#include <apps/i18n.h>
#include <escher/message_table_cell_with_chevron_and_message.h>
#include <escher/message_table_cell_with_message_with_switch.h>
#include <escher/selectable_list_view_delegate.h>

#include "color_parameter_controller.h"
#include "function_store.h"

namespace Shared {

class ListParameterController
    : public Escher::ExplicitSelectableListViewController {
 public:
  ListParameterController(
      Responder* parentResponder, I18n::Message functionColorMessage,
      I18n::Message deleteFunctionMessage,
      Escher::SelectableListViewDelegate* listDelegate = nullptr);

  bool handleEvent(Ion::Events::Event event) override;
  TELEMETRY_ID("ListParameter");
  virtual void setRecord(Ion::Storage::Record record);
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;

  // MemoizedListViewDataSource
  int numberOfRows() const override { return k_numberOfSharedCells; }
  void willDisplayCellForIndex(Escher::HighlightCell* cell, int index) override;

 protected:
  // Type order defines cell order
  constexpr static int k_numberOfSharedCells = 3;
  FunctionStore* functionStore();
  ExpiringPointer<Function> function();
  Escher::MessageTableCellWithMessageWithSwitch m_enableCell;
  Escher::MessageTableCellWithChevronAndMessage m_colorCell;
  Escher::MessageTableCell m_deleteCell;
  Ion::Storage::Record m_record;

 private:
  ColorParameterController m_colorParameterController;
};

}  // namespace Shared

#endif
