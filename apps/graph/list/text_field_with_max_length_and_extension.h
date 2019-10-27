#ifndef GRAPH_TEXT_FIELD_WITH_MAX_LENGTH_AND_EXTENSION_H
#define GRAPH_TEXT_FIELD_WITH_MAX_LENGTH_AND_EXTENSION_H

#include <apps/shared/text_field_with_extension.h>

namespace Graph {

class TextFieldWithMaxLengthAndExtension : public Shared::TextFieldWithExtension {
public:
  TextFieldWithMaxLengthAndExtension(size_t extensionLength,
      Responder * parentResponder,
      char * textBuffer,
      size_t textBufferSize,
      size_t draftTextBufferSize,
      ::InputEventHandlerDelegate * inputEventHandlerDelegate,
      ::TextFieldDelegate * delegate = nullptr,
      const KDFont * size = KDFont::LargeFont,
      float horizontalAlignment = 0.0f,
      float verticalAlignment = 0.5f,
      KDColor textColor = KDColorBlack,
      KDColor backgroundColor = KDColorWhite) :
    TextFieldWithExtension(extensionLength, parentResponder, textBuffer, textBufferSize, draftTextBufferSize, inputEventHandlerDelegate, delegate, size, horizontalAlignment, verticalAlignment, textColor, backgroundColor) {}
  void setDraftTextBufferSize(size_t size) { m_contentView.setDraftTextBufferSize(size); }
};

}

#endif
