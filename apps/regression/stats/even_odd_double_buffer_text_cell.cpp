#include "even_odd_double_buffer_text_cell.h"

#include <assert.h>

#include "escher/metric.h"

using namespace Escher;

namespace Regression {

EvenOddDoubleBufferTextCell::EvenOddDoubleBufferTextCell(
    Responder* parentResponder, KDGlyph::Format format)
    : EvenOddCell(),
      Responder(parentResponder),
      m_firstTextSelected(true),
      m_firstBufferTextView(format),
      m_secondBufferTextView(format) {}

const char* EvenOddDoubleBufferTextCell::text() const {
  if (m_firstTextSelected) {
    return firstText();
  } else {
    return secondText();
  }
}

const char* EvenOddDoubleBufferTextCell::firstText() const {
  return m_firstBufferTextView.text();
}

const char* EvenOddDoubleBufferTextCell::secondText() const {
  return m_secondBufferTextView.text();
}

bool EvenOddDoubleBufferTextCell::firstTextSelected() {
  return m_firstTextSelected;
}

void EvenOddDoubleBufferTextCell::selectFirstText(bool selectFirstText) {
  m_firstTextSelected = selectFirstText;
  setHighlighted(isHighlighted());
}

void EvenOddDoubleBufferTextCell::reloadCell() {
  m_firstBufferTextView.reloadCell();
  m_secondBufferTextView.reloadCell();
}

void EvenOddDoubleBufferTextCell::setHighlighted(bool highlight) {
  HighlightCell::setHighlighted(highlight);
  m_firstBufferTextView.setHighlighted(highlight && m_firstTextSelected);
  m_secondBufferTextView.setHighlighted(highlight && !m_firstTextSelected);
}

void EvenOddDoubleBufferTextCell::setEven(bool even) {
  m_firstBufferTextView.setEven(even);
  m_secondBufferTextView.setEven(even);
  reloadCell();
}

void EvenOddDoubleBufferTextCell::setFirstText(const char* textContent) {
  m_firstBufferTextView.setText(textContent);
}

void EvenOddDoubleBufferTextCell::setSecondText(const char* textContent) {
  m_secondBufferTextView.setText(textContent);
}

void EvenOddDoubleBufferTextCell::setTextColor(KDColor textColor) {
  m_firstBufferTextView.setTextColor(textColor);
  m_secondBufferTextView.setTextColor(textColor);
}

int EvenOddDoubleBufferTextCell::numberOfSubviews() const { return 2; }

View* EvenOddDoubleBufferTextCell::subviewAtIndex(int index) {
  assert(index == 0 || index == 1);
  if (index == 0) {
    return &m_firstBufferTextView;
  }
  return &m_secondBufferTextView;
}

void EvenOddDoubleBufferTextCell::layoutSubviews(bool force) {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  setChildFrame(&m_firstBufferTextView, KDRect(0, 0, width / 2, height), force);
  setChildFrame(&m_secondBufferTextView,
                KDRect(width / 2, 0, width - width / 2, height), force);
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

}  // namespace Regression
