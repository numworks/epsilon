#ifndef SOLVER_EXPRESSION_CELL_WITH_SUBLABEL_AND_DROPDOWN_H
#define SOLVER_EXPRESSION_CELL_WITH_SUBLABEL_AND_DROPDOWN_H

#include <apps/i18n.h>
#include <escher/button_delegate.h>
#include <escher/container.h>
#include <escher/dropdown_view.h>
#include <escher/message_table_cell_with_message.h>
#include <escher/responder.h>

namespace Solver {

class MessageTableCellWithSublabelAndDropdown : public Escher::MessageTableCellWithMessage, public Escher::Responder {
public:
  MessageTableCellWithSublabelAndDropdown(Escher::Responder * parentResponder,
                                        Escher::ListViewDataSource * listDataSource,
                                        Escher::DropdownCallback * callback = nullptr) :
        MessageTableCellWithMessage(I18n::Message::Ok),
        Escher::Responder(parentResponder),
        m_dropdown(this, listDataSource, callback) {}
  const Escher::View * accessoryView() const override { return &m_dropdown; }
  Escher::Dropdown * dropdown() { return &m_dropdown; }
  Escher::Responder * responder() override { return this; }
  void didBecomeFirstResponder() override {
    Escher::Container::activeApp()->setFirstResponder(&m_dropdown);
  }
  bool shouldAlignSublabelRight() const override { return false; }
  void reload() {
    m_dropdown.reloadAllCells();
    markRectAsDirty(bounds());
    layoutSubviews();
  }
  void setHighlighted(bool highlight) override {
    m_dropdown.setHighlighted(highlight);
    Escher::MessageTableCellWithMessage::setHighlighted(highlight);
  }
  bool shouldAlignLabelAndAccessory() const override { return true; }

private:
  Escher::Dropdown m_dropdown;
};

}  // namespace Solver

#endif /* SOLVER_EXPRESSION_CELL_WITH_SUBLABEL_AND_DROPDOWN_H */
