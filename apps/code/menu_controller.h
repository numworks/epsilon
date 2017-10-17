#ifndef CODE_MENU_CONTROLLER_H
#define CODE_MENU_CONTROLLER_H

#include <escher.h>
#include "console_controller.h"
#include "program_parameter_controller.h"
#include <apps/shared/new_function_cell.h>
#include "program_store.h"

namespace Code {

class MenuController : public ViewController, public SimpleListViewDataSource, public SelectableTableViewDataSource, public ButtonRowDelegate {
public:
  MenuController(Responder * parentResponder, ProgramStore * programStore, ButtonRowController * footer);
  ConsoleController * consoleController();
  StackViewController * stackViewController();
  void configureProgram();
  void addProgram();

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
  constexpr static int k_maxNumberOfCells = 5; //TODO
  static constexpr KDCoordinate k_rowHeight = 50; //TODO create common parent class with Shared::ListController
  ProgramStore * m_programStore;
  MessageTableCell m_cells[k_maxNumberOfCells];
  Shared::NewFunctionCell m_addNewProgramCell;
  Button m_consoleButton;
  SelectableTableView m_selectableTableView;
  ConsoleController m_consoleController;
  ProgramParameterController m_programParameterController;
};

}

#endif
