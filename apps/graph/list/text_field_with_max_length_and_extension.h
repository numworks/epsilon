#ifndef GRAPH_TEXT_FIELD_WITH_MAX_LENGTH_AND_EXTENSION_H
#define GRAPH_TEXT_FIELD_WITH_MAX_LENGTH_AND_EXTENSION_H

#include <apps/shared/text_field_with_extension.h>

namespace Graph {

class TextFieldWithMaxLengthAndExtension
    : public Shared::TextFieldWithExtension {
 public:
  TextFieldWithMaxLengthAndExtension(
      size_t extensionLength, Escher::Responder* parentResponder,
      char* textBuffer, size_t textBufferSize,
      Escher::InputEventHandlerDelegate* inputEventHandlerDelegate,
      Escher::TextFieldDelegate* delegate = nullptr,
      KDFont::Size size = KDFont::Size::Large,
      float horizontalAlignment = KDGlyph::k_alignLeft,
      float verticalAlignment = KDGlyph::k_alignCenter,
      KDColor textColor = KDColorBlack, KDColor backgroundColor = KDColorWhite)
      : TextFieldWithExtension(extensionLength, parentResponder, textBuffer,
                               textBufferSize, inputEventHandlerDelegate,
                               delegate, size, horizontalAlignment,
                               verticalAlignment, textColor, backgroundColor) {}
};

}  // namespace Graph

#endif
