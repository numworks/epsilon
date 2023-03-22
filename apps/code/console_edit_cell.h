#ifndef CODE_EDIT_CELL_H
#define CODE_EDIT_CELL_H

#include <escher/highlight_cell.h>
#include <escher/pointer_text_view.h>
#include <escher/responder.h>
#include <escher/text_field.h>
#include <escher/text_field_delegate.h>

namespace Code {

class ConsoleEditCell : public Escher::HighlightCell, public Escher::Responder {
 public:
  ConsoleEditCell(
      Escher::Responder* parentResponder = nullptr,
      Escher::InputEventHandlerDelegate* inputEventHandlerDelegate = nullptr,
      Escher::TextFieldDelegate* delegate = nullptr);

  // View
  int numberOfSubviews() const override;
  Escher::View* subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;

  // Responder
  void didBecomeFirstResponder() override;

  /* HighlightCell */
  Escher::Responder* responder() override { return this; }

  // Edit cell
  void setEditing(bool isEditing);
  const char* text() const override { return m_textField.text(); }
  void setText(const char* text);
  bool insertText(const char* text);
  void setPrompt(const char* prompt);
  const char* promptText() const { return m_promptView.text(); }

 private:
  Escher::PointerTextView m_promptView;
  Escher::TextField m_textField;
};

}  // namespace Code

#endif
