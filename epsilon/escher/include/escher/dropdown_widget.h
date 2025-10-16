#pragma once

#include <escher/container.h>
#include <escher/dropdown_view.h>
#include <escher/menu_cell.h>

namespace Escher {

/* Widgets need a default constructor, which is not the case for dropdowns. This
 * widget allows the controller to own the dropdown, initialize it, and then
 * pass it to the widget. */
class DropdownWidget : public CellWidget {
 public:
  void setDropdown(Dropdown* dropdown) { m_dropdown = dropdown; }

  // CellWidget
  const View* view() const override {
    assert(m_dropdown != nullptr);
    return m_dropdown;
  }
  Responder* responder() override {
    assert(m_dropdown != nullptr);
    return m_dropdown;
  }
  void setHighlighted(bool highlighted) override {
    assert(m_dropdown != nullptr);
    m_dropdown->setHighlighted(highlighted);
  }
  bool alwaysAlignWithLabelAsAccessory() const override { return true; }
  bool preventRightAlignedSubLabel(Type type) const override {
    return type == Type::Accessory;
  }

 private:
  Dropdown* m_dropdown = nullptr;
};

}  // namespace Escher
