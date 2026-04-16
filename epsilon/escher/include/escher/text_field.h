#pragma once

#include "abstract_text_field.h"
#include "text_format.h"

namespace Escher {

class TextField : public AbstractTextField {
 public:
  TextField(Responder* parentResponder, char* textBuffer, size_t textBufferSize,
            TextFieldDelegate* delegate = nullptr,
            const Escher::TextFormat& format = {})
      : AbstractTextField(parentResponder, &m_contentView, delegate),
        m_contentView(textBuffer, textBufferSize, format) {}

 protected:
  const AbstractTextField::ContentView* nonEditableContentView() const {
    return &m_contentView;
  }

  AbstractTextField::ContentView m_contentView;
};

}  // namespace Escher
