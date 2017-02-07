#ifndef SETTINGS_MAIN_CONTROLLER_H
#define SETTINGS_MAIN_CONTROLLER_H

#include <escher.h>
#include "sub_controller.h"
#include "settings_node.h"
#include "../preferences.h"

namespace Settings {

class MainController : public ViewController, public SimpleListViewDataSource {
public:
  MainController(Responder * parentResponder, Preferences * preferences);

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
  ChevronTextMenuListCell m_cells[k_totalNumberOfCell];
  SelectableTableView m_selectableTableView;
  Node * m_nodeModel;
  Preferences * m_preferences;
  SubController m_subController;
};

}

#endif
