#include "buffer_text_view_with_text_field.h"

namespace Shared {

BufferTextViewWithTextField::BufferTextViewWithTextField(Responder * parentResponder, TextFieldDelegate * delegate, KDText::FontSize size) :
  View(),
  Responder(parentResponder),
  m_bufferTextView(size, 0.0f, 0.5f),
  m_textField(this, m_textFieldBuffer, m_textFieldBuffer, k_textFieldBufferSize, delegate, false, size, 0.0f, 0.5f),
  m_textFieldBuffer{}
{
  m_bufferTextView.setText("TODO");
}

KDSize BufferTextViewWithTextField::minimalSizeForOptimalDisplay() const {
  return m_bufferTextView.minimalSizeForOptimalDisplay();
}

void BufferTextViewWithTextField::setBufferText(const char * text) {
  m_bufferTextView.setText(text);
}

void BufferTextViewWithTextField::didBecomeFirstResponder() {
  app()->setFirstResponder(&m_textField);
  m_textField.setEditing(true, true);
  markRectAsDirty(bounds());
}

View * BufferTextViewWithTextField::subviewAtIndex(int index) {
  assert(index >= 0 && index < numberOfSubviews());
  View * views[] = {&m_bufferTextView, &m_textField};
  return views[index];
}

void BufferTextViewWithTextField::layoutSubviews() {
  m_bufferTextView.setFrame(KDRect(0, 0, k_height, k_bufferTextWidth));
  m_textField.setFrame(KDRect(k_bufferTextWidth, 0, bounds().width() - k_bufferTextWidth, k_height));
}

}
