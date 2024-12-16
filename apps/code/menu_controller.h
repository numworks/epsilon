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
  MenuController(Escher::Responder* parentResponder, App* pythonDelegate,
                 Escher::ButtonRowController* footer);
  ConsoleController* consoleController();
  Escher::StackViewController* stackViewController();
  void willExitResponderChain(Escher::Responder* nextFirstResponder) override;
  void renameSelectedScript();
  void deleteScript(Script script);
  void reloadConsole();
  void openConsoleWithScript(Script script);
  void scriptContentEditionDidFinish();
  void willExitApp();
  int editedScriptIndex() const { return m_editorController.scriptIndex(); }

  /* ViewController */
  Escher::View* view() override { return &m_selectableTableView; }
  ViewController::TitlesDisplay titlesDisplay() const override {
    return TitlesDisplay::NeverDisplayOwnTitle;
  }
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;

  /* TableViewDataSource */
  int numberOfRows() const override;
  int numberOfColumns() const override { return 2; }
  void fillCellForLocation(Escher::HighlightCell* cell, int column,
                           int row) override;
  Escher::HighlightCell* reusableCell(int index, int type) override;
  int reusableCellCount(int type) const override;
  int typeAtLocation(int column, int row) const override;
  KDCoordinate defaultRowHeight() override {
    return Escher::Metric::StoreRowHeight;
  }

  /* SelectableTableViewDelegate */
  void tableViewDidChangeSelectionAndDidScroll(
      Escher::SelectableTableView* t, int previousSelectedCol,
      int previousSelectedRow, KDPoint previousOffset,
      bool withinTemporarySelection) override;

  /* TextFieldDelegate */
  bool textFieldShouldFinishEditing(Escher::AbstractTextField* textField,
                                    Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(Escher::AbstractTextField* textField,
                                 Ion::Events::Event event) override;
  void textFieldDidAbortEditing(Escher::AbstractTextField* textField) override {
    privateTextFieldDidAbortEditing(textField, true);
  }
  void textFieldDidHandleEvent(Escher::AbstractTextField* textField) override;

  /* ButtonRowDelegate */
  int numberOfButtons(
      Escher::ButtonRowController::Position position) const override {
    return 1;
  }
  Escher::ButtonCell* buttonAtIndex(
      int index,
      Escher::ButtonRowController::Position position) const override {
    assert(index == 0);
    return const_cast<Escher::ButtonCell*>(&m_consoleButton);
  }

 private:
  constexpr static int k_maxNumberOfDisplayableScriptCells = 5;  // = 240/50
  constexpr static int k_parametersColumnWidth =
      Escher::Metric::EllipsisCellWidth;
  constexpr static int k_addScriptCellType = 0;
  constexpr static int k_scriptCellType = 1;
  constexpr static int k_scriptParameterCellType = 2;
  constexpr static int k_emptyCellType = 3;
  // TableViewDataSource
  KDCoordinate nonMemoizedColumnWidth(int column) override;
  void addScript();
  void configureScript();
  void editScriptAtIndex(int scriptIndex);
  void updateAddScriptRowDisplay();
  void privateTextFieldDidAbortEditing(
      Escher::AbstractTextField* textField,
      bool menuControllerStaysInResponderChain);
  void forceTextFieldEditionToAbort(bool menuControllerStaysInResponderChain);
  void privateModalViewAltersFirstResponder(
      FirstResponderAlteration alteration) override;
  ScriptNameCell m_scriptCells[k_maxNumberOfDisplayableScriptCells];
  Escher::EvenOddCellWithEllipsis
      m_scriptParameterCells[k_maxNumberOfDisplayableScriptCells];
  Escher::EvenOddMessageTextCell m_addNewScriptCell;
  Escher::EvenOddCell m_emptyCell;
  Escher::ButtonCell m_consoleButton;
  Escher::SelectableTableView m_selectableTableView;
  ScriptParameterController m_scriptParameterController;
  EditorController m_editorController;
  bool m_reloadConsoleWhenBecomingFirstResponder;
  bool m_shouldDisplayAddScriptRow;
};

}  // namespace Code

#endif
