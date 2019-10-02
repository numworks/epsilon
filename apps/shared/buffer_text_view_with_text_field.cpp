#include "buffer_text_view_with_text_field.h"
#include <escher/palette.h>

namespace Shared {

BufferTextViewWithTextField::BufferTextViewWithTextField(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, TextFieldDelegate * delegate, const KDFont * font) :
  View(),
  Responder(parentResponder),
  m_bufferTextView(font, 0.0f, 0.5f),
  m_textField(this, nullptr, TextField::maxBufferSize(), TextField::maxBufferSize(), inputEventHandlerDelegate, delegate, font, 0.0f, 0.5f)
{
}

KDSize BufferTextViewWithTextField::minimalSizeForOptimalDisplay() const {
  return m_bufferTextView.minimalSizeForOptimalDisplay();
}

void BufferTextViewWithTextField::setBufferText(const char * text) {
  m_bufferTextView.setText(text);
}

void BufferTextViewWithTextField::drawRect(KDContext * ctx, KDRect rect) const {
  KDRect textFieldRect = textFieldFrame();

  // Fill margins with white
  // Left margin
  ctx->fillRect(KDRect(0, 0, Metric::TitleBarExternHorizontalMargin, bounds().height()), KDColorWhite);
  ctx->fillRect(KDRect(bounds().width() - Metric::TitleBarExternHorizontalMargin, 0, Metric::TitleBarExternHorizontalMargin, bounds().height()), KDColorWhite);
  // Right margin
  ctx->fillRect(KDRect(bounds().width() - Metric::TitleBarExternHorizontalMargin, 0, Metric::TitleBarExternHorizontalMargin, bounds().height()), KDColorWhite);
  // Above the text field
  ctx->fillRect(KDRect(textFieldRect.x() - k_borderWidth, 0, textFieldRect.width() + 2*k_borderWidth, bounds().height()), KDColorWhite);
  // Under the text field
  ctx->fillRect(KDRect(textFieldRect.x() - k_borderWidth, textFieldRect.bottom() + k_borderWidth, textFieldRect.width() + 2*k_borderWidth, bounds().height()), KDColorWhite);

  // Draw the text field border
  KDRect borderRect = KDRect(textFieldRect.x()-k_borderWidth, textFieldRect.y()-k_borderWidth, textFieldRect.width()+2*k_borderWidth, textFieldRect.height()+2*k_borderWidth);
  ctx->strokeRect(borderRect, Palette::GreyMiddle);
}

void BufferTextViewWithTextField::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(&m_textField);
  m_textField.setEditing(true);
  markRectAsDirty(bounds());
}

View * BufferTextViewWithTextField::subviewAtIndex(int index) {
  assert(index >= 0 && index < numberOfSubviews());
  View * views[] = {&m_bufferTextView, &m_textField};
  return views[index];
}

void BufferTextViewWithTextField::layoutSubviews(bool force) {
  m_bufferTextView.setFrame(KDRect(Metric::TitleBarExternHorizontalMargin, 0, k_bufferTextWidth, bounds().height()), force);
  m_textField.setFrame(textFieldFrame(), force);
}

KDRect BufferTextViewWithTextField::textFieldFrame() const {
  return KDRect(Metric::TitleBarExternHorizontalMargin + k_bufferTextWidth + k_borderWidth, k_textFieldVerticalMargin + k_borderWidth, bounds().width() - 2 * Metric::TitleBarExternHorizontalMargin - k_bufferTextWidth - 2 * k_borderWidth, bounds().height() - 2 * k_textFieldVerticalMargin - 2 * k_borderWidth);
}

}
