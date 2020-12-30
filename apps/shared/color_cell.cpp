#include "color_cell.h"

namespace Shared {

constexpr const I18n::Message MessageTableCellWithColor::k_textForIndex[Palette::numberOfDataColors()];

MessageTableCellWithColor::MessageTableCellWithColor() :
  MessageTableCell(),
  m_accessoryView()
  {}

View * MessageTableCellWithColor::accessoryView() const {
  return (View *)&m_accessoryView;
}

void MessageTableCellWithColor::setColor(int i) {
  m_accessoryView.setColor(i);
  MessageTextView * label = (MessageTextView*)(labelView());
  return label->setMessage(k_textForIndex[i]);
}

MessageTableCellWithColor::ColorView::ColorView() :
  m_index(0)
  {}

void MessageTableCellWithColor::ColorView::drawRect(KDContext * ctx, KDRect rect) const {
  KDColor Buffer[MessageTableCellWithColor::ColorView::k_colorSize*MessageTableCellWithColor::ColorView::k_colorSize];
  KDRect Frame(bounds().x(), bounds().y() + bounds().height()/2 - k_colorSize/2, k_colorSize, k_colorSize);
  ctx->blendRectWithMask(Frame, Palette::DataColor[m_index], (const uint8_t *)colorMask, Buffer);
  //ctx->fillRect(bounds(), Palette::DataColor[m_index]);
}

KDSize MessageTableCellWithColor::ColorView::minimalSizeForOptimalDisplay() const {
  return KDSize(k_colorSize, k_colorSize);
}

}
