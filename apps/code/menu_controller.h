#ifndef CODE_MENU_CONTROLLER_H
#define CODE_MENU_CONTROLLER_H

#include <escher.h>
#include <apps/shared/new_function_cell.h>
#include "console_controller.h"
#include "editor_controller.h"
#include "script_parameter_controller.h"
#include "script_store.h"

namespace Code {

class ScriptParameterController;

class MenuController : public ViewController, public TableViewDataSource, public SelectableTableViewDataSource, public SelectableTableViewDelegate, public TextFieldDelegate, public ButtonRowDelegate {
public:
  MenuController(Responder * parentResponder, ScriptStore * scriptStore, ButtonRowController * footer
#if EPSILON_GETOPT
      , bool m_lockOnConsole
#endif
      );
  ConsoleController * consoleController() { return &m_consoleController; }
  StackViewController * stackViewController();
  void willExitResponderChain(Responder * nextFirstResponder) override;
  void renameSelectedScript();
  void deleteScript(Script script);
  void reloadConsole();
  void loadPythonIfNeeded();
  void openConsoleWithScript(Script script);
  void scriptContentEditionDidFinish();

  /* ViewController */
  View * view() override { return &m_selectableTableView; }
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;

  /* TableViewDataSource */
  int numberOfRows() override;
  int numberOfColumns() override { return 2; }
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
  void tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY) override;

  /* TextFieldDelegate */
  bool textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) override;
  bool textFieldDidAbortEditing(TextField * textField, const char * text) override;
  bool textFieldDidHandleEvent(TextField * textField, bool returnValue, bool textHasChanged) override;
  Toolbox * toolboxForTextInput(TextInput * textInput) override { return nullptr; }

  /* ButtonRowDelegate */
  int numberOfButtons(ButtonRowController::Position position) const override { return 1; }
  Button * buttonAtIndex(int index, ButtonRowController::Position position) const override {
    assert(index == 0);
    return const_cast<Button *>(&m_consoleButton);
  }

private:
  static constexpr int k_maxNumberOfDisplayableScriptCells = 5; // = 240/50
  static constexpr int k_parametersColumnWidth = 37;
  static constexpr int AddScriptCellType = 0;
  static constexpr int ScriptCellType = 1;
  static constexpr int ScriptParameterCellType = 2;
  static constexpr int EmptyCellType = 3;
  static constexpr int k_defaultScriptNameMaxSize = 9 + 2 + 1;
  // k_defaultScriptNameMaxSize is the length of a name between script1.py and
  // script99.py.
  // 9 = strlen("script.py")
  // 2 = maxLength of integers between 1 and 99.
  // 1 = length of null terminating char.
  void addScript();
  void configureScript();
  void editScriptAtIndex(int scriptIndex);
  void numberedDefaultScriptName(char * buffer);
  void intToText(int i, char * buffer);
  void updateAddScriptRowDisplay();
  ScriptStore * m_scriptStore;
  class EvenOddEditableTextCell : public ::EvenOddEditableTextCell {
  public:
    Responder * responder() override {
      if (editableTextCell()->textField()->isEditing()) {
        return this;
      }
      return nullptr;
    }
  };
  EvenOddEditableTextCell m_scriptCells[k_maxNumberOfDisplayableScriptCells];
  /* In the initializer list of the MenuController constructor, we initialize
   * m_scriptCells by copying k_maxNumberOfDisplayableScriptCells times the
   * constructor of an EvenOddEditableTextCell. */
  char m_draftTextBuffer[TextField::maxBufferSize()];
  EvenOddCellWithEllipsis m_scriptParameterCells[k_maxNumberOfDisplayableScriptCells];
  Shared::NewFunctionCell m_addNewScriptCell;
  EvenOddCell m_emptyCell;
  Button m_consoleButton;
  SelectableTableView m_selectableTableView;
  ConsoleController m_consoleController;
  ScriptParameterController m_scriptParameterController;
  EditorController m_editorController;
  bool m_reloadConsoleWhenBecomingFirstResponder;
  bool m_shouldDisplayAddScriptRow;
};

}

#endif
