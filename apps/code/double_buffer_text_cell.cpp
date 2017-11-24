#include "double_buffer_text_cell.h"
#include <escher/palette.h>
#include <assert.h>

DoubleBufferTextCell::DoubleBufferTextCell(KDText::FontSize size, float horizontalAlignment, float verticalAlignment) :
  TableCell(TableCell::Layout::Vertical),
  m_firstBufferTextView(size, horizontalAlignment, verticalAlignment),
  m_secondBufferTextView(size, horizontalAlignment, verticalAlignment)
{
}

const char * DoubleBufferTextCell::firstText() {
  return m_firstBufferTextView.text();
}

const char * DoubleBufferTextCell::secondText() {
  return m_secondBufferTextView.text();
}

void DoubleBufferTextCell::setFirstText(const char * textContent) {
  m_firstBufferTextView.setText(textContent);
}

void DoubleBufferTextCell::setSecondText(const char * textContent) {
  m_secondBufferTextView.setText(textContent);
}

void DoubleBufferTextCell::setFirstTextColor(KDColor textColor) {
  m_firstBufferTextView.setTextColor(textColor);
}

void DoubleBufferTextCell::setSecondTextColor(KDColor textColor) {
  m_secondBufferTextView.setTextColor(textColor);
}

void DoubleBufferTextCell::setHighlighted(bool highlight) {
  HighlightCell::setHighlighted(highlight);
  KDColor background = isHighlighted() ? Palette::Select : KDColorWhite;
  m_firstBufferTextView.setBackgroundColor(background);
  m_secondBufferTextView.setBackgroundColor(background);
}

View * DoubleBufferTextCell::subviewAtIndex(int index) {
  assert(index >= 0 && index <= 1);
  if (index == 0) {
    return &m_firstBufferTextView;
  }
  return &m_secondBufferTextView;
}

void DoubleBufferTextCell::layoutSubviews() {
  TableCell::layoutSubviews();
}
