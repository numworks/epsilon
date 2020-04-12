#include "calculation_cell.h"
#include "responder_image_cell.h"
#include <apps/i18n.h>
#include <assert.h>
#include <algorithm>

namespace Probability {

CalculationCell::CalculationCell(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, TextFieldDelegate * textFieldDelegate) :
  m_text(KDFont::LargeFont, I18n::Message::Default, 0.5f, 0.5f),
  m_calculation(parentResponder, inputEventHandlerDelegate, textFieldDelegate),
  m_isResponder(true)
{
}

Responder * CalculationCell::responder() {
  if (m_isResponder) {
    return &m_calculation;
  }
  return nullptr;
}

void CalculationCell::setResponder(bool shouldBeResponder) {
  m_isResponder = shouldBeResponder;
}

void CalculationCell::setHighlighted(bool highlight) {
  HighlightCell::setHighlighted(highlight);
  m_calculation.setHighlighted(highlight);
}

KDSize CalculationCell::minimalSizeForOptimalDisplay() const {
  KDSize textSize = m_text.minimalSizeForOptimalDisplay();
  return KDSize(
    2 * k_margin + textSize.width() + calculationCellWidth() + 2 * ResponderImageCell::k_outline,
    KDFont::LargeFont->glyphSize().height()
  );
}

void CalculationCell::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), KDColorWhite);
  if (m_isResponder) {
    KDSize textSize = m_text.minimalSizeForOptimalDisplay();
    ctx->strokeRect(KDRect(2*k_margin+textSize.width(), 0, calculationCellWidth()+2*ResponderImageCell::k_outline, ImageCell::k_height+2*ResponderImageCell::k_outline), Palette::GreyMiddle);
  }
}

EditableTextCell * CalculationCell::editableTextCell() {
  return &m_calculation;
}

MessageTextView * CalculationCell::messageTextView() {
  return &m_text;
}

int CalculationCell::numberOfSubviews() const {
  return 2;
}

View * CalculationCell::subviewAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return &m_text;
  }
  return &m_calculation;
}

void CalculationCell::layoutSubviews(bool force) {
  KDSize textSize = m_text.minimalSizeForOptimalDisplay();
  m_text.setFrame(KDRect(k_margin, 0, textSize.width(), bounds().height()), force);
  m_calculation.setFrame(KDRect(2*k_margin+textSize.width()+ResponderImageCell::k_outline, ResponderImageCell::k_outline, calculationCellWidth(), ImageCell::k_height), force);
}

KDCoordinate CalculationCell::calculationCellWidth() const {
  KDCoordinate calculationCellWidth = m_calculation.minimalSizeForOptimalDisplay().width();
  KDCoordinate glyphWidth = KDFont::LargeFont->glyphSize().width();
  KDCoordinate minTextFieldWidth = 4 * glyphWidth + TextCursorView::k_width;
  KDCoordinate maxTextFieldWidth = 14 * glyphWidth + TextCursorView::k_width;
  return std::min(maxTextFieldWidth, std::max(minTextFieldWidth, calculationCellWidth));
}

}
