#ifndef PERIODIC_DISPLAY_TYPE_CONTROLLER_H
#define PERIODIC_DISPLAY_TYPE_CONTROLLER_H

#include "elements_view_data_source.h"
#include <escher/message_table_cell.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>

namespace Periodic {

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
  constexpr static size_t k_numberOfRows = ElementsViewDataSource::k_numberOfDisplayTypes;
  constexpr static size_t k_numberOfCells = 7;
  constexpr static const DisplayType * k_displayTypes[k_numberOfRows] = {
    &ElementsViewDataSource::ColorByGroups,
    &ElementsViewDataSource::ColorByBlocks,
    &ElementsViewDataSource::ColorByMetals,
    &ElementsViewDataSource::ColorByMass,
    &ElementsViewDataSource::ColorByElectronegativity,
    &ElementsViewDataSource::ColorByMeltingPoint,
    &ElementsViewDataSource::ColorByBoilingPoint,
    &ElementsViewDataSource::ColorByRadius,
  };

  Escher::StackViewController * stackViewController() const { return static_cast<Escher::StackViewController *>(parentResponder()); }

  Escher::MessageTableCell m_cells[k_numberOfCells];
};

}

#endif
