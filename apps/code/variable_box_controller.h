#ifndef CODE_VARIABLE_BOX_CONTROLLER_H
#define CODE_VARIABLE_BOX_CONTROLLER_H

#include <escher.h>
#include "menu_controller.h"
#include "script_node.h"
#include "script_node_cell.h"
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

    void addFunctionAtIndex(const char * functionName, int scriptIndex);
    void addVariableAtIndex(const char * variableName, int scriptIndex);

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
    constexpr static int k_leafType = 0;
    constexpr static int k_maxScriptNodesCount = 32;
    void insertTextInCaller(const char * text);
    int m_scriptNodesCount;
    ScriptNode m_scriptNodes[k_maxScriptNodesCount];
    MenuController * m_menuController;
    ScriptStore * m_scriptStore;
    TextField * m_textFieldCaller;
    TextArea * m_textAreaCaller;
    ScriptNodeCell m_leafCells[k_maxNumberOfDisplayedRows];
    SelectableTableView m_selectableTableView;
  };
  ContentViewController m_contentViewController;
};

}
#endif
