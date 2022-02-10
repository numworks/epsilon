#include <escher/transparent_image_with_message_cell.h>
#include <escher/palette.h>
#include <assert.h>

namespace Escher {

TransparentImageWithMessageCell::TransparentImageWithMessageCell() :
    TableCell(),
    m_messageTextView(KDFont::LargeFont, static_cast<I18n::Message>(0), 0, 0.5, KDColorBlack, KDColorWhite) {
   m_icon.setBackgroundColor(KDColorWhite);
}

void TransparentImageWithMessageCell::setHighlighted(bool highlight) {
  HighlightCell::setHighlighted(highlight);
  KDColor backgroundColor = highlight? Palette::Select : KDColorWhite;
  m_messageTextView.setBackgroundColor(backgroundColor);
  m_icon.setBackgroundColor(backgroundColor);
}

void TransparentImageWithMessageCell::setMessage(I18n::Message text) {
  m_messageTextView.setMessage(text);
}

void TransparentImageWithMessageCell::setImage(const Image * image) {
  m_icon.setImage(image);
}

}
