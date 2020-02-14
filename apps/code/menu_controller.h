#ifndef CODE_MENU_CONTROLLER_H
#define CODE_MENU_CONTROLLER_H

#include <escher.h>
#include "console_controller.h"
#include "editor_controller.h"
#include "script_name_cell.h"
#include "script_parameter_controller.h"
#include "script_store.h"

namespace Code {

class ScriptParameterController;

class MenuController : public ViewController, public TableViewDataSource, public SelectableTableViewDataSource, public SelectableTableViewDelegate, public TextFieldDelegate, public ButtonRowDelegate {
public:
  MenuController(Responder * parentResponder, App * pythonDelegate, ScriptStore * scriptStore, ButtonRowController * footer);
  ConsoleController * consoleController();
  StackViewController * stackViewController();
  void willExitResponderChain(Responder * nextFirstResponder) override;
  void renameSelectedScript();
  void deleteScript(Script script);
  void reloadConsole();
  void openConsoleWithScript(Script script);
  void scriptContentEditionDidFinish();
  void willExitApp();

  /* ViewController */
  View * view() override { return &m_selectableTableView; }
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;
  TELEMETRY_ID("Menu");

  /* TableViewDataSource */
  int numberOfRows() const override;
  int numberOfColumns() const override { return 2; }
  void willDisplayCellAtLocation(HighlightCell * cell, int i, int j) override;
  KDCoordinate columnWidth(int i) override;
  KDCoordinate rowHeight(int j) override { return Metric::StoreRowHeight; }
  KDCoordinate cumulatedWidthFromIndex(int i) override;
  KDCoordinate cumulatedHeightFromIndex(int j) override;
  int indexFromCumulatedWidth(KDCoordinate offsetX) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtLocation(int i, int j) override;
  void willDisplayScriptTitleCellForIndex(HighlightCell * cell, int index);

  /* SelectableTableViewDelegate */
  void tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) override;

  /* TextFieldDelegate */
  bool textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) override { return false; }
  bool textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) override;
  bool textFieldDidAbortEditing(TextField * textField) override {
    return privateTextFieldDidAbortEditing(textField, true);
  }
  bool textFieldDidHandleEvent(TextField * textField, bool returnValue, bool textSizeDidChange) override;

  /* ButtonRowDelegate */
  int numberOfButtons(ButtonRowController::Position position) const override { return 1; }
  Button * buttonAtIndex(int index, ButtonRowController::Position position) const override {
    assert(index == 0);
    return const_cast<Button *>(&m_consoleButton);
  }

private:
  static constexpr int k_maxNumberOfDisplayableScriptCells = 5; // = 240/50
  static constexpr int k_parametersColumnWidth = Metric::EllipsisCellWidth;
  static constexpr int AddScriptCellType = 0;
  static constexpr int ScriptCellType = 1;
  static constexpr int ScriptParameterCellType = 2;
  static constexpr int EmptyCellType = 3;
  void addScript();
  void configureScript();
  void editScriptAtIndex(int scriptIndex);
  void numberedDefaultScriptName(char * buffer);
  void updateAddScriptRowDisplay();
  bool privateTextFieldDidAbortEditing(TextField * textField, bool menuControllerStaysInResponderChain);
  ScriptStore * m_scriptStore;
  ScriptNameCell m_scriptCells[k_maxNumberOfDisplayableScriptCells];
  EvenOddCellWithEllipsis m_scriptParameterCells[k_maxNumberOfDisplayableScriptCells];
  EvenOddMessageTextCell m_addNewScriptCell;
  EvenOddCell m_emptyCell;
  Button m_consoleButton;
  SelectableTableView m_selectableTableView;
  ScriptParameterController m_scriptParameterController;
  EditorController m_editorController;
  bool m_reloadConsoleWhenBecomingFirstResponder;
  bool m_shouldDisplayAddScriptRow;
};

}

#endif
