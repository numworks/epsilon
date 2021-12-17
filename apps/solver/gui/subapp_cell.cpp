#include "subapp_cell.h"

#include <kandinsky/font.h>

#include <algorithm>

namespace Solver {

SubappCell::SubappCell() {
  m_title.setFont(KDFont::LargeFont);
  m_subTitle.setFont(KDFont::SmallFont);
  m_subTitle.setTextColor(Escher::Palette::GrayDark);
}

void SubappCell::drawRect(KDContext * ctx, KDRect rect) const {
  KDColor backColor = isHighlighted() ? Escher::Palette::Select : KDColorWhite;
  drawInnerRect(ctx, bounds(), backColor);
  drawBorderOfRect(ctx, bounds(), Escher::Palette::GrayBright);
}

void SubappCell::setHighlighted(bool highlighted) {
  m_icon.setHighlighted(highlighted);
  m_title.setHighlighted(highlighted);
  m_subTitle.setHighlighted(highlighted);
  HighlightCell::setHighlighted(highlighted);
}

Escher::View * SubappCell::subviewAtIndex(int i) {
  assert(i < numberOfSubviews());
  switch (i) {
    case 0:
      return &m_icon;
    case 1:
      return &m_title;
    case 2:
      return &m_subTitle;
    case 3:
      return &m_chevron;
  }
  return nullptr;
}

KDSize SubappCell::minimalSizeForOptimalDisplay() const {
  KDSize iconSize = m_icon.minimalSizeForOptimalDisplay();
  KDSize titleSize = m_title.minimalSizeForOptimalDisplay();
  KDSize subTitleSize = m_subTitle.minimalSizeForOptimalDisplay();
  KDSize chevronSize = m_chevron.minimalSizeForOptimalDisplay();

  return KDSize(Escher::Metric::CellLeftMargin + iconSize.width() +
                    k_horizontalMarginBetweenTextAndIcon +
                    std::max(titleSize.width(), subTitleSize.width()) + chevronSize.width() +
                    Escher::Metric::CellRightMargin,
                k_verticalMarginTop + titleSize.height() + k_verticalMarginBetweenTexts +
                    subTitleSize.height() + k_verticalMarginBottom);
}

void SubappCell::layoutSubviews(bool force) {
  KDCoordinate width = m_frame.width();
  KDCoordinate height = m_frame.height();

  KDSize iconSize = m_icon.minimalSizeForOptimalDisplay();
  m_icon.setFrame(
      KDRect(Escher::Metric::CellLeftMargin, (height - iconSize.height()) / 2, iconSize),
      force);
  KDSize titleSize = m_title.minimalSizeForOptimalDisplay();
  KDSize subTitleSize = m_subTitle.minimalSizeForOptimalDisplay();
  KDCoordinate textXPosition = Escher::Metric::CellLeftMargin + iconSize.width() +
                               k_horizontalMarginBetweenTextAndIcon;
  m_title.setFrame(KDRect(textXPosition, k_verticalMarginTop, titleSize), force);
  m_subTitle.setFrame(
      KDRect(textXPosition,
             Escher::Metric::CellTopMargin + titleSize.height() + k_verticalMarginBetweenTexts,
             subTitleSize),
      force);
  KDSize chevronSize = m_chevron.minimalSizeForOptimalDisplay();
  m_chevron.setFrame(KDRect(width - chevronSize.width() - Escher::Metric::CellRightMargin,
                            (height - chevronSize.height()) / 2,
                            chevronSize),
                     force);
}

void SubappCell::setImage(const Escher::Image * image) {
  m_icon.setImage(image);
}

void SubappCell::setMessages(I18n::Message title, I18n::Message subTitle) {
  m_title.setMessage(title);
  m_subTitle.setMessage(subTitle);
}

}  // namespace Solver
