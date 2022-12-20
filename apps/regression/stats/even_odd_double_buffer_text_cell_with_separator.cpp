#include "even_odd_double_buffer_text_cell_with_separator.h"
#include "escher/metric.h"
#include <assert.h>

using namespace Escher;

namespace Regression {

EvenOddDoubleBufferTextCellWithSeparator::EvenOddDoubleBufferTextCellWithSeparator(Responder * parentResponder, float horizontalAlignment, float verticalAlignment) :
  EvenOddCell(),
  Responder(parentResponder),
  m_firstTextSelected(true),
  m_firstBufferTextView(KDFont::Size::Small, horizontalAlignment, verticalAlignment),
  m_secondBufferTextView(KDFont::Size::Small, horizontalAlignment, verticalAlignment)
{
}

const char * EvenOddDoubleBufferTextCellWithSeparator::text() const {
  if (m_firstTextSelected) {
    return firstText();
  } else {
    return secondText();
  }
}

const char * EvenOddDoubleBufferTextCellWithSeparator::firstText() const {
  return m_firstBufferTextView.text();
}

const char * EvenOddDoubleBufferTextCellWithSeparator::secondText() const {
  return m_secondBufferTextView.text();
}

bool EvenOddDoubleBufferTextCellWithSeparator::firstTextSelected() {
  return m_firstTextSelected;
}

void EvenOddDoubleBufferTextCellWithSeparator::selectFirstText(bool selectFirstText) {
  m_firstTextSelected = selectFirstText;
  setHighlighted(isHighlighted());
}

void EvenOddDoubleBufferTextCellWithSeparator::reloadCell() {
  m_firstBufferTextView.reloadCell();
  m_secondBufferTextView.reloadCell();
}

void EvenOddDoubleBufferTextCellWithSeparator::setHighlighted(bool highlight) {
  HighlightCell::setHighlighted(highlight);
  m_firstBufferTextView.setHighlighted(highlight && m_firstTextSelected);
  m_secondBufferTextView.setHighlighted(highlight && !m_firstTextSelected);
}

void EvenOddDoubleBufferTextCellWithSeparator::setEven(bool even) {
  m_firstBufferTextView.setEven(even);
  m_secondBufferTextView.setEven(even);
  reloadCell();
}

void EvenOddDoubleBufferTextCellWithSeparator::setFirstText(const char * textContent) {
  m_firstBufferTextView.setText(textContent);
}

void EvenOddDoubleBufferTextCellWithSeparator::setSecondText(const char * textContent) {
  m_secondBufferTextView.setText(textContent);
}

void EvenOddDoubleBufferTextCellWithSeparator::setTextColor(KDColor textColor) {
  m_firstBufferTextView.setTextColor(textColor);
  m_secondBufferTextView.setTextColor(textColor);
}

void EvenOddDoubleBufferTextCellWithSeparator::drawRect(KDContext * ctx, KDRect rect) const {
  EvenOddCell::drawRect(ctx, rect);
  // Draw the separator
  KDRect separatorRect(0, 0, k_separatorWidth, bounds().height());
  ctx->fillRect(separatorRect, EvenOddCell::k_hideColor);
}

int EvenOddDoubleBufferTextCellWithSeparator::numberOfSubviews() const {
  return 2;
}

View * EvenOddDoubleBufferTextCellWithSeparator::subviewAtIndex(int index) {
  assert(index == 0 || index == 1);
  if (index == 0) {
    return &m_firstBufferTextView;
  }
  return &m_secondBufferTextView;
}

void EvenOddDoubleBufferTextCellWithSeparator::layoutSubviews(bool force) {
  KDCoordinate width = bounds().width() - k_separatorWidth;
  KDCoordinate height = bounds().height();
  m_firstBufferTextView.setFrame(KDRect(k_separatorWidth, 0, width/2, height), force);
  m_secondBufferTextView.setFrame(KDRect(k_separatorWidth + width/2, 0, width - width/2, height), force);
}

bool EvenOddDoubleBufferTextCellWithSeparator::handleEvent(Ion::Events::Event & event) {
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

}
