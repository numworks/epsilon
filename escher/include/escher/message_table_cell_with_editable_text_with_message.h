#ifndef ESCHER_MESSAGE_TABLE_CELL_WITH_EDITABLE_TEXT_WITH_MESSAGE_H
#define ESCHER_MESSAGE_TABLE_CELL_WITH_EDITABLE_TEXT_WITH_MESSAGE_H

#include <escher/chained_text_field_delegate.h>
#include <escher/message_table_cell_with_editable_text.h>

namespace Escher {

class MessageTableCellWithEditableTextWithMessage : public MessageTableCellWithEditableText,
                                                    public ChainedTextFieldDelegate {
public:
  MessageTableCellWithEditableTextWithMessage(
      Responder * parentResponder = nullptr,
      InputEventHandlerDelegate * inputEventHandlerDelegate = nullptr,
      TextFieldDelegate * textFieldDelegate = nullptr,
      I18n::Message message = (I18n::Message)0);
  const View * subLabelView() const override {
    return m_subLabelView.text()[0] == 0 ? nullptr : &m_subLabelView;
  }
  void setHighlighted(bool highlight) override;
  void setSubLabelMessage(I18n::Message textBody);
  bool shouldHideSublabel() override { return singleRowMode() && textField()->isEditing(); }

  // ChainedTextFieldDelegate
  void textFieldDidStartEditing(AbstractTextField * textField) override;
  bool textFieldDidFinishEditing(AbstractTextField * textField,
                                 const char * text,
                                 Ion::Events::Event event) override;
  bool textFieldDidAbortEditing(AbstractTextField * textField) override;

  void setDelegates(InputEventHandlerDelegate * inputEventHandlerDelegate,
                    TextFieldDelegate * textFieldDelegate) override;

protected:
  MessageTextView m_subLabelView;
};

}  // namespace Escher

#endif
