#include "table_cells.h"

namespace Periodic {

// InertExpressionTableCell

InertExpressionTableCell::InertExpressionTableCell() :
  m_subLabelView(KDFont::Size::Small, I18n::Message::Default, KDContext::k_alignLeft, KDContext::k_alignCenter, Escher::Palette::GrayDark, KDColorWhite)
{}

void InertExpressionTableCell::setLayout(Poincare::Layout layout) {
  m_labelView.setLayout(layout);
  if (!layout.isUninitialized()) {
    layoutSubviews();
  }
}

void InertExpressionTableCell::setTextColor(KDColor color) {
  m_labelView.setTextColor(color);
  m_accessoryView.setTextColor(color);
}

void InertExpressionTableCell::setHighlighted(bool highlight) {
  Escher::TableCell::setHighlighted(highlight);
  KDColor backgroundColor = highlight? Escher::Palette::Select : KDColorWhite;
  m_labelView.setBackgroundColor(backgroundColor);
  m_subLabelView.setBackgroundColor(backgroundColor);
  m_accessoryView.setBackgroundColor(backgroundColor);
}

// MessageTableCellWithMessageWithExpression

void MessageTableCellWithMessageWithExpression::setHighlighted(bool highlight) {
  Escher::MessageTableCellWithMessage::setHighlighted(highlight);
  KDColor backgroundColor = highlight? Escher::Palette::Select : KDColorWhite;
  m_accessoryView.setBackgroundColor(backgroundColor);
}

void MessageTableCellWithMessageWithExpression::setAccessoryLayout(Poincare::Layout layout) {
  m_accessoryView.setLayout(layout);
  if (!layout.isUninitialized()) {
    layoutSubviews();
  }
}

}
