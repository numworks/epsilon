#include <escher/raw_color_message_text_view.h>

namespace Escher {

RawColorMessageTextView::RawColorMessageTextView(
    I18n::Message message, const TextFormatWithoutColors& format,
    const TextColors& colors, KDCoordinate lineSpacing)
    : MessageTextView(
          message,
          TextFormat{.style = TextStyle{.font = format.font},
                     .horizontalAlignment = format.horizontalAlignment,
                     .verticalAlignment = format.verticalAlignment},
          lineSpacing),
      m_rawColors{.glyphColor = colors.glyphColor,
                  .backgroundColor = colors.backgroundColor} {}

void RawColorMessageTextView::setTextColor(KDColor color) {
  if (m_rawColors.glyphColor != color) {
    m_rawColors.glyphColor = color;
    markWholeFrameAsDirty();
  }
}

void RawColorMessageTextView::setBackgroundColor(KDColor color) {
  if (m_rawColors.backgroundColor != color) {
    m_rawColors.backgroundColor = color;
    markWholeFrameAsDirty();
  }
}

}  // namespace Escher
