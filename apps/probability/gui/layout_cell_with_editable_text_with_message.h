#ifndef APPS_PROBABILITY_GUI_LAYOUT_CELL_WITH_EDITABLE_TEXT_WITH_MESSAGE_H
#define APPS_PROBABILITY_GUI_LAYOUT_CELL_WITH_EDITABLE_TEXT_WITH_MESSAGE_H

#include <escher/chained_text_field_delegate.h>
#include <escher/responder.h>
#include <escher/text_field.h>
#include <poincare/print_float.h>

#include "layout_cell_with_sub_message.h"

namespace Probability {

class LayoutCellWithEditableTextWithMessage : public LayoutCellWithSubMessage,
                                              public Escher::Responder,
                                              public Escher::ChainedTextFieldDelegate {
public:
  LayoutCellWithEditableTextWithMessage(
      Escher::Responder * parent = nullptr,
      Escher::InputEventHandlerDelegate * inputEventHandlerDelegate = nullptr,
      Escher::TextFieldDelegate * textFieldDelegate = nullptr);

  void didBecomeFirstResponder() override;
  Responder * responder() override { return this; }

  const View * accessoryView() const override { return &m_textField; }
  Escher::TextField * textField() { return &m_textField; }

  bool giveAccessoryAllWidth() const override { return true; }
  bool alignLabelAndAccessory() const override { return true; }

  void setAccessoryText(const char * text);
  void setHighlighted(bool highlight) override;
  bool hideSublabel() override { return singleRowMode() && m_textField.isEditing(); }

  void textFieldDidStartEditing(Escher::TextField * textField) override;
  bool textFieldDidFinishEditing(Escher::TextField * textField,
                                 const char * text,
                                 Ion::Events::Event event) override;
  bool textFieldDidAbortEditing(Escher::TextField * textField) override;

  void setDelegates(Escher::InputEventHandlerDelegate * inputEventHandlerDelegate,
                            Escher::TextFieldDelegate * textFieldDelegate);

private:
  char m_textBody[Poincare::PrintFloat::k_maxFloatCharSize];
  Escher::TextField m_textField;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_GUI_LAYOUT_CELL_WITH_EDITABLE_TEXT_WITH_MESSAGE_H */
