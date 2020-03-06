#include "cell.h"
#include <assert.h>

namespace Probability {

Cell::Cell() :
  HighlightCell(),
  m_labelView(KDFont::LargeFont, (I18n::Message)0, 0, 0.5, KDColorBlack, KDColorWhite),
  m_icon(nullptr),
  m_focusedIcon(nullptr)
{
}

int Cell::numberOfSubviews() const {
  return 3;
}

View * Cell::subviewAtIndex(int index) {
  assert(index >= 0 && index < 3);
  if (index == 0) {
    return &m_labelView;
  }
  if (index == 1) {
    return &m_iconView;
  }
  return &m_chevronView;
}

void Cell::layoutSubviews(bool force) {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  m_labelView.setFrame(KDRect(1+k_iconWidth+2*k_iconMargin, 1, width-2-k_iconWidth-2*k_iconMargin - k_chevronWidth, height-2), force);
  m_iconView.setFrame(KDRect(1+k_iconMargin, (height - k_iconHeight)/2, k_iconWidth, k_iconHeight), force);
  m_chevronView.setFrame(KDRect(width-1-k_chevronWidth-k_chevronMargin, 1, k_chevronWidth, height - 2), force);
}

void Cell::reloadCell() {
  HighlightCell::reloadCell();
  KDColor backgroundColor = isHighlighted()? Palette::Select : KDColorWhite;
  m_labelView.setBackgroundColor(backgroundColor);
  if (isHighlighted()) {
    m_iconView.setImage(m_focusedIcon);
  } else {
    m_iconView.setImage(m_icon);
  }
}

void Cell::setLabel(I18n::Message message) {
  m_labelView.setMessage(message);
}

void Cell::setImage(const Image * image, const Image * focusedImage) {
  m_icon = image;
  m_focusedIcon = focusedImage;
}

void Cell::drawRect(KDContext * ctx, KDRect rect) const {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  KDColor backgroundColor = isHighlighted() ? Palette::Select : KDColorWhite;
  ctx->fillRect(KDRect(1, 1, width-2, height-1), backgroundColor);
  ctx->fillRect(KDRect(0, 0, width, 1), Palette::GreyBright);
  ctx->fillRect(KDRect(0, 1, 1, height-1), Palette::GreyBright);
  ctx->fillRect(KDRect(width-1, 1, 1, height-1), Palette::GreyBright);
  ctx->fillRect(KDRect(0, height-1, width, 1), Palette::GreyBright);
 }

}
