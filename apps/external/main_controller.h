#ifndef EXTERNAL_MAIN_CONTROLLER_H
#define EXTERNAL_MAIN_CONTROLLER_H

#include <escher.h>
#include "pointer_text_table_cell.h"

namespace External {

class MainController : public ViewController, public ListViewDataSource, public SelectableTableViewDataSource {
public:
  MainController(Responder * parentResponder, App * app);
  View * view() override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  int numberOfRows() const override;
  KDCoordinate rowHeight(int j) override;
  KDCoordinate cumulatedHeightFromIndex(int j) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtLocation(int i, int j) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  void viewWillAppear() override;
private:
  App * m_app;
  SelectableTableView m_selectableTableView;
  int k_numberOfCells = 1;
  constexpr static int k_maxNumberOfCells = 16;
  PointerTextTableCell m_cells[k_maxNumberOfCells];
};

}

#endif
