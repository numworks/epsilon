#include <escher/button_cell.h>

namespace Escher {

ButtonCell::ButtonCell(Responder* parentResponder, I18n::Message textBody,
                       Escher::Invocation invocation, Style style,
                       KDFont::Size fontSize, KDColor textColor)
    : HighlightCell(),
      Responder(parentResponder),
      m_messageTextView(textBody,
                        {.style = {.glyphColor = textColor, .font = fontSize},
                         .horizontalAlignment = KDGlyph::k_alignCenter}),
      m_invocation(invocation),
      m_font(fontSize),
      m_style(style) {}

View* ButtonCell::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_messageTextView;
}

bool ButtonCell::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    m_invocation.perform(this);
    return true;
  }
  return event == Ion::Events::Var || event == Ion::Events::Sto ||
         event == Ion::Events::Clear;
}

void ButtonCell::setHighlighted(bool highlight) {
  HighlightCell::setHighlighted(highlight);
  KDColor backgroundColor =
      highlight ? highlightedBackgroundColor() : KDColorWhite;
  m_messageTextView.setBackgroundColor(backgroundColor);
  markWholeFrameAsDirty();
}

void ButtonCell::drawBorder(KDContext* ctx, OMG::Direction direction, int index,
                            KDColor color) const {
  assert(0 <= index && index <= numberOfBordersInDirection(direction));
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  if (direction.isVertical()) {
    KDCoordinate y = direction.isUp() ? index * k_lineThickness
                                      : height - (index + 1) * k_lineThickness;
    ctx->fillRect(KDRect(0, y, width, k_lineThickness), color);
  } else {
    assert(direction.isHorizontal());
    KDCoordinate x = direction.isLeft() ? index * k_lineThickness
                                        : width - (index + 1) * k_lineThickness;
    ctx->fillRect(KDRect(x, 0, k_lineThickness, height), color);
  }
}

void ButtonCell::drawRect(KDContext* ctx, KDRect rect) const {
  // Draw rectangle around cell
  if (m_style == Style::EmbossedLight) {
    drawBorder(ctx, OMG::Direction::Right(), 0, Palette::GrayBright);
    drawBorder(ctx, OMG::Direction::Up(), 0, Palette::GrayBright);
    drawBorder(ctx, OMG::Direction::Left(), 0, Palette::GrayBright);
    drawBorder(ctx, OMG::Direction::Down(), 0, Palette::GrayMiddle);
    drawBorder(ctx, OMG::Direction::Down(), 1, Palette::GrayBright);
    drawBorder(ctx, OMG::Direction::Down(), 2, Palette::GrayWhite);
  } else if (m_style == Style::EmbossedGray) {
    drawBorder(ctx, OMG::Direction::Right(), 0, Palette::GrayMiddle);
    drawBorder(ctx, OMG::Direction::Up(), 0, Palette::GrayMiddle);
    drawBorder(ctx, OMG::Direction::Left(), 1, Palette::GrayMiddle);
    drawBorder(ctx, OMG::Direction::Down(), 1, Palette::GrayMiddle);
    drawBorder(ctx, OMG::Direction::Left(), 0, Palette::GrayDark);
    drawBorder(ctx, OMG::Direction::Down(), 0, Palette::GrayDark);
  }
}

void ButtonCell::layoutSubviews(bool force) {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  int nLinesTop = numberOfBordersInDirection(OMG::Direction::Up());
  int nLinesBottom = numberOfBordersInDirection(OMG::Direction::Down());
  int nLinesLeft = numberOfBordersInDirection(OMG::Direction::Left());
  int nLinesRight = numberOfBordersInDirection(OMG::Direction::Right());
  setChildFrame(
      &m_messageTextView,
      KDRect(nLinesLeft * k_lineThickness, nLinesTop * k_lineThickness,
             width - (nLinesLeft + nLinesRight) * k_lineThickness,
             height - (nLinesTop + nLinesBottom) * k_lineThickness),
      force);
}

KDSize ButtonCell::minimalSizeForOptimalDisplay() const {
  KDSize textSize = m_messageTextView.minimalSizeForOptimalDisplay();
  return m_font == KDFont::Size::Small
             ? KDSize(textSize.width() + k_horizontalMarginSmall,
                      textSize.height() + k_verticalMarginSmall)
             : KDSize(textSize.width() + k_horizontalMarginLarge,
                      textSize.height() + k_verticalMarginLarge);
}

int ButtonCell::numberOfBordersInDirection(OMG::Direction direction) const {
  if (m_style == Style::None) {
    return 0;
  }
  if (direction.isLeft()) {
    return 1 + (m_style == Style::EmbossedGray);
  }
  if (direction.isDown()) {
    return 2 + (m_style == Style::EmbossedLight);
  }
  assert(direction.isUp() || direction.isRight());
  return 1;
}

}  // namespace Escher
