#ifndef SETTINGS_MAIN_CONTROLLER_H
#define SETTINGS_MAIN_CONTROLLER_H

#include <escher.h>
#include "sub_controller.h"
#include "settings_node.h"
#include "menu_cell.h"
#include "../preference.h"

namespace Settings {

class MainController : public ViewController, public SimpleListViewDataSource {
public:
  MainController(Responder * parentResponder, Preference * preference);

  View * view() override;
  const char * title() const override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  int numberOfRows() override;
  KDCoordinate cellHeight() override;
  TableViewCell * reusableCell(int index) override;
  int reusableCellCount() override;
  void willDisplayCellForIndex(TableViewCell * cell, int index) override;
private:
  StackViewController * stackController() const;
  constexpr static int k_totalNumberOfCell = 5;
  MenuCell m_cells[k_totalNumberOfCell];
  SelectableTableView m_selectableTableView;
  Node * m_nodeModel;
  Preference * m_preference;
  SubController m_subController;
};

}

#endif
