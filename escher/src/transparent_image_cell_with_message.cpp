#include <assert.h>
#include <escher/palette.h>
#include <escher/transparent_image_cell_with_message.h>

namespace Escher {

TransparentImageCellWithMessage::TransparentImageCellWithMessage()
    : TableCell(), m_messageTextView(static_cast<I18n::Message>(0)) {
  m_icon.setBackgroundColor(KDColorWhite);
}

void TransparentImageCellWithMessage::setHighlighted(bool highlight) {
  HighlightCell::setHighlighted(highlight);
  KDColor backgroundColor = highlight ? Palette::Select : KDColorWhite;
  m_messageTextView.setBackgroundColor(backgroundColor);
  m_icon.setBackgroundColor(backgroundColor);
}

void TransparentImageCellWithMessage::setMessage(I18n::Message text) {
  m_messageTextView.setMessage(text);
}

void TransparentImageCellWithMessage::setImage(const Image* image) {
  m_icon.setImage(image);
}

}  // namespace Escher
