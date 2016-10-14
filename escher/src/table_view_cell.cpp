#include <escher/table_view_cell.h>
#include <assert.h>

constexpr KDCoordinate TableViewCell::k_separatorThickness;

TableViewCell::TableViewCell(char * label) :
  View(),
  m_highlighted(false),
  m_pointerTextView(PointerTextView(label, 0, 0.5, KDColorBlack, Palette::CellBackgroundColor))
{
}

int TableViewCell::numberOfSubviews() const {
  if (contentView() == nullptr) {
    return 1;
  }
  return 2;
}

View * TableViewCell::subviewAtIndex(int index) {
  if (index == 0) {
    return &m_pointerTextView;
  }
  assert(numberOfSubviews() == 2 && index == 1);
  return contentView();
}

void TableViewCell::layoutSubviews() {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  m_pointerTextView.setFrame(KDRect(k_separatorThickness, k_separatorThickness, 3*width/4 - 2*k_separatorThickness, height - 2*k_separatorThickness));
  View * content = contentView();
  if (content) {
    content->setFrame(KDRect(k_separatorThickness + 3*width/4, k_separatorThickness, width/4-2*k_separatorThickness, height-2*k_separatorThickness));
  }
}

PointerTextView * TableViewCell::textView() {
  return &m_pointerTextView;
}

View * TableViewCell::contentView() const {
  return nullptr;
}

bool TableViewCell::isHighlighted() const {
  return m_highlighted;
}

void TableViewCell::setHighlighted(bool highlight) {
  m_highlighted = highlight;
  KDColor backgroundColor = highlight? Palette::FocusCellBackgroundColor : Palette::CellBackgroundColor;
  m_pointerTextView.setBackgroundColor(backgroundColor);
  markRectAsDirty(bounds());
}

void TableViewCell::drawRect(KDContext * ctx, KDRect rect) const {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  KDColor backgroundColor = (m_highlighted ? Palette::FocusCellBackgroundColor : Palette::CellBackgroundColor);

  ctx->fillRect(KDRect(k_separatorThickness, k_separatorThickness, width-2*k_separatorThickness, height-k_separatorThickness), backgroundColor);
  ctx->fillRect(KDRect(0, 0, width, k_separatorThickness), Palette::LineColor);
  ctx->fillRect(KDRect(0, k_separatorThickness, k_separatorThickness, height-k_separatorThickness), Palette::LineColor);
  ctx->fillRect(KDRect(width-k_separatorThickness, k_separatorThickness, k_separatorThickness, height-k_separatorThickness), Palette::LineColor);
 }
