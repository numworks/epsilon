#pragma once

#include <apps/i18n.h>
#include <escher/buffer_text_view.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>
#include <escher/tab_view_controller.h>

#include "../data/store.h"

namespace Statistics::Categorical {

using GroupLabelCell =
    Escher::MenuCell<Escher::BufferTextView<sizeof(Store::Label)>>;

/* Menu controller to choose the displayed pie chart (group) */
class DisplayedDataController
    : public Escher::UniformSelectableListController<
          GroupLabelCell, Store::k_maxNumberOfGroups> {
 public:
  DisplayedDataController(Escher::Responder* parentResponder,
                          Escher::TabViewController* tabViewController,
                          Escher::StackViewController* stackView, Store* store);

  void initView() override;

  bool handleEvent(Ion::Events::Event event) override;

  const char* title() const override {
    return I18n::translate(I18n::Message::DisplayedData);
  }

  Escher::TabViewController* m_tabController;
  Escher::StackViewController* m_stackViewController;
  Store* m_store;
};

}  // namespace Statistics::Categorical
