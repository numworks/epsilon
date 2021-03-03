#include <escher/buffer_table_cell.h>
#include <escher/palette.h>
#include <assert.h>

namespace Escher {

BufferTableCell::BufferTableCell() :
  TableCell(),
  // Text is aligned to the left, and vertically centered
  m_labelView(KDFont::LargeFont, 0.0f, 0.5f, Palette::GrayDark),
  m_backgroundColor(KDColorWhite)
{
}

void BufferTableCell::setLabelText(const char * textBody) {
  assert(textBody);
  m_labelView.setText(textBody);
  layoutSubviews();
}

void BufferTableCell::appendText(const char * textBody) {
  assert(textBody);
  m_labelView.appendText(textBody);
  layoutSubviews();
}

void BufferTableCell::setTextColor(KDColor color) {
  m_labelView.setTextColor(color);
}

void BufferTableCell::setMessageFont(const KDFont * font) {
  m_labelView.setFont(font);
}

void BufferTableCell::setBackgroundColor(KDColor color) {
  m_backgroundColor = color;
  m_labelView.setBackgroundColor(color);
}

void BufferTableCell::layoutSubviews(bool force)  {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  if (width == 0 || height == 0) {
    return;
  }
  width -= k_separatorThickness + Metric::CellRightMargin;
  KDCoordinate x = k_separatorThickness + Metric::CellLeftMargin;
  /* With an enforced 23px height, no vertical margins are enforced. Vertical
   * centering is handled in BufferTextView. Additionally, an offset is added at
   * the top to lower the baseline of the buffet view to look more centered. */
  KDCoordinate topOffset = 1;
  height -= k_separatorThickness + topOffset;
  KDCoordinate y = k_separatorThickness + topOffset;

  assert(width > 0 && height > 0);

  m_labelView.setFrame(KDRect(x, y, width, height), force);
}

}
