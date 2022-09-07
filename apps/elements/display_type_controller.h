#ifndef ELEMENTS_DISPLAY_TYPE_CONTROLLER_H
#define ELEMENTS_DISPLAY_TYPE_CONTROLLER_H

#include "elements_view_data_source.h"
#include <escher/message_table_cell.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>

namespace Elements {

class DisplayTypeController : public Escher::SelectableListViewController<Escher::SimpleListViewDataSource> {
public:
  DisplayTypeController(Escher::StackViewController * stackController);

  // Escher::Responder
  bool handleEvent(Ion::Events::Event e) override;
  void didBecomeFirstResponder() override { Escher::Container::activeApp()->setFirstResponder(&m_selectableTableView); }

  // Escher::ViewController
  const char * title() override { return I18n::translate(I18n::Message::DisplayTypeTitle); }
  void viewWillAppear() override;

  // Escher::SelectableListViewController
  Escher::HighlightCell * reusableCell(int index) override { assert(0 <= index && index < k_numberOfCells); return m_cells + index; };
  int reusableCellCount() const override { return k_numberOfCells; };

  // Escher::TableViewDataSource
  int numberOfRows() const override { return k_numberOfRows; }

  // Escher::ListViewDataSource
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;


private:
  constexpr static size_t k_numberOfRows = 8;
  constexpr static size_t k_numberOfCells = 7;
  constexpr static const DataField * k_fields[k_numberOfRows] = {
    &ElementsDataBase::GroupField,
    &ElementsDataBase::BlockField,
    &ElementsDataBase::MetalField,
    &ElementsDataBase::MassField,
    &ElementsDataBase::ElectronegativityField,
    &ElementsDataBase::MeltingPointField,
    &ElementsDataBase::BoilingPointField,
    &ElementsDataBase::RadiusField,
  };

  Escher::StackViewController * stackViewController() const { return static_cast<Escher::StackViewController *>(parentResponder()); }

  Escher::MessageTableCell m_cells[k_numberOfCells];
};

}

#endif
