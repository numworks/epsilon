#ifndef CODE_SCRIPT_NAME_CELL_H
#define CODE_SCRIPT_NAME_CELL_H

#include <escher/even_odd_cell.h>
#include <escher/metric.h>
#include <escher/responder.h>
#include <escher/text_field_delegate.h>
#include <apps/shared/text_field_with_extension.h>
#include "script_store.h"

namespace Code {

class ScriptNameCell : public EvenOddCell, public Responder {
public:
  ScriptNameCell(Responder * parentResponder = nullptr, TextFieldDelegate * delegate = nullptr) :
    EvenOddCell(),
    Responder(parentResponder),
    m_textField(k_extensionLength, this, m_textBody, TextField::maxBufferSize(), TextField::maxBufferSize(), nullptr, delegate)
  {
    m_textBody[0] = 0;
  }

  Shared::TextFieldWithExtension * textField() { return &m_textField; }

  // EvenOddCell
  void setEven(bool even) override;
  // HighlightCell
  void setHighlighted(bool highlight) override;
  Responder * responder() override {
    if (m_textField.isEditing()) {
      return this;
    }
    return nullptr;
  }
  const char * text() const override;
  // View
  KDSize minimalSizeForOptimalDisplay() const override;
  // Responder
  void didBecomeFirstResponder() override;

private:
  constexpr static size_t k_extensionLength = 1+ScriptStore::k_scriptExtensionLength; // '.' + "py"
  constexpr static KDCoordinate k_leftMargin = Metric::CommonLargeMargin;

  // View
  int numberOfSubviews() const override { return 1; }
  View * subviewAtIndex(int index) override {
    assert(index == 0);
    return &m_textField;
  }
  void layoutSubviews(bool force = false) override;

  Shared::TextFieldWithExtension m_textField;
  char m_textBody[TextField::maxBufferSize()];
};

}

#endif
