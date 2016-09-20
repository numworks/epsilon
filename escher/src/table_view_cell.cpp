#include <escher/table_view_cell.h>
#include <assert.h>

constexpr KDCoordinate TableViewCell::k_margin;
constexpr KDCoordinate TableViewCell::k_marginLabel;

TableViewCell::TableViewCell(char * label) :
  View(),
  m_highlighted(false),
  m_labelView(LabelView(label, Palette::CellBackgroundColor, KDColorBlack))
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
    return &m_labelView;
  }
  assert(numberOfSubviews() == 2 && index == 1);
  return contentView();
}

void TableViewCell::layoutSubviews() {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  m_labelView.setFrame(KDRect(k_margin + k_marginLabel, k_marginLabel, 3*width/4, height));
  View * content = contentView();
  if (content) {
    content->setFrame(KDRect(k_margin + k_marginLabel + 3*width/4, k_marginLabel, width/4-k_margin, height-k_marginLabel));
  }
}

LabelView * TableViewCell::labelView() {
  return &m_labelView;
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
  m_labelView.setBackgroundColor(backgroundColor);
  markRectAsDirty(bounds());
}

void TableViewCell::drawRect(KDContext * ctx, KDRect rect) const {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  KDColor backgroundColor = (m_highlighted ? Palette::FocusCellBackgroundColor : Palette::CellBackgroundColor);

  ctx->fillRect(KDRect(k_margin+1, 1, width-2*k_margin-1, height-1), backgroundColor);
  ctx->fillRect(KDRect(0,0,k_margin,height), Palette::BackgroundColor);
  ctx->fillRect(KDRect(k_margin,0,width-2*k_margin,1), Palette::LineColor);
  ctx->fillRect(KDRect(k_margin,0,1,height), Palette::LineColor);
  ctx->fillRect(KDRect(width-k_margin,0,1,height), Palette::LineColor);
  ctx->fillRect(KDRect(width-k_margin+1,0,k_margin, height), Palette::BackgroundColor);
}
