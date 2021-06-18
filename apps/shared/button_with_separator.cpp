#include "button_with_separator.h"

ButtonWithSeparator::ButtonWithSeparator(Responder * parentResponder, I18n::Message message, Invocation invocation) :
  Button(parentResponder, message, invocation, KDFont::LargeFont, Palette::ButtonText)
{
}

void ButtonWithSeparator::drawRect(KDContext * ctx, KDRect rect) const {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  ctx->fillRect(KDRect(0, 0, width, k_lineThickness), Palette::ListCellBorder);
  ctx->fillRect(KDRect(0, k_lineThickness, width, k_margin-k_lineThickness), Palette::BackgroundApps);
  // Draw rectangle around cell
  ctx->fillRect(KDRect(0, k_margin, width, k_lineThickness), Palette::ListCellBorder);
  ctx->fillRect(KDRect(0, k_margin+k_lineThickness, k_lineThickness, height-k_margin), Palette::ListCellBorder);
  ctx->fillRect(KDRect(width-k_lineThickness, k_lineThickness+k_margin, k_lineThickness, height-k_margin), Palette::ListCellBorder);
  ctx->fillRect(KDRect(0, height-3*k_lineThickness, width, k_lineThickness), Palette::ButtonBorderOut);
  ctx->fillRect(KDRect(0, height-2*k_lineThickness, width, k_lineThickness), Palette::ListCellBorder);
  ctx->fillRect(KDRect(k_lineThickness, height-k_lineThickness, width-2*k_lineThickness, k_lineThickness), Palette::ButtonShadow);
}


void ButtonWithSeparator::layoutSubviews(bool force) {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  m_messageTextView.setFrame(KDRect(k_lineThickness, k_margin + k_lineThickness, width-2*k_lineThickness, height - 4*k_lineThickness-k_margin), force);
}
