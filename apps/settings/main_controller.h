#ifndef SETTINGS_MAIN_CONTROLLER_H
#define SETTINGS_MAIN_CONTROLLER_H

#include <escher.h>
#include "sub_controller.h"
#include "settings_node.h"

namespace Settings {

class MainController : public ViewController, public SimpleListViewDataSource {
public:
  MainController(Responder * parentResponder);

  View * view() override;
  const char * title() const override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  int numberOfRows() override;
  KDCoordinate cellHeight() override;
  HighlightCell * reusableCell(int index) override;
  int reusableCellCount() override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  void viewWillAppear() override;
private:
  StackViewController * stackController() const;
  constexpr static int k_totalNumberOfCell = 5;
  PointerTableCellWithChevronAndPointer m_cells[k_totalNumberOfCell];
  SelectableTableView m_selectableTableView;
  Node * m_nodeModel;
  SubController m_subController;
};

}

#endif
