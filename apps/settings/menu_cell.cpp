#include "menu_cell.h"

namespace Settings {

MenuCell::MenuCell() :
ChevronMenuListCell(nullptr, KDText::FontSize::Large),
  m_subtitleView(KDText::FontSize::Large, "", 1.0f, 0.5f, Palette::GreyDark)
{
}

void MenuCell::reloadCell() {
  ChevronMenuListCell::reloadCell();
  KDColor backgroundColor = isHighlighted()? Palette::Select : KDColorWhite;
  m_subtitleView.setBackgroundColor(backgroundColor);
}

void MenuCell::setSubtitle(const char * text) {
  m_subtitleView.setText(text);
  markRectAsDirty(bounds());
  layoutSubviews();
}

int MenuCell::numberOfSubviews() const {
  return 3;
}

View * MenuCell::subviewAtIndex(int index) {
  if (index == 0 || index == 1) {
    return ChevronMenuListCell::subviewAtIndex(index);
  }
  return &m_subtitleView;
}

void MenuCell::layoutSubviews() {
  ChevronMenuListCell::layoutSubviews();
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  KDSize subtitleSize = m_subtitleView.minimalSizeForOptimalDisplay();
  KDSize chevronSize = accessoryView()->minimalSizeForOptimalDisplay();
  m_subtitleView.setFrame(KDRect(width-chevronSize.width()-subtitleSize.width()-k_margin, k_separatorThickness, subtitleSize.width(), height - 2*k_separatorThickness));
}

}
