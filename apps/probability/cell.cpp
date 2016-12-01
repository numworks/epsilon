#include "cell.h"
#include <assert.h>

namespace Probability {

Cell::Cell() :
  TableViewCell(),
  m_labelView(PointerTextView(nullptr, 0, 0.5, KDColorBlack, Palette::CellBackgroundColor))
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

void Cell::layoutSubviews() {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  m_labelView.setFrame(KDRect(1+k_iconWidth+2*k_iconMargin, 1, width-2-k_iconWidth-2*k_iconMargin - k_chevronWidth, height-2));
  m_iconView.setFrame(KDRect(1+k_iconMargin, (height - k_iconHeight)/2, k_iconWidth, k_iconHeight));
  m_chevronView.setFrame(KDRect(width-1-k_chevronWidth, 1, k_chevronWidth, height - 2));
}

void Cell::reloadCell() {
  TableViewCell::reloadCell();
  KDColor backgroundColor = isHighlighted()? Palette::FocusCellBackgroundColor : Palette::CellBackgroundColor;
  m_labelView.setBackgroundColor(backgroundColor);
  m_chevronView.setHighlighted(isHighlighted());
  if (isHighlighted()) {
    m_iconView.setImage(m_focusedIcon);
  } else {
    m_iconView.setImage(m_icon);
  }
}

void Cell::setLabel(const char * text) {
  m_labelView.setText(text);
}

void Cell::setImage(const Image * image, const Image * focusedImage) {
  m_icon = image;
  m_focusedIcon = focusedImage;
}

void Cell::drawRect(KDContext * ctx, KDRect rect) const {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  KDColor backgroundColor = isHighlighted() ? Palette::FocusCellBackgroundColor : Palette::CellBackgroundColor;
  ctx->fillRect(KDRect(1, 1, width-2, height-1), backgroundColor);
  ctx->fillRect(KDRect(0, 0, width, 1), Palette::LineColor);
  ctx->fillRect(KDRect(0, 1, 1, height-1), Palette::LineColor);
  ctx->fillRect(KDRect(width-1, 1, 1, height-1), Palette::LineColor);
 }

}
