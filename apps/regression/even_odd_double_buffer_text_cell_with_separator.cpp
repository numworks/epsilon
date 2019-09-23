#include "even_odd_double_buffer_text_cell_with_separator.h"
#include "../shared/hideable_even_odd_editable_text_cell.h"
#include "escher/metric.h"
#include <assert.h>

namespace Regression {

EvenOddDoubleBufferTextCellWithSeparator::EvenOddDoubleBufferTextCellWithSeparator(Responder * parentResponder, float horizontalAlignment, float verticalAlignment) :
  EvenOddCell(),
  Responder(parentResponder),
  m_firstTextSelected(true),
  m_firstBufferTextView(KDFont::SmallFont, horizontalAlignment, verticalAlignment),
  m_secondBufferTextView(KDFont::SmallFont, horizontalAlignment, verticalAlignment)
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
  m_firstBufferTextView.setHighlighted(selectFirstText);
  m_secondBufferTextView.setHighlighted(!selectFirstText);
}

void EvenOddDoubleBufferTextCellWithSeparator::reloadCell() {
  m_firstBufferTextView.reloadCell();
  m_secondBufferTextView.reloadCell();
}

void EvenOddDoubleBufferTextCellWithSeparator::setHighlighted(bool highlight) {
  m_firstBufferTextView.setHighlighted(false);
  m_secondBufferTextView.setHighlighted(false);
  HighlightCell::setHighlighted(highlight);
  if (isHighlighted()) {
    if (m_firstTextSelected) {
      m_firstBufferTextView.setHighlighted(true);
    } else {
      m_secondBufferTextView.setHighlighted(true);
    }
  }
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
  KDRect separatorRect(0, 0, Metric::TableSeparatorThickness, bounds().height());
  ctx->fillRect(separatorRect, Shared::HideableEvenOddEditableTextCell::hideColor());
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

void EvenOddDoubleBufferTextCellWithSeparator::layoutSubviews() {
  KDCoordinate width = bounds().width() - Metric::TableSeparatorThickness;
  KDCoordinate height = bounds().height();
  m_firstBufferTextView.setFrame(KDRect(Metric::TableSeparatorThickness, 0, width/2, height));
  m_secondBufferTextView.setFrame(KDRect(Metric::TableSeparatorThickness + width/2, 0, width - width/2, height));
}

bool EvenOddDoubleBufferTextCellWithSeparator::handleEvent(Ion::Events::Event event) {
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
