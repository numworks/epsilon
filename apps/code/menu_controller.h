#ifndef CODE_MENU_CONTROLLER_H
#define CODE_MENU_CONTROLLER_H

#include <escher.h>
#include <apps/shared/new_function_cell.h>
#include "console_controller.h"
#include "script_parameter_controller.h"
#include "script_store.h"

namespace Code {

class ScriptParameterController;

class MenuController : public ViewController, public ListViewDataSource, public SelectableTableViewDataSource, public TextFieldDelegate, public ButtonRowDelegate {
public:
  MenuController(Responder * parentResponder, ScriptStore * scriptStore, ButtonRowController * footer);

  ConsoleController * consoleController() { return &m_consoleController; }
  StackViewController * stackViewController();
  void configureScript();
  void setParameteredScript();
  void addScript();
  void renameScriptAtIndex(int i);
  void deleteScriptAtIndex(int i);
  void reloadConsole();

  /* ViewController */
  View * view() override { return &m_selectableTableView; }
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;

  /* ListViewDataSource */
  int numberOfRows() override;
  KDCoordinate cellHeight() { return k_rowHeight; }
  KDCoordinate rowHeight(int j) override;
  KDCoordinate cumulatedHeightFromIndex(int j) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtLocation(int i, int j) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;

  /* TextFieldDelegate */
  bool textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) override;
  bool textFieldDidAbortEditing(TextField * textField, const char * text) override;
  Toolbox * toolboxForTextField(TextField * textFied) override { return nullptr; }

  /* ButtonRowDelegate */
  int numberOfButtons(ButtonRowController::Position position) const override { return 1; }
  Button * buttonAtIndex(int index, ButtonRowController::Position position) const override {
    assert(index == 0);
    return const_cast<Button *>(&m_consoleButton);
  }

private:
  static constexpr int k_maxNumberOfDisplayableScriptCells = 7; //TODO
  static constexpr int AddScriptCellType = 0;
  static constexpr int ScriptCellType = 1;
  static constexpr KDCoordinate k_rowHeight = 50; //TODO create common parent class with Shared::ListController
  ScriptStore * m_scriptStore;
  EvenOddEditableTextCell m_scriptCells[k_maxNumberOfDisplayableScriptCells];
  /* In the initializer list of the MenuController constructor, we initialize
   * m_scriptCells by copying k_maxNumberOfDisplayableScriptCells times the
   * constructor of an EvenOddEditableTextCell. */
  char m_draftTextBuffer[TextField::maxBufferSize()];
  Shared::NewFunctionCell m_addNewScriptCell;
  Button m_consoleButton;
  SelectableTableView m_selectableTableView;
  ConsoleController m_consoleController;
  ScriptParameterController m_scriptParameterController;
};

}

#endif
