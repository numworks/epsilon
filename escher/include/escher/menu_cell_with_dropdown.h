#ifndef ESCHER_MENU_CELL_WITH_DROPDOWN_H
#define ESCHER_MENU_CELL_WITH_DROPDOWN_H

#include <escher/container.h>
#include <escher/dropdown_view.h>
#include <escher/menu_cell.h>

namespace Escher {

class DropdownWidget : public CellWidget {
 public:
  const View* view() const override { return m_dropdown; }
  void setDropdown(Dropdown* dropdown) { m_dropdown = dropdown; }

 private:
  Dropdown* m_dropdown;
};

template <typename Label, typename SubLabel = EmptyCellWidget>
class MenuCellWithDropdown : public MenuCell<Label, SubLabel, DropdownWidget>,
                             public Responder {
 public:
  MenuCellWithDropdown(Responder* parentResponder,
                       ListViewDataSource* listDataSource,
                       DropdownCallback* callback = nullptr)
      : MenuCell<Label, SubLabel, DropdownWidget>(),
        Responder(parentResponder),
        m_dropdown(this, listDataSource, callback) {
    // "this->" is required because of template
    this->m_accessory.setDropdown(&m_dropdown);
  }

  Dropdown* dropdown() { return &m_dropdown; }
  void reload() {
    // "this->" is required because of template
    m_dropdown.reloadAllCells();
    this->markRectAsDirty(this->bounds());
    this->layoutSubviews();
  }

  // Responder
  Responder* responder() override { return this; }
  void didBecomeFirstResponder() override {
    Container::activeApp()->setFirstResponder(&m_dropdown);
  }

  // Abstract menu cell
  void setHighlighted(bool highlight) override {
    m_dropdown.setHighlighted(highlight);
    MenuCell<Label, SubLabel, DropdownWidget>::setHighlighted(highlight);
  }
  bool shouldAlignSublabelRight() const override { return false; }
  bool forceAlignLabelAndAccessory() const override { return true; }

 private:
  Dropdown m_dropdown;
};

}  // namespace Escher

#endif
