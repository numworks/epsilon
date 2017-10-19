#ifndef CODE_CONSOLE_CONTROLLER_H
#define CODE_CONSOLE_CONTROLLER_H

#include <escher.h>
#include <python/port/port.h>

#include "console_edit_cell.h"
#include "console_line_cell.h"
#include "console_store.h"
#include "script_store.h"

namespace Code {

class ConsoleController : public ViewController, public ListViewDataSource, public ScrollViewDataSource, public TextFieldDelegate, public MicroPython::ExecutionEnvironment {
public:
  ConsoleController(Responder * parentResponder, ScriptStore * scriptStore);
  ~ConsoleController();
  ConsoleController(const ConsoleController& other) = delete;
  ConsoleController(ConsoleController&& other) = delete;
  ConsoleController operator=(const ConsoleController& other) = delete;
  ConsoleController& operator=(ConsoleController&& other) = delete;

  static constexpr KDText::FontSize k_fontSize = KDText::FontSize::Large;

  // ViewController
  View * view() override { return &m_tableView; }
  void viewWillAppear() override;
  void didBecomeFirstResponder() override;

  // ListViewDataSource
  int numberOfRows() override;
  KDCoordinate rowHeight(int j) override;
  KDCoordinate cumulatedHeightFromIndex(int j) override;
  int indexFromCumulatedHeight(KDCoordinate offsetY) override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtLocation(int i, int j) override;
  void willDisplayCellAtLocation(HighlightCell * cell, int i, int j) override;

  // TextFieldDelegate
  bool textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) override;
  bool textFieldDidAbortEditing(TextField * textField, const char * text) override;
  Toolbox * toolboxForTextField(TextField * textFied) override;

  // MicroPython::ExecutionEnvironment
  void printText(const char * text, size_t length) override;

private:
  static constexpr int LineCellType = 0;
  static constexpr int EditCellType = 1;
  static constexpr int k_numberOfLineCells = 15; // May change depending on the screen height
  static constexpr int k_pythonHeapSize = 16384;
  static constexpr int k_outputAccumulationBufferSize = 40;
  void flushOutputAccumulationBufferToStore();
  void appendTextToOutputAccumulationBuffer(const char * text, size_t length);
  void emptyOutputAccumulationBuffer();
  int firstNewLineCharIndex(const char * text, size_t length);
  int m_rowHeight;
  ConsoleStore m_consoleStore;
  TableView m_tableView;
  ConsoleLineCell m_cells[k_numberOfLineCells];
  ConsoleEditCell m_editCell;
  char * m_pythonHeap;
  char * m_outputAccumulationBuffer;
  /* The Python machine might call printText several times to print a single
   * string. We thus use m_outputAccumulationBuffer to store and concatenate the
   * different strings until a new line char appears in the text. When this
   * happens, or when m_outputAccumulationBuffer is full, we create a new
   * ConsoleLine in the ConsoleStore and empty m_outputAccumulationBuffer. */
};
}

#endif
