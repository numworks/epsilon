#ifndef CODE_MENU_CONTROLLER_H
#define CODE_MENU_CONTROLLER_H

#include <escher.h>
#include "console_controller.h"
#include "editor_controller.h"
#include <apps/shared/new_function_cell.h>
#include "program.h"

namespace Code {

class MenuController : public ViewController, public SimpleListViewDataSource, public SelectableTableViewDataSource, public ButtonRowDelegate {
public:
  MenuController(Responder * parentResponder, Program * program, ButtonRowController * footer);
  ConsoleController * consoleController();
  StackViewController * stackViewController() { return m_stackViewController; }

  /* ViewController */
  View * view() override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;

  /* SimpleListViewDataSource */
  int numberOfRows() override;
  KDCoordinate cellHeight() override;
  HighlightCell * reusableCell(int index) override;
  int reusableCellCount() override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;

  /* ButtonRowDelegate */
  int numberOfButtons(ButtonRowController::Position position) const override;
  Button * buttonAtIndex(int index, ButtonRowController::Position position) const override;
private:
  constexpr static int k_totalNumberOfCells = 2;
  MessageTableCell m_cells[k_totalNumberOfCells];
  Shared::NewFunctionCell m_addNewProgramCell;
  StackViewController * m_stackViewController;
  EditorController m_editorController;
  ConsoleController m_consoleController;
  SelectableTableView m_selectableTableView;
  Button m_consoleButton;
};

}

#endif
