#include <escher/subapp_cell.h>
#include <escher/theme/color_picker.h>
#include <escher/theme/highlight.h>
#include <kandinsky/font.h>

#include <algorithm>

namespace Escher {

SubappCell::SubappCell() {
  m_icon.setBackgroundColor(k_background);
  m_title.setFont(KDFont::Size::Large);
  m_subTitle.setFont(KDFont::Size::Small);
  m_subTitle.setTextColor(ThemeComponent::TextGrayDark);
}

void SubappCell::drawRect(KDContext* ctx, KDRect rect) const {
  ThemeComponent backColor = Highlight(k_background, isHighlighted());
  drawInnerRect(ctx, bounds(), backColor);
  drawBorderOfRect(ctx, bounds(), ThemeComponent::BorderGrayBright);
}

void SubappCell::setHighlighted(bool highlighted) {
  HighlightCell::setHighlighted(highlighted);
  m_icon.setBackgroundColor(Highlight(k_defaultBackground, isHighlighted()));
  m_title.setHighlighted(highlighted);
  m_subTitle.setHighlighted(highlighted);
}

View* SubappCell::subviewAtIndex(int i) {
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

  return KDSize(Metric::CellMargins.width() + iconSize.width() +
                    k_horizontalMarginBetweenTextAndIcon +
                    std::max(titleSize.width(), subTitleSize.width()) +
                    chevronSize.width(),
                k_verticalMarginTop + titleSize.height() +
                    k_verticalMarginBetweenTexts + subTitleSize.height() +
                    k_verticalMarginBottom);
}

void SubappCell::layoutSubviews(bool force) {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();

  KDSize iconSize = m_icon.minimalSizeForOptimalDisplay();
  setChildFrame(&m_icon,
                KDRect(Metric::CellMargins.left(),
                       (height - iconSize.height()) / 2, iconSize),
                force);
  KDSize titleSize = m_title.minimalSizeForOptimalDisplay();
  KDSize subTitleSize = m_subTitle.minimalSizeForOptimalDisplay();
  KDCoordinate textXPosition = Metric::CellMargins.left() + iconSize.width() +
                               k_horizontalMarginBetweenTextAndIcon;
  setChildFrame(&m_title, KDRect(textXPosition, k_verticalMarginTop, titleSize),
                force);
  setChildFrame(&m_subTitle,
                KDRect(textXPosition,
                       Metric::CellMargins.top() + titleSize.height() +
                           k_verticalMarginBetweenTexts,
                       subTitleSize),
                force);
  KDSize chevronSize = m_chevron.minimalSizeForOptimalDisplay();
  setChildFrame(
      &m_chevron,
      KDRect(width - chevronSize.width() - Metric::CellMargins.right(),
             (height - chevronSize.height()) / 2, chevronSize),
      force);
}

void SubappCell::setImage(const Image* image) { m_icon.setImage(image); }

void SubappCell::setMessages(I18n::Message title, I18n::Message subTitle) {
  m_title.setMessage(title);
  m_subTitle.setMessage(subTitle);
}

}  // namespace Escher
