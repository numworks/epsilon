#include <escher/chevron_text_menu_list_cell.h>

ChevronTextMenuListCell::ChevronTextMenuListCell(KDText::FontSize size) :
ChevronMenuListCell(nullptr, size),
  m_subtitleView(size, "", 1.0f, 0.5f, Palette::GreyDark)
{
}

void ChevronTextMenuListCell::setHighlighted(bool highlight) {
  ChevronMenuListCell::setHighlighted(highlight);
  KDColor backgroundColor = isHighlighted()? Palette::Select : KDColorWhite;
  m_subtitleView.setBackgroundColor(backgroundColor);
}

void ChevronTextMenuListCell::setSubtitle(const char * text) {
  m_subtitleView.setText(text);
  markRectAsDirty(bounds());
  layoutSubviews();
}

int ChevronTextMenuListCell::numberOfSubviews() const {
  return 3;
}

View * ChevronTextMenuListCell::subviewAtIndex(int index) {
  if (index == 0 || index == 1) {
    return ChevronMenuListCell::subviewAtIndex(index);
  }
  return &m_subtitleView;
}

void ChevronTextMenuListCell::layoutSubviews() {
  ChevronMenuListCell::layoutSubviews();
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  KDSize subtitleSize = m_subtitleView.minimalSizeForOptimalDisplay();
  KDSize chevronSize = accessoryView()->minimalSizeForOptimalDisplay();
  m_subtitleView.setFrame(KDRect(width-chevronSize.width()-subtitleSize.width()-k_margin, k_separatorThickness, subtitleSize.width(), height - 2*k_separatorThickness));
}
