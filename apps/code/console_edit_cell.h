#ifndef CODE_EDIT_CELL_H
#define CODE_EDIT_CELL_H

#include <escher/responder.h>
#include <escher/highlight_cell.h>
#include <escher/text_field.h>
#include <escher/text_field_delegate.h>
#include <escher/message_text_view.h>

namespace Code {

class ConsoleEditCell : public HighlightCell, public Responder {
public:
  ConsoleEditCell(Responder * parentResponder = nullptr, TextFieldDelegate * delegate = nullptr);

  // View
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;

  // Responder
  void didBecomeFirstResponder() override;

  /* HighlightCell */
  Responder * responder() override {
    return this;
  }

  // Edit cell
  void setEditing(bool isEditing, bool reinitDraftBuffer = false);
  void setText(const char * text);
  bool insertText(const char * text);

private:
  char m_textBuffer[TextField::maxBufferSize()];
  MessageTextView m_promptView;
  TextField m_textField;
};

}

#endif
