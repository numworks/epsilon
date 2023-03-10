#include <escher/text_view.h>

namespace Escher {

void TextView::setBackgroundColor(KDColor backgroundColor) {
  if (m_backgroundColor != backgroundColor) {
    m_backgroundColor = backgroundColor;
    markRectAsDirty(bounds());
  }
}

void TextView::setTextColor(KDColor textColor) {
  if (m_textColor != textColor) {
    m_textColor = textColor;
    markRectAsDirty(bounds());
  }
}

void TextView::setAlignment(float horizontalAlignment,
                            float verticalAlignment) {
  m_horizontalAlignment = horizontalAlignment;
  m_verticalAlignment = verticalAlignment;
  markRectAsDirty(bounds());
}

void TextView::setFont(KDFont::Size font) {
  m_font = font;
  markRectAsDirty(bounds());
}

KDSize TextView::minimalSizeForOptimalDisplay() const {
  if (text() == nullptr) {
    return KDSizeZero;
  }
  return KDFont::Font(m_font)->stringSize(text());
}

void TextView::drawRect(KDContext* ctx, KDRect rect) const {
  if (text() == nullptr) {
    return;
  }
  ctx->fillRect(bounds(), m_backgroundColor);
  ctx->alignAndDrawString(text(), KDPointZero, bounds().size(),
                          m_horizontalAlignment, m_verticalAlignment, m_font,
                          m_textColor, m_backgroundColor);
}

void TextView::defaultInitialization(CellWidget::Type type) {
  InitializationParameters parameters =
      type == CellWidget::Type::Label
          ? k_labelInitializationParameters
          : (type == CellWidget::Type::SubLabel
                 ? k_subLabelInitializationParameters
                 : k_accessoryInitializationParameters);
  setFont(parameters.fontSize);
  setTextColor(parameters.textColor);
  setBackgroundColor(KDColorWhite);
  setAlignment(parameters.horizontalAlginment, parameters.verticalAlignment);
}

}  // namespace Escher
