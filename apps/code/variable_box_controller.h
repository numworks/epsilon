#ifndef CODE_VARIABLE_BOX_CONTROLLER_H
#define CODE_VARIABLE_BOX_CONTROLLER_H

#include <escher.h>
#include "double_buffer_text_cell.h"
#include "menu_controller.h"
#include "script_store.h"

namespace Code {

class VariableBoxController : public StackViewController {
public:
  VariableBoxController(MenuController * menuController, ScriptStore * scriptStore);
  void didBecomeFirstResponder() override;
  void setTextFieldCaller(TextField * textField);
  void setTextAreaCaller(TextArea * textArea);
  void viewWillAppear() override;
  void viewDidDisappear() override;
private:
  class ContentViewController : public ViewController, public ListViewDataSource, public SelectableTableViewDataSource {
  public:
    ContentViewController(Responder * parentResponder, MenuController * menuController, ScriptStore * scriptStore);
    void setTextFieldCaller(TextField * textField);
    void setTextAreaCaller(TextArea * textArea);
    void reloadData();
    void resetDepth();

    void setFunctionsCountInScriptAtIndex(int functionsCount, int scriptIndex);
    void setFunctionNameAtIndex(const char * functionName, int functionIndex);
    void setGlobalVariablesCountInScriptAtIndex(int scriptIndex, int globalVariablesCount);
    void setGlobalVariableNameAtIndex(const char * globalVariableName, int globalVariableIndex);

    /* ViewController */
    const char * title() override;
    View * view() override { return &m_selectableTableView; }
    void viewWillAppear() override;
    void viewDidDisappear() override;

    /* Responder */
    void didBecomeFirstResponder() override;
    bool handleEvent(Ion::Events::Event event) override;

    /* ListViewDataSource */
    int numberOfRows() override;
    HighlightCell * reusableCell(int index, int type) override;
    int reusableCellCount(int type) override;
    void willDisplayCellForIndex(HighlightCell * cell, int index) override;
    KDCoordinate rowHeight(int j) override;
    KDCoordinate cumulatedHeightFromIndex(int j) override;
    int indexFromCumulatedHeight(KDCoordinate offsetY) override;
    int typeAtLocation(int i, int j) override;
  private:
    constexpr static int k_maxNumberOfDisplayedRows = 6; //240/40
    constexpr static int k_numberOfMenuRows = 2;
    constexpr static int k_nodeType = 0;
    constexpr static int k_leafType = 1;
    constexpr static int k_functionsAndVarsNamePointersArrayLength = 32;
    void insertTextInCaller(const char * text);
    int m_currentDepth;
    int m_firstSelectedRow;
    int m_previousSelectedRow;
    int m_scriptFunctionsCount;
    int m_scriptVariablesCount;

    int m_functionDefinitionsCount[ScriptStore::k_maxNumberOfScripts];
    int m_globalVariablesCount[ScriptStore::k_maxNumberOfScripts];
    const char * m_functionNamesPointers[k_functionsAndVarsNamePointersArrayLength];
    const char * m_globalVariablesNamesPointers[k_functionsAndVarsNamePointersArrayLength];

    MenuController * m_menuController;
    ScriptStore * m_scriptStore;
    TextField * m_textFieldCaller;
    TextArea * m_textAreaCaller;
    DoubleBufferTextCell m_leafCells[k_maxNumberOfDisplayedRows];
    MessageTableCellWithChevron m_nodeCells[k_numberOfMenuRows];
    SelectableTableView m_selectableTableView;
  };
  ContentViewController m_contentViewController;
};

}
#endif
