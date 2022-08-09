#include <escher/subapp_cell.h>
#include <escher/palette.h>
#include <kandinsky/font.h>
#include <algorithm>

namespace Escher {

SubappCell::SubappCell() {
  m_icon.setBackgroundColor(KDColorWhite);
  m_title.setFont(KDFont::Size::Large);
  m_subTitle.setFont(KDFont::Size::Small);
  m_subTitle.setTextColor(Palette::GrayDark);
}

void SubappCell::drawRect(KDContext * ctx, KDRect rect) const {
  KDColor backColor = isHighlighted() ? Palette::Select : KDColorWhite;
  drawInnerRect(ctx, bounds(), backColor);
  drawBorderOfRect(ctx, bounds(), Palette::GrayBright);
}

void SubappCell::setHighlighted(bool highlighted) {
  HighlightCell::setHighlighted(highlighted);
  KDColor backgroundColor = defaultBackgroundColor();
  m_icon.setBackgroundColor(backgroundColor);
  m_title.setBackgroundColor(backgroundColor);
  m_subTitle.setBackgroundColor(backgroundColor);
}

View * SubappCell::subviewAtIndex(int i) {
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

  return KDSize(Metric::CellLeftMargin + iconSize.width() +
                    k_horizontalMarginBetweenTextAndIcon +
                    std::max(titleSize.width(), subTitleSize.width()) + chevronSize.width() +
                    Metric::CellRightMargin,
                k_verticalMarginTop + titleSize.height() + k_verticalMarginBetweenTexts +
                    subTitleSize.height() + k_verticalMarginBottom);
}

void SubappCell::layoutSubviews(bool force) {
  KDCoordinate width = m_frame.width();
  KDCoordinate height = m_frame.height();

  KDSize iconSize = m_icon.minimalSizeForOptimalDisplay();
  m_icon.setFrame(
      KDRect(Metric::CellLeftMargin, (height - iconSize.height()) / 2, iconSize),
      force);
  KDSize titleSize = m_title.minimalSizeForOptimalDisplay();
  KDSize subTitleSize = m_subTitle.minimalSizeForOptimalDisplay();
  KDCoordinate textXPosition = Metric::CellLeftMargin + iconSize.width() +
                               k_horizontalMarginBetweenTextAndIcon;
  m_title.setFrame(KDRect(textXPosition, k_verticalMarginTop, titleSize), force);
  m_subTitle.setFrame(
      KDRect(textXPosition,
             Metric::CellTopMargin + titleSize.height() + k_verticalMarginBetweenTexts,
             subTitleSize),
      force);
  KDSize chevronSize = m_chevron.minimalSizeForOptimalDisplay();
  m_chevron.setFrame(KDRect(width - chevronSize.width() - Metric::CellRightMargin,
                            (height - chevronSize.height()) / 2,
                            chevronSize),
                     force);
}

void SubappCell::setImage(const Image * image) {
  m_icon.setImage(image);
}

void SubappCell::setMessages(I18n::Message title, I18n::Message subTitle) {
  m_title.setMessage(title);
  m_subTitle.setMessage(subTitle);
}

}
