#ifndef SHARED_TEXT_FIELD_WITH_EXTENSION_H
#define SHARED_TEXT_FIELD_WITH_EXTENSION_H

#include <escher/text_field.h>

namespace Shared {

class TextFieldWithExtension : public Escher::TextField {
 public:
  TextFieldWithExtension(
      size_t extensionLength, Responder* parentResponder, char* textBuffer,
      size_t textBufferSize,
      Escher::InputEventHandlerDelegate* inputEventHandlerDelegate,
      Escher::TextFieldDelegate* delegate = nullptr,
      KDGlyph::Format format = {})
      : TextField(parentResponder, textBuffer, textBufferSize,
                  inputEventHandlerDelegate, delegate, format),
        m_extensionLength(extensionLength) {}
  void setExtensionLength(size_t extensionLength) {
    m_extensionLength = extensionLength;
  }

 private:
  void willSetCursorLocation(const char** location) override;
  bool privateRemoveEndOfLine() override;
  void removeWholeText() override;
  bool removeTextBeforeExtension(bool whole);
  size_t m_extensionLength;
};

}  // namespace Shared

#endif
