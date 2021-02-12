#include <escher/message_table_cell_with_editable_text.h>
#include <escher/palette.h>
#include <escher/container.h>

namespace Escher {

MessageTableCellWithEditableText::MessageTableCellWithEditableText(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, TextFieldDelegate * textFieldDelegate, I18n::Message message) :
  Responder(parentResponder),
  MessageTableCell(message),
  m_textField(this, m_textBody, Poincare::PrintFloat::k_maxFloatCharSize, TextField::maxBufferSize(), inputEventHandlerDelegate, textFieldDelegate, KDFont::LargeFont, 1.0f, 0.5f, KDColorBlack)
{
  m_textBody[0] = '\0';
}

const char * MessageTableCellWithEditableText::editedText() const {
  return m_textField.text();
}

void MessageTableCellWithEditableText::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(&m_textField);
}

bool MessageTableCellWithEditableText::isEditing() {
  return m_textField.isEditing();
}

void MessageTableCellWithEditableText::setEditing(bool isEditing) {
  m_textField.setEditing(isEditing);
}

void MessageTableCellWithEditableText::setHighlighted(bool highlight) {
  MessageTableCell::setHighlighted(highlight);
  KDColor backgroundColor = highlight? Palette::Select : KDColorWhite;
  m_textField.setBackgroundColor(backgroundColor);
}

void MessageTableCellWithEditableText::setSubLabelText(const char * text) {
  m_textField.setText(text);
  layoutSubviews();
}

/* Overriding this function with a nullptr subLabel because m_textField should
 * not force a two row table cell while the user writes. */
KDSize MessageTableCellWithEditableText::minimalSizeForOptimalDisplay() const {
  // Available width is necessary to compute it minimal height.
  KDCoordinate expectedWidth = m_frame.width();
  assert(expectedWidth > 0);
  return KDSize(expectedWidth, minimalHeightForOptimalDisplay(labelView(), nullptr, accessoryView(), expectedWidth));
}

void MessageTableCellWithEditableText::layoutSubviews(bool force) {
  if (bounds().width() == 0 || bounds().height() == 0) {
    return;
  }
  TableCell::layoutSubviews(force);
  KDSize labelSize = labelView()->minimalSizeForOptimalDisplay();
  /* Handle TextField that has no defined width (as their width evolves with
   * the length of edited text */
  KDCoordinate marginsAndLabelHorizontalOffset = k_separatorThickness + Metric::CellLeftMargin + labelSize.width() + Metric::CellHorizontalElementMargin;
  KDCoordinate marginsVerticalOffset = k_separatorThickness + Metric::CellTopMargin;

  assert(!accessoryView());
  assert(m_textField.minimalSizeForOptimalDisplay().width() <= bounds().width() - marginsAndLabelHorizontalOffset - Metric::CellRightMargin - k_separatorThickness);
  assert(m_textField.minimalSizeForOptimalDisplay().height() <= bounds().height() - marginsVerticalOffset - Metric::CellBottomMargin - k_separatorThickness);

  m_textField.setFrame(KDRect(
      marginsAndLabelHorizontalOffset,
      marginsVerticalOffset,
      bounds().width() - marginsAndLabelHorizontalOffset - Metric::CellRightMargin - k_separatorThickness,
      bounds().height() - marginsVerticalOffset - Metric::CellBottomMargin - k_separatorThickness)
    , force);
}

}
