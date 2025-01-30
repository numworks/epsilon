#ifndef CODE_SCRIPT_NAME_CELL_H
#define CODE_SCRIPT_NAME_CELL_H

#include <escher/even_odd_cell.h>
#include <escher/metric.h>
#include <escher/responder.h>
#include <escher/text_field.h>
#include <escher/text_field_delegate.h>

#include "script_store.h"

namespace Code {

class ScriptNameCell : public Escher::EvenOddCell, public Escher::Responder {
 public:
  ScriptNameCell(Escher::Responder* parentResponder = nullptr,
                 Escher::TextFieldDelegate* delegate = nullptr)
      : Escher::EvenOddCell(),
        Escher::Responder(parentResponder),
        m_textField(this, m_textBody, Escher::TextField::MaxBufferSize(),
                    delegate) {
    m_textBody[0] = 0;
  }

  Escher::TextField* textField() { return &m_textField; }
  Escher::Responder* responder() override {
    return m_textField.isEditing() ? this : nullptr;
  }
  const char* text() const override;
  // View
  KDSize minimalSizeForOptimalDisplay() const override;

 protected:
  // Responder
  void handleResponderChainEvent(ResponderChainEvent event) override;

 private:
  constexpr static KDCoordinate k_leftMargin =
      Escher::Metric::CommonLargeMargin;

  class ScriptNameTextField : public Escher::TextField {
   public:
    // '.' + "py"
    constexpr static size_t k_extensionLength =
        1 + ScriptStore::k_scriptExtensionLength;

    ScriptNameTextField(Responder* parentResponder, char* textBuffer,
                        size_t textBufferSize,
                        Escher::TextFieldDelegate* delegate = nullptr,
                        KDGlyph::Format format = {})
        : TextField(parentResponder, textBuffer, textBufferSize, delegate,
                    format) {}
    bool handleEvent(Ion::Events::Event event) override;

   private:
    void willSetCursorLocation(const char** location) override;
    bool privateRemoveEndOfLine() override;
    void removeWholeText() override;
    bool removeTextBeforeExtension(bool whole);
  };

  void updateSubviewsBackgroundAfterChangingState() override;

  // View
  int numberOfSubviews() const override { return 1; }
  Escher::View* subviewAtIndex(int index) override {
    assert(index == 0);
    return &m_textField;
  }
  void layoutSubviews(bool force = false) override;

  ScriptNameTextField m_textField;
  char m_textBody[Escher::TextField::MaxBufferSize()];
};

}  // namespace Code

#endif
