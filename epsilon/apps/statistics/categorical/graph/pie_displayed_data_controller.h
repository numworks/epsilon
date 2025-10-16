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

class DisplayedDataController
    : public Escher::UniformSelectableListController<
          GroupLabelCell, Store::k_maxNumberOfGroups> {
 public:
  DisplayedDataController(Escher::Responder* parentResponder,
                          Escher::TabViewController* tabViewController,
                          Escher::StackViewController* stackView, Store* store)
      : Escher::UniformSelectableListController<GroupLabelCell,
                                                k_numberOfCells>(
            parentResponder),
        m_tabController(tabViewController),
        m_stackViewController(stackView),
        m_store(store) {};

  void initView() override {
    int selectedGroup = m_store->getSelectedGroupForPieGraph();
    assert(m_store->isGroupActive(selectedGroup));
    for (uint8_t i = 0; i < Store::k_maxNumberOfGroups; i++) {
      if (!m_store->isGroupActive(i)) {
        cell(i)->setVisible(false);
        continue;
      }
      cell(i)->label()->setGlyphFormat(Escher::GlyphsView::FormatForWidgetType(
          Escher::CellWidget::Type::Label));
      char buffer[20];
      m_store->getGroupName(i, buffer, sizeof(buffer));
      cell(i)->label()->setText(buffer);
      cell(i)->setVisible(true);
      if (i == selectedGroup) {
        selectRow(i);
      }
    }
  }

  bool handleEvent(Ion::Events::Event event) override {
    if (event == Ion::Events::Up && selectedRow() == 0) {
      m_tabController->selectTab();
      return true;
    }
    if (event == Ion::Events::Back) {
      m_stackViewController->pop();
      return true;
    }
    if (event == Ion::Events::OK || event == Ion::Events::EXE ||
        event == Ion::Events::Right) {
      m_store->setSelectedGroupForPieGraph(selectedRow());
      m_stackViewController->pop();
      return true;
    }
    return false;
  }

  const char* title() const override {
    return I18n::translate(I18n::Message::DisplayedData);
  }

  Escher::TabViewController* m_tabController;
  Escher::StackViewController* m_stackViewController;
  Store* m_store;
};

}  // namespace Statistics::Categorical
