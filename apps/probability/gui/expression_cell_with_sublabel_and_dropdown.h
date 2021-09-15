#ifndef PROBABILITY_GUI_EXPRESSION_CELL_WITH_SUBLABEL_AND_DROPDOWN_H
#define PROBABILITY_GUI_EXPRESSION_CELL_WITH_SUBLABEL_AND_DROPDOWN_H

#include <escher/container.h>
#include <escher/expression_table_cell_with_message.h>
#include <escher/message_table_cell_with_message.h>
#include <escher/responder.h>

#include "dropdown_view.h"

namespace Probability {

class ExpressionCellWithSublabelAndDropdown : public Escher::ExpressionTableCellWithMessage {
public:
  ExpressionCellWithSublabelAndDropdown(Escher::Responder * parentResponder,
                                        Escher::ListViewDataSource * listDataSource,
                                        DropdownCallback * callback = nullptr) :
        ExpressionTableCellWithMessage(parentResponder),
        m_dropdown(this, listDataSource, callback) {}
  const Escher::View * accessoryView() const override { return &m_dropdown; }
  Dropdown * dropdown() { return &m_dropdown; }
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
    Escher::ExpressionTableCellWithMessage::setHighlighted(highlight);
  }
  bool shouldAlignLabelAndAccessory() const override { return true; }

private:
  Dropdown m_dropdown;
};

}  // namespace Probability

#endif /* PROBABILITY_GUI_EXPRESSION_CELL_WITH_SUBLABEL_AND_DROPDOWN_H */
