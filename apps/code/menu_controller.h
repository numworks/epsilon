#ifndef CODE_MENU_CONTROLLER_H
#define CODE_MENU_CONTROLLER_H

#include <escher/button_row_controller.h>
#include <escher/even_odd_cell_with_ellipsis.h>
#include <escher/even_odd_message_text_cell.h>
#include <escher/regular_table_view_data_source.h>

#include "console_controller.h"
#include "editor_controller.h"
#include "script_name_cell.h"
#include "script_parameter_controller.h"
#include "script_store.h"

namespace Code {

class ScriptParameterController;

class MenuController : public Escher::ViewController,
                       public Escher::RegularHeightTableViewDataSource,
                       public Escher::SelectableTableViewDataSource,
                       public Escher::SelectableTableViewDelegate,
                       public Escher::TextFieldDelegate,
                       public Escher::ButtonRowDelegate {
 public:
  MenuController(Escher::Responder *parentResponder, App *pythonDelegate,
                 ScriptStore *scriptStore, Escher::ButtonRowController *footer);
  ConsoleController *consoleController();
  Escher::StackViewController *stackViewController();
  void willExitResponderChain(Escher::Responder *nextFirstResponder) override;
  void renameSelectedScript();
  void deleteScript(Script script);
  void reloadConsole();
  void openConsoleWithScript(Script script);
  void scriptContentEditionDidFinish();
  void willExitApp();
  int editedScriptIndex() const { return m_editorController.scriptIndex(); }

  /* ViewController */
  Escher::View *view() override { return &m_selectableTableView; }
  ViewController::TitlesDisplay titlesDisplay() override {
    return TitlesDisplay::NeverDisplayOwnTitle;
  }
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;
  TELEMETRY_ID("Menu");

  /* TableViewDataSource */
  int numberOfRows() const override;
  int numberOfColumns() const override { return 2; }
  void willDisplayCellAtLocation(Escher::HighlightCell *cell, int i,
                                 int j) override;
  Escher::HighlightCell *reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtLocation(int i, int j) override;
  void willDisplayScriptTitleCellForIndex(Escher::HighlightCell *cell,
                                          int index);
  KDCoordinate defaultRowHeight() override {
    return Escher::Metric::StoreRowHeight;
  }

  /* SelectableTableViewDelegate */
  void tableViewDidChangeSelectionAndDidScroll(
      Escher::SelectableTableView *t, int previousSelectedCol,
      int previousSelectedRow, bool withinTemporarySelection) override;

  /* TextFieldDelegate */
  bool textFieldShouldFinishEditing(Escher::AbstractTextField *textField,
                                    Ion::Events::Event event) override;
  bool textFieldDidReceiveEvent(Escher::AbstractTextField *textField,
                                Ion::Events::Event event) override {
    return false;
  }
  bool textFieldDidFinishEditing(Escher::AbstractTextField *textField,
                                 const char *text,
                                 Ion::Events::Event event) override;
  bool textFieldDidAbortEditing(Escher::AbstractTextField *textField) override {
    return privateTextFieldDidAbortEditing(textField, true);
  }
  bool textFieldDidHandleEvent(Escher::AbstractTextField *textField,
                               bool returnValue, bool textDidChange) override;

  /* ButtonRowDelegate */
  int numberOfButtons(
      Escher::ButtonRowController::Position position) const override {
    return 1;
  }
  Escher::AbstractButtonCell *buttonAtIndex(
      int index,
      Escher::ButtonRowController::Position position) const override {
    assert(index == 0);
    return const_cast<Escher::AbstractButtonCell *>(&m_consoleButton);
  }

 private:
  constexpr static int k_maxNumberOfDisplayableScriptCells = 5;  // = 240/50
  constexpr static int k_parametersColumnWidth =
      Escher::Metric::EllipsisCellWidth;
  constexpr static int AddScriptCellType = 0;
  constexpr static int ScriptCellType = 1;
  constexpr static int ScriptParameterCellType = 2;
  constexpr static int EmptyCellType = 3;
  // TableViewDataSource
  KDCoordinate nonMemoizedColumnWidth(int i) override;
  void addScript();
  void configureScript();
  void editScriptAtIndex(int scriptIndex);
  void updateAddScriptRowDisplay();
  bool privateTextFieldDidAbortEditing(
      Escher::AbstractTextField *textField,
      bool menuControllerStaysInResponderChain);
  void forceTextFieldEditionToAbort(bool menuControllerStaysInResponderChain);
  void privateModalViewAltersFirstResponder(
      FirstResponderAlteration alteration) override;
  ScriptStore *m_scriptStore;
  ScriptNameCell m_scriptCells[k_maxNumberOfDisplayableScriptCells];
  Escher::EvenOddCellWithEllipsis
      m_scriptParameterCells[k_maxNumberOfDisplayableScriptCells];
  Escher::EvenOddMessageTextCell m_addNewScriptCell;
  Escher::EvenOddCell m_emptyCell;
  Escher::AbstractButtonCell m_consoleButton;
  Escher::SelectableTableView m_selectableTableView;
  ScriptParameterController m_scriptParameterController;
  EditorController m_editorController;
  bool m_reloadConsoleWhenBecomingFirstResponder;
  bool m_shouldDisplayAddScriptRow;
};

}  // namespace Code

#endif
