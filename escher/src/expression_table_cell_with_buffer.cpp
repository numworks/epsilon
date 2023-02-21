#include <escher/expression_table_cell_with_buffer.h>
#include <escher/palette.h>

namespace Escher {

ExpressionTableCellWithBuffer::ExpressionTableCellWithBuffer(
    Responder* responder)
    : ExpressionTableCell(responder),
      m_subLabelView(KDFont::Size::Small, KDContext::k_alignRight,
                     KDContext::k_alignCenter, Palette::GrayDark) {}

void ExpressionTableCellWithBuffer::setSubLabelText(const char* textBody) {
  m_subLabelView.setText(textBody);
  layoutSubviews();
}

const char* ExpressionTableCellWithBuffer::subLabelText() {
  return m_subLabelView.text();
}

void ExpressionTableCellWithBuffer::setHighlighted(bool highlight) {
  ExpressionTableCell::setHighlighted(highlight);
  m_subLabelView.setBackgroundColor(defaultBackgroundColor());
}

}  // namespace Escher
