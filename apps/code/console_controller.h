#ifndef CODE_CONSOLE_CONTROLLER_H
#define CODE_CONSOLE_CONTROLLER_H

#include <escher.h>
#include <python/port/port.h>

#include "console_edit_cell.h"
#include "console_line_cell.h"
#include "console_store.h"
#include "sandbox_controller.h"
#include "script_store.h"

namespace Code {

class App;

class ConsoleController : public ViewController, public ListViewDataSource, public SelectableTableViewDataSource, public SelectableTableViewDelegate, public TextFieldDelegate, public MicroPython::ExecutionEnvironment {
public:
  ConsoleController(Responder * parentResponder, App * pythonDelegate, ScriptStore * scriptStore
#if EPSILON_GETOPT
      , bool m_lockOnConsole
#endif
      );

  bool loadPythonEnvironment();
  void unloadPythonEnvironment();

  void setAutoImport(bool autoImport) { m_autoImportScripts = autoImport; }
  void autoImport();
  void autoImportScript(Script script, bool force = false);
  void runAndPrintForCommand(const char * command);
  bool inputRunLoopActive() const { return m_inputRunLoopActive; }
  void terminateInputLoop();

  // ViewController
  View * view() override { return &m_selectableTableView; }
  void viewWillAppear() override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  ViewController::DisplayParameter displayParameter() override { return ViewController::DisplayParameter::WantsMaximumSpace; }
  TELEMETRY_ID("Console");

  // ListViewDataSource
  int numberOfRows() const override;
  KDCoordinate rowHeight(int j) override;
  KDCoordinate cumulatedHeightFromIndex(int j) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtLocation(int i, int j) override;
  void willDisplayCellAtLocation(HighlightCell * cell, int i, int j) override;

  // SelectableTableViewDelegate
  void tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) override;

  // TextFieldDelegate
  bool textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) override;
  bool textFieldDidAbortEditing(TextField * textField) override;

  // MicroPython::ExecutionEnvironment
  ViewController * sandbox() override { return &m_sandboxController; }
  void resetSandbox() override;
  void displayViewController(ViewController * controller) override;
  void hideAnyDisplayedViewController() override;
  void refreshPrintOutput() override;
  void printText(const char * text, size_t length) override;
  const char * inputText(const char * prompt) override;

#if EPSILON_GETOPT
  bool locked() const {
    return m_locked;
  }
#endif
private:
  static constexpr const char * k_importCommand1 = "from ";
  static constexpr const char * k_importCommand2 = " import *";
  static constexpr size_t k_maxImportCommandSize = 5 + 9 + TextField::maxBufferSize(); // strlen(k_importCommand1) + strlen(k_importCommand2) + TextField::maxBufferSize()
  static constexpr int LineCellType = 0;
  static constexpr int EditCellType = 1;
  static constexpr int k_numberOfLineCells = (Ion::Display::Height - Metric::TitleBarHeight) / 14 + 2; // 14 = KDFont::SmallFont->glyphSize().height()
  // k_numberOfLineCells = (240 - 18)/14 ~ 15.9. The 0.1 cell can be above and below the 15 other cells so we add +2 cells.
  static constexpr int k_outputAccumulationBufferSize = 100;
  bool isDisplayingViewController();
  void reloadData(bool isEditing);
  void flushOutputAccumulationBufferToStore();
  void appendTextToOutputAccumulationBuffer(const char * text, size_t length);
  void emptyOutputAccumulationBuffer();
  size_t firstNewLineCharIndex(const char * text, size_t length);
  StackViewController * stackViewController();
  App * m_pythonDelegate;
  bool m_importScriptsWhenViewAppears;
  ConsoleStore m_consoleStore;
  SelectableTableView m_selectableTableView;
  ConsoleLineCell m_cells[k_numberOfLineCells];
  ConsoleEditCell m_editCell;
  char m_outputAccumulationBuffer[k_outputAccumulationBufferSize];
  /* The Python machine might call printText several times to print a single
   * string. We thus use m_outputAccumulationBuffer to store and concatenate the
   * different strings until a new line char appears in the text. When this
   * happens, or when m_outputAccumulationBuffer is full, we create a new
   * ConsoleLine in the ConsoleStore and empty m_outputAccumulationBuffer. */
  ScriptStore * m_scriptStore;
  SandboxController m_sandboxController;
  bool m_inputRunLoopActive;
  bool m_autoImportScripts;
#if EPSILON_GETOPT
  bool m_locked;
#endif
};
}

#endif
