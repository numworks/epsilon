#include "physical_quantity_cell.h"

namespace Elements {

PhysicalQuantityCell::PhysicalQuantityCell()
    : m_subLabelView(I18n::Message::Default,
                     Escher::CellWidget::Type::SubLabel) {}

void PhysicalQuantityCell::setLayouts(Poincare::Layout label,
                                      Poincare::Layout accessory) {
  m_labelView.setLayout(label);
  m_accessoryView.setLayout(accessory);
  if (!label.isUninitialized() || !accessory.isUninitialized()) {
    layoutSubviews();
  }
}

void PhysicalQuantityCell::setHighlighted(bool highlight) {
  Escher::TableCell::setHighlighted(highlight);
  KDColor backgroundColor = highlight ? Escher::Palette::Select : KDColorWhite;
  m_labelView.setBackgroundColor(backgroundColor);
  m_subLabelView.setBackgroundColor(backgroundColor);
  m_accessoryView.setBackgroundColor(backgroundColor);
}

}  // namespace Elements
