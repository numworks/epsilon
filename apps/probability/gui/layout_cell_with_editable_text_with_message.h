#ifndef APPS_PROBABILITY_GUI_LAYOUT_CELL_WITH_EDITABLE_TEXT_WITH_MESSAGE_H
#define APPS_PROBABILITY_GUI_LAYOUT_CELL_WITH_EDITABLE_TEXT_WITH_MESSAGE_H

#include <escher/expression_view.h>
#include <escher/message_text_view.h>
#include <escher/responder.h>
#include <escher/table_cell.h>
#include <escher/text_field.h>
#include <poincare/print_float.h>

namespace Probability {

class LayoutCellWithEditableTextWithMessage : public Escher::TableCell, public Escher::Responder {
public:
  LayoutCellWithEditableTextWithMessage(
      Escher::Responder * parent,
      Escher::InputEventHandlerDelegate * inputEventHandlerDelegate,
      Escher::TextFieldDelegate * textFieldDelegate);

  void didBecomeFirstResponder() override;
  Responder * responder() override { return this; }

  const View * accessoryView() const override { return &m_textField; }
  const View * labelView() const override { return &m_expressionView; }
  const View * subLabelView() const override { return &m_messageTextView; }
  Escher::TextField * textField() { return &m_textField; }

  bool giveAccessoryAllWidth() const override { return true; }
  bool alignLabelAndAccessory() const override { return true; }

  void setAccessoryText(const char * text);
  void setLayout(Poincare::Layout layout);
  void setSubLabelMessage(I18n::Message message);
  void setHighlighted(bool highlight) override;

private:
  char m_textBody[Poincare::PrintFloat::k_maxFloatCharSize];
  Escher::TextField m_textField;
  Escher::MessageTextView m_messageTextView;
  Escher::ExpressionView m_expressionView;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_GUI_LAYOUT_CELL_WITH_EDITABLE_TEXT_WITH_MESSAGE_H */
