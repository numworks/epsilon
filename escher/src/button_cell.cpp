#include <escher/button_cell.h>

namespace Escher {

ButtonCell::ButtonCell(Responder* parentResponder, I18n::Message textBody,
                       Escher::Invocation invocation, KDColor backgroundColor,
                       KDCoordinate horizontalMargins)
    : AbstractButtonCell(parentResponder, textBody, invocation,
                         KDFont::Size::Large, KDColorBlack),
      m_backgroundColor(backgroundColor),
      m_horizontalMargins(horizontalMargins) {}

void ButtonCell::drawRect(KDContext* ctx, KDRect rect) const {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  // Draw horizontal margins
  ctx->fillRect(KDRect(0, 0, m_horizontalMargins, height), m_backgroundColor);
  ctx->fillRect(
      KDRect(width - m_horizontalMargins, 0, m_horizontalMargins, height),
      m_backgroundColor);
  // Draw rectangle around cell
  ctx->fillRect(KDRect(m_horizontalMargins, k_lineThickness, k_lineThickness,
                       height - 3 * k_lineThickness),
                Palette::GrayBright);
  ctx->fillRect(
      KDRect(width - m_horizontalMargins - k_lineThickness, k_lineThickness,
             k_lineThickness, height - 3 * k_lineThickness),
      Palette::GrayBright);
  ctx->fillRect(KDRect(m_horizontalMargins, 0, width - 2 * m_horizontalMargins,
                       k_lineThickness),
                Palette::GrayBright);
  ctx->fillRect(KDRect(m_horizontalMargins, height - 3 * k_lineThickness,
                       width - 2 * m_horizontalMargins, k_lineThickness),
                Palette::GrayWhite);
  ctx->fillRect(KDRect(m_horizontalMargins, height - 2 * k_lineThickness,
                       width - 2 * m_horizontalMargins, k_lineThickness),
                Palette::GrayBright);
  ctx->fillRect(KDRect(m_horizontalMargins, height - k_lineThickness,
                       width - 2 * m_horizontalMargins, k_lineThickness),
                Palette::GrayMiddle);
}

void ButtonCell::layoutSubviews(bool force) {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  setChildFrame(&m_messageTextView,
                KDRect(m_horizontalMargins + k_lineThickness, k_lineThickness,
                       width - 2 * m_horizontalMargins - 2 * k_lineThickness,
                       height - 4 * k_lineThickness),
                force);
}

KDSize ButtonCell::minimalSizeForOptimalDisplay() const {
  KDSize buttonSize =
      Escher::AbstractButtonCell::minimalSizeForOptimalDisplay();
  return KDSize(buttonSize.width() + 2 * m_horizontalMargins,
                buttonSize.height());
}

}  // namespace Escher
