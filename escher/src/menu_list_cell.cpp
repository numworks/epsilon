#include <escher/menu_list_cell.h>
#include <assert.h>

constexpr KDCoordinate MenuListCell::k_separatorThickness;

MenuListCell::MenuListCell(char * label, KDText::FontSize size) :
  TableViewCell(),
  m_pointerTextView(PointerTextView(size, label, 0, 0.5, KDColorBlack, KDColorWhite))
{
}

int MenuListCell::numberOfSubviews() const {
  if (accessoryView() == nullptr) {
    return 1;
  }
  return 2;
}

View * MenuListCell::subviewAtIndex(int index) {
  if (index == 0) {
    return &m_pointerTextView;
  }
  assert(numberOfSubviews() >= 2 && index == 1);
  return accessoryView();
}

void MenuListCell::layoutSubviews() {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  KDSize labelSize = m_pointerTextView.minimalSizeForOptimalDisplay();
  m_pointerTextView.setFrame(KDRect(k_separatorThickness, k_separatorThickness, labelSize.width(), height - 2*k_separatorThickness));
  View * accessory = accessoryView();
  if (accessory) {
    accessory->setFrame(KDRect(k_separatorThickness + labelSize.width(), k_separatorThickness, width - labelSize.width()-2*k_separatorThickness, height-2*k_separatorThickness));
  }
}

void MenuListCell::reloadCell() {
  TableViewCell::reloadCell();
  KDColor backgroundColor = isHighlighted()? Palette::Select : KDColorWhite;
  m_pointerTextView.setBackgroundColor(backgroundColor);
}

void MenuListCell::setText(const char * text) {
  m_pointerTextView.setText(text);
  layoutSubviews();
}

void MenuListCell::setTextColor(KDColor color) {
  m_pointerTextView.setTextColor(color);
}

View * MenuListCell::accessoryView() const {
  return nullptr;
}

void MenuListCell::drawRect(KDContext * ctx, KDRect rect) const {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  KDColor backgroundColor = isHighlighted() ? Palette::Select : KDColorWhite;

  ctx->fillRect(KDRect(k_separatorThickness, k_separatorThickness, width-2*k_separatorThickness, height-k_separatorThickness), backgroundColor);
  ctx->fillRect(KDRect(0, 0, width, k_separatorThickness), Palette::GreyBright);
  ctx->fillRect(KDRect(0, k_separatorThickness, k_separatorThickness, height-k_separatorThickness), Palette::GreyBright);
  ctx->fillRect(KDRect(width-k_separatorThickness, k_separatorThickness, k_separatorThickness, height-k_separatorThickness), Palette::GreyBright);
 }
