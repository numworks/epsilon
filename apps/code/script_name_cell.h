#ifndef CODE_SCRIPT_NAME_CELL_H
#define CODE_SCRIPT_NAME_CELL_H

#include <escher/even_odd_cell.h>
#include <escher/metric.h>
#include <escher/responder.h>
#include <escher/text_field_delegate.h>
#include <apps/shared/text_field_with_extension.h>
#include "script_store.h"

namespace Code {

class ScriptNameCell : public Escher::EvenOddCell, public Escher::Responder {
public:
  ScriptNameCell(Escher::Responder * parentResponder = nullptr, Escher::TextFieldDelegate * delegate = nullptr) :
    Escher::EvenOddCell(),
    Escher::Responder(parentResponder),
    m_textField(k_extensionLength, this, m_textBody, Escher::TextField::MaxBufferSize(), Escher::TextField::MaxBufferSize(), nullptr, delegate)
  {
    m_textBody[0] = 0;
  }

  Shared::TextFieldWithExtension * textField() { return &m_textField; }
  Escher::Responder * responder() override {
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
  bool canOpenStoreMenu() const override { return false; }

private:
  constexpr static size_t k_extensionLength = 1+ScriptStore::k_scriptExtensionLength; // '.' + "py"
  constexpr static KDCoordinate k_leftMargin = Escher::Metric::CommonLargeMargin;

  void updateSubviewsBackgroundAfterChangingState() override;

  // View
  int numberOfSubviews() const override { return 1; }
  Escher::View * subviewAtIndex(int index) override {
    assert(index == 0);
    return &m_textField;
  }
  void layoutSubviews(bool force = false) override;

  Shared::TextFieldWithExtension m_textField;
  char m_textBody[Escher::TextField::MaxBufferSize()];
};

}

#endif
