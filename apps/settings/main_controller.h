#ifndef SETTINGS_MAIN_CONTROLLER_H
#define SETTINGS_MAIN_CONTROLLER_H

#include <escher.h>
#include "sub_controller.h"
#include "settings_node.h"

namespace Settings {

class MainController : public ViewController, public ListViewDataSource {
public:
  MainController(Responder * parentResponder);
  ~MainController();
  View * view() override;
  const char * title() override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  int numberOfRows() override;
  KDCoordinate rowHeight(int j) override;
  KDCoordinate cumulatedHeightFromIndex(int j) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtLocation(int i, int j) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  void viewWillAppear() override;
private:
  StackViewController * stackController() const;
  constexpr static int k_totalNumberOfCell = 4;
  PointerTableCellWithChevronAndPointer m_cells[k_totalNumberOfCell-1];
  PointerTableCellWithChevronAndExpression m_complexFormatCell;
  Poincare::ExpressionLayout * m_complexFormatLayout;
  SelectableTableView m_selectableTableView;
  Node * m_nodeModel;
  SubController m_subController;
};

}

#endif
