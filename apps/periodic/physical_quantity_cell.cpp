#include "physical_quantity_cell.h"

namespace Periodic {

PhysicalQuantityCell::PhysicalQuantityCell() :
  m_subLabelView(KDFont::Size::Small, I18n::Message::Default, KDContext::k_alignLeft, KDContext::k_alignCenter, Escher::Palette::GrayDark, KDColorWhite)
{}

void PhysicalQuantityCell::setLayouts(Poincare::Layout label, Poincare::Layout accessory) {
  m_labelView.setLayout(label);
  m_accessoryView.setLayout(accessory);
  if (!label.isUninitialized() || !accessory.isUninitialized()) {
    layoutSubviews();
  }
}

void PhysicalQuantityCell::setHighlighted(bool highlight) {
  Escher::TableCell::setHighlighted(highlight);
  KDColor backgroundColor = highlight? Escher::Palette::Select : KDColorWhite;
  m_labelView.setBackgroundColor(backgroundColor);
  m_subLabelView.setBackgroundColor(backgroundColor);
  m_accessoryView.setBackgroundColor(backgroundColor);
}

}
