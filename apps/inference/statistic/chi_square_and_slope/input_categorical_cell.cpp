#include "input_categorical_cell.h"

namespace Inference {

InputCategoricalCell::InputCategoricalCell(
    Responder* parentResponder,
    InputEventHandlerDelegate* inputEventHandlerDelegate,
    TextFieldDelegate* textFieldDelegate)
    : HighlightCell(),
      m_innerCell(parentResponder, inputEventHandlerDelegate,
                  textFieldDelegate) {}

KDSize InputCategoricalCell::minimalSizeForOptimalDisplay() const {
  KDSize innerSize = m_innerCell.minimalSizeForOptimalDisplay();
  return KDSize(innerSize.width() + 2 * Metric::CommonMargin,
                innerSize.height());
}

void InputCategoricalCell::drawRect(KDContext* ctx, KDRect rect) const {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();

  // Draw horizontal margins
  ctx->fillRect(KDRect(0, 0, Metric::CommonMargin, height),
                Palette::WallScreenDark);
  ctx->fillRect(
      KDRect(width - Metric::CommonMargin, 0, Metric::CommonMargin, height),
      Escher::Palette::WallScreenDark);
}

void InputCategoricalCell::setHighlighted(bool highlight) {
  HighlightCell::setHighlighted(highlight);
  m_innerCell.setHighlighted(highlight);
}

void InputCategoricalCell::setMessages(I18n::Message labelMessage,
                                       I18n::Message subLabelMessage) {
  m_innerCell.label()->setMessage(labelMessage);
  m_innerCell.subLabel()->setMessage(subLabelMessage);
}

void InputCategoricalCell::layoutSubviews(bool force) {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  setChildFrame(
      &m_innerCell,
      KDRect(Metric::CommonMargin, 0, width - 2 * Metric::CommonMargin, height),
      force);
}

}  // namespace Inference
