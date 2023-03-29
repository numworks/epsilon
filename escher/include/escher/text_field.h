#ifndef ESCHER_TEXT_FIELD_H
#define ESCHER_TEXT_FIELD_H

#include "abstract_text_field.h"

namespace Escher {

class TextField : public AbstractTextField {
 public:
  // TODO: Initalize with KDGlyph::Format
  TextField(Responder* parentResponder, char* textBuffer, size_t textBufferSize,
            InputEventHandlerDelegate* inputEventHandlerDelegate,
            TextFieldDelegate* delegate = nullptr, KDGlyph::Format format = {})
      : AbstractTextField(parentResponder, &m_contentView,
                          inputEventHandlerDelegate, delegate),
        m_contentView(textBuffer, textBufferSize, format) {}

 protected:
  const AbstractTextField::ContentView* nonEditableContentView() const {
    return &m_contentView;
  }

  AbstractTextField::ContentView m_contentView;
};

}  // namespace Escher

#endif
