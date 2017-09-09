#include "even_odd_double_buffer_text_cell.h"
#include <assert.h>

EvenOddDoubleBufferTextCell::EvenOddDoubleBufferTextCell(Responder * parentResponder) :
  EvenOddCell(),
  Responder(parentResponder),
  m_firstTextSelected(true),
  m_firstBufferTextView(KDText::FontSize::Small),
  m_secondBufferTextView(KDText::FontSize::Small)
{
}

const char * EvenOddDoubleBufferTextCell::firstText() {
  return m_firstBufferTextView.text();
}

const char * EvenOddDoubleBufferTextCell::secondText() {
  return m_secondBufferTextView.text();
}

bool EvenOddDoubleBufferTextCell::firstTextSelected() {
  return m_firstTextSelected;
}

void EvenOddDoubleBufferTextCell::selectFirstText(bool selectFirstText) {
  m_firstTextSelected = selectFirstText;
  m_firstBufferTextView.setHighlighted(selectFirstText);
  m_secondBufferTextView.setHighlighted(!selectFirstText);
  reloadCell();
}

void EvenOddDoubleBufferTextCell::reloadCell() {
  m_firstBufferTextView.reloadCell();
  m_secondBufferTextView.reloadCell();
}

void EvenOddDoubleBufferTextCell::setHighlighted(bool highlight) {
  m_firstBufferTextView.setHighlighted(false);
  m_secondBufferTextView.setHighlighted(false);
  HighlightCell::setHighlighted(highlight);
  if (isHighlighted()) {
    if (m_firstTextSelected) {
      m_firstBufferTextView.setHighlighted(true);
    } else {
      m_secondBufferTextView.setHighlighted(false);
    }
  }
  reloadCell();
}

void EvenOddDoubleBufferTextCell::setEven(bool even) {
  m_firstBufferTextView.setEven(even);
  m_secondBufferTextView.setEven(even);
  reloadCell();
}

void EvenOddDoubleBufferTextCell::setFirstText(const char * textContent) {
  m_firstBufferTextView.setText(textContent);
}

void EvenOddDoubleBufferTextCell::setSecondText(const char * textContent) {
  m_secondBufferTextView.setText(textContent);
}

void EvenOddDoubleBufferTextCell::setTextColor(KDColor textColor) {
  m_firstBufferTextView.setTextColor(textColor);
  m_secondBufferTextView.setTextColor(textColor);
}

int EvenOddDoubleBufferTextCell::numberOfSubviews() const {
  return 2;
}

View * EvenOddDoubleBufferTextCell::subviewAtIndex(int index) {
  assert(index == 0 || index == 1);
  if (index == 0) {
    return &m_firstBufferTextView;
  }
  return &m_secondBufferTextView;
}

void EvenOddDoubleBufferTextCell::layoutSubviews() {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  m_firstBufferTextView.setFrame(KDRect(0, 0, width/2, height));
  m_secondBufferTextView.setFrame(KDRect(width/2, 0, width/2, height));
}

bool EvenOddDoubleBufferTextCell::handleEvent(Ion::Events::Event event) {
  if (m_firstTextSelected && event == Ion::Events::Right) {
    selectFirstText(false);
    return true;
  }
  if (!m_firstTextSelected && event == Ion::Events::Left) {
    selectFirstText(true);
    return true;
  }
  return false;
}

