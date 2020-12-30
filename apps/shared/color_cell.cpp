#include "color_cell.h"

namespace Shared {

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
  switch(i) {
    case 0:
      return label->setMessage(k_textForIndex[0]); // FIXME
    case 1:
      return label->setMessage(k_textForIndex[1]); // FIXME
    case 2:
      return label->setMessage(k_textForIndex[2]); // FIXME
    case 3:
      return label->setMessage(k_textForIndex[3]); // FIXME
    case 4:
      return label->setMessage(k_textForIndex[4]); // FIXME
    case 5:
      return label->setMessage(k_textForIndex[5]); // FIXME
    case 6:
      return label->setMessage(k_textForIndex[6]); // FIXME
    case 7:
      return label->setMessage(k_textForIndex[7]); // FIXME
    default:
      assert(false);
  }
  // return label->setMessage(k_textForIndex[i]); // FIXME
}

MessageTableCellWithColor::ColorView::ColorView() :
  m_index(0)
  {}

void MessageTableCellWithColor::ColorView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), Palette::DataColor[m_index]);
}

KDSize MessageTableCellWithColor::ColorView::minimalSizeForOptimalDisplay() const {
  return KDSize(10, 10);
}

}
