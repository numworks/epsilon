#ifndef SETTINGS_MESSAGE_TABLE_CELL_WITH_EDITABLE_TEXT_WITH_SEPARATOR_H
#define SETTINGS_MESSAGE_TABLE_CELL_WITH_EDITABLE_TEXT_WITH_SEPARATOR_H

#include <escher.h>

namespace Settings {

class MessageTableCellWithEditableTextWithSeparator : public HighlightCell {
public:
  MessageTableCellWithEditableTextWithSeparator(Responder * parentResponder = nullptr, InputEventHandlerDelegate * inputEventHandlerDelegate = nullptr, TextFieldDelegate * textFieldDelegate = nullptr, I18n::Message message = (I18n::Message)0);
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void setHighlighted(bool highlight) override;
  void reloadCell() override { m_cell.reloadCell(); }
  Responder * responder() override { return m_cell.responder(); }
  const char * text() const override { return m_cell.text(); }
  Poincare::Layout layout() const override{ return m_cell.layout(); }
  MessageTableCellWithEditableText * messageTableCellWithEditableText() { return &m_cell; }
  constexpr static KDCoordinate k_margin = 10;
private:
  constexpr static KDCoordinate k_separatorThickness = Metric::CellSeparatorThickness;
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  MessageTableCellWithEditableText m_cell;
};

}

#endif
