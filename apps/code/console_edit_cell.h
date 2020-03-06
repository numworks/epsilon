#ifndef CODE_EDIT_CELL_H
#define CODE_EDIT_CELL_H

#include <escher/responder.h>
#include <escher/highlight_cell.h>
#include <escher/text_field.h>
#include <escher/text_field_delegate.h>
#include <escher/pointer_text_view.h>

namespace Code {

class ConsoleEditCell : public HighlightCell, public Responder {
public:
  ConsoleEditCell(Responder * parentResponder = nullptr, InputEventHandlerDelegate * inputEventHandlerDelegate = nullptr, TextFieldDelegate * delegate = nullptr);

  // View
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;

  // Responder
  void didBecomeFirstResponder() override;

  /* HighlightCell */
  Responder * responder() override {
    return this;
  }

  // Edit cell
  void setEditing(bool isEditing);
  const char * text() const override { return m_textField.text(); }
  void setText(const char * text);
  bool insertText(const char * text);
  void setPrompt(const char * prompt);
  const char * promptText() const { return m_promptView.text(); }
  void clearAndReduceSize();
  const char * shiftCurrentTextAndClear();
private:
  PointerTextView m_promptView;
  TextField m_textField;
};

}

#endif
