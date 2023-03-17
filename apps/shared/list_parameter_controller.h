#ifndef SHARED_LIST_PARAMETER_CONTROLLER_H
#define SHARED_LIST_PARAMETER_CONTROLLER_H

#include <apps/i18n.h>
#include <escher/chevron_view.h>
#include <escher/menu_cell.h>
#include <escher/message_text_view.h>
#include <escher/selectable_list_view_delegate.h>
#include <escher/switch_view.h>

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
  void viewWillAppear() override;

  // MemoizedListViewDataSource
  int numberOfRows() const override { return k_numberOfSharedCells; }
  void willDisplayCellForIndex(Escher::HighlightCell* cell, int index) override;

 protected:
  // Type order defines cell order
  constexpr static int k_numberOfSharedCells = 3;
  FunctionStore* functionStore();
  ExpiringPointer<Function> function();

  Escher::MenuCell<Escher::MessageTextView, Escher::MessageTextView,
                   Escher::SwitchView>
      m_enableCell;
  Escher::MenuCell<Escher::MessageTextView, Escher::MessageTextView,
                   Escher::ChevronView>
      m_colorCell;
  Escher::MenuCell<Escher::MessageTextView> m_deleteCell;
  Ion::Storage::Record m_record;

 private:
  ColorParameterController m_colorParameterController;
};

}  // namespace Shared

#endif
