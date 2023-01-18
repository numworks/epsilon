#ifndef ESCHER_EXPRESSION_CELL_WITH_SUBLABEL_AND_DROPDOWN_H
#define ESCHER_EXPRESSION_CELL_WITH_SUBLABEL_AND_DROPDOWN_H

#include <escher/i18n.h>
#include <escher/container.h>
#include <escher/dropdown_view.h>
#include <escher/message_table_cell_with_message.h>
#include <escher/responder.h>

namespace Escher {

class MessageTableCellWithSublabelAndDropdown : public MessageTableCellWithMessage, public Responder {
public:
  MessageTableCellWithSublabelAndDropdown(Responder * parentResponder, ListViewDataSource * listDataSource, DropdownCallback * callback = nullptr) :
        MessageTableCellWithMessage(),
        Responder(parentResponder),
        m_dropdown(this, listDataSource, callback) {}
  const View * accessoryView() const override { return &m_dropdown; }
  Dropdown * dropdown() { return &m_dropdown; }
  Responder * responder() override { return this; }
  void didBecomeFirstResponder() override {
    Container::activeApp()->setFirstResponder(&m_dropdown);
  }
  bool shouldAlignSublabelRight() const override { return false; }
  void reload() {
    m_dropdown.reloadAllCells();
    markRectAsDirty(bounds());
    layoutSubviews();
  }
  void setHighlighted(bool highlight) override {
    m_dropdown.setHighlighted(highlight);
    MessageTableCellWithMessage::setHighlighted(highlight);
  }
  bool forceAlignLabelAndAccessory() const override { return true; }

private:
  Dropdown m_dropdown;
};

}  // namespace Escher

#endif /* ESCHER_EXPRESSION_CELL_WITH_SUBLABEL_AND_DROPDOWN_H */
