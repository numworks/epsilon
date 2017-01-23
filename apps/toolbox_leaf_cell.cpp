#include "toolbox_leaf_cell.h"
#include <assert.h>

ToolboxLeafCell::ToolboxLeafCell() :
  TableViewCell(),
  m_labelView(PointerTextView(KDText::FontSize::Small, nullptr, 0, 0.5, KDColorBlack, KDColorWhite)),
  m_subtitleView(PointerTextView(KDText::FontSize::Small, nullptr, 0, 0.5, Palette::GreyDark, KDColorWhite))
{
}

int ToolboxLeafCell::numberOfSubviews() const {
  return 2;
}

View * ToolboxLeafCell::subviewAtIndex(int index) {
  assert(index == 0 || index == 1);
  if (index == 0) {
    return &m_labelView;
  }
  return &m_subtitleView;
}

void ToolboxLeafCell::layoutSubviews() {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  m_labelView.setFrame(KDRect(1, 1, width-2, height/2 - 1));
  m_subtitleView.setFrame(KDRect(1, height/2, width-2, height/2 - 1));
}

void ToolboxLeafCell::reloadCell() {
  TableViewCell::reloadCell();
  KDColor backgroundColor = isHighlighted()? Palette::Select : KDColorWhite;
  m_labelView.setBackgroundColor(backgroundColor);
  m_subtitleView.setBackgroundColor(backgroundColor);
}

void ToolboxLeafCell::setLabel(const char * text) {
  m_labelView.setText(text);
}

void ToolboxLeafCell::setSubtitle(const char * text) {
  m_subtitleView.setText(text);
}

void ToolboxLeafCell::drawRect(KDContext * ctx, KDRect rect) const {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  KDColor backgroundColor = isHighlighted() ? Palette::Select : KDColorWhite;
  ctx->fillRect(KDRect(1, 0, width-2, height-1), backgroundColor);
  ctx->fillRect(KDRect(0, height-1, width, 1), Palette::GreyBright);
  ctx->fillRect(KDRect(0, 0, 1, height-1), Palette::GreyBright);
  ctx->fillRect(KDRect(width-1, 0, 1, height-1), Palette::GreyBright);
 }
