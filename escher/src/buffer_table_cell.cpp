#include <escher/buffer_table_cell.h>
#include <escher/palette.h>
#include <apps/i18n.h>
#include <assert.h>

namespace Escher {

constexpr KDColor BufferTableCell::k_backgroundColor;
constexpr KDColor BufferTableCell::k_textColor;

BufferTableCell::BufferTableCell() :
    TableCell(),
    // Text is aligned to the left, vertically centered and has a small font.
    m_labelView(KDFont::SmallFont,
                KDContext::k_alignLeft,
                KDContext::k_alignCenter,
                k_textColor,
                k_backgroundColor,
                k_maxNumberOfCharsInBuffer) {
}

void BufferTableCell::setLabelText(const char * textBody) {
  assert(textBody);
  m_labelView.setText(textBody);
  layoutSubviews();
}

void BufferTableCell::setMessageAndStringAsLabelText(I18n::Message message, const char * string) {
  m_labelView.setMessageAndStringAsText(message, string);
}

void BufferTableCell::appendText(const char * textBody) {
  assert(textBody);
  m_labelView.appendText(textBody);
  layoutSubviews();
}

void BufferTableCell::layoutSubviews(bool force)  {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  if (width == 0 || height == 0) {
    return;
  }
  KDCoordinate x = k_separatorThickness + Metric::CellLeftMargin;
  width -= x + Metric::CellRightMargin + k_separatorThickness;
  /* With an enforced 24px height, no vertical margins are enforced. Vertical
   * centering is handled in BufferTextView. Additionally, an offset is added at
   * the top to lower the baseline of the buffer view to look more centered. */
  KDCoordinate topOffset = 2;
  KDCoordinate y = k_separatorThickness + topOffset;
  height -= y + k_separatorThickness;

  assert(width > 0 && height > 0);

  m_labelView.setFrame(KDRect(x, y, width, height), force);
}

}
