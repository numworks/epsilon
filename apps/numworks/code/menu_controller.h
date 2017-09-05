#ifndef CODE_MENU_CONTROLLER_H
#define CODE_MENU_CONTROLLER_H

#include <escher.h>
#include "editor_controller.h"
#include "executor_controller.h"
#include "program.h"

namespace Code {

class MenuController : public ViewController, public SimpleListViewDataSource, public SelectableTableViewDataSource {
public:
  MenuController(Responder * parentResponder, Program * program);
  View * view() override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  int numberOfRows() override;
  KDCoordinate cellHeight() override;
  HighlightCell * reusableCell(int index) override;
  int reusableCellCount() override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
private:
  constexpr static int k_totalNumberOfCells = 2;
  MessageTableCell m_cells[k_totalNumberOfCells];
  SelectableTableView m_selectableTableView;
  EditorController m_editorController;
  ExecutorController m_executorController;
};

}

#endif
