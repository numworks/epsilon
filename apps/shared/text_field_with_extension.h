#ifndef SHARED_TEXT_FIELD_WITH_EXTENSION_H
#define SHARED_TEXT_FIELD_WITH_EXTENSION_H

#include <escher/text_field.h>

namespace Shared {

class TextFieldWithExtension : public TextField {
public:
  TextFieldWithExtension(size_t extensionLength,
      Responder * parentResponder,
      char * textBuffer,
      char * draftTextBuffer,
      size_t textBufferSize,
      ::InputEventHandlerDelegate * inputEventHandlerDelegate,
      ::TextFieldDelegate * delegate = nullptr,
      bool hasTwoBuffers = true,
      const KDFont * size = KDFont::LargeFont,
      float horizontalAlignment = 0.0f,
      float verticalAlignment = 0.5f,
      KDColor textColor = KDColorBlack,
      KDColor backgroundColor = KDColorWhite) :
    TextField(parentResponder, textBuffer, draftTextBuffer, textBufferSize, inputEventHandlerDelegate, delegate, hasTwoBuffers, size, horizontalAlignment, verticalAlignment, textColor, backgroundColor),
    m_extensionLength(extensionLength)
  {}
private:
  void willSetCursorTextLocation(const char * * location) override;
  bool privateRemoveEndOfLine() override;
  void removeWholeText() override;
  bool removeTextBeforeExtension(bool whole);
  size_t m_extensionLength;
};

}

#endif
