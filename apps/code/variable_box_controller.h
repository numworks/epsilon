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
  void setTextInputCaller(TextInput * textInput);
  void viewWillAppear() override;
  void viewDidDisappear() override;
private:
  class ContentViewController : public ViewController, public SimpleListViewDataSource, public SelectableTableViewDataSource {
  public:
    ContentViewController(Responder * parentResponder, MenuController * menuController, ScriptStore * scriptStore);
    void setTextInputCaller(TextInput * textInput);
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

    /* SimpleListViewDataSource */
    KDCoordinate cellHeight() override { return Metric::ToolboxRowHeight; }
    int numberOfRows() override;
    HighlightCell * reusableCell(int index) override;
    int reusableCellCount() override;
    void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  private:
    constexpr static int k_maxNumberOfDisplayedRows = 6; //240/40
    constexpr static int k_maxScriptNodesCount = 32;
    void insertTextInCaller(const char * text);
    int m_scriptNodesCount;
    ScriptNode m_scriptNodes[k_maxScriptNodesCount];
    MenuController * m_menuController;
    ScriptStore * m_scriptStore;
    TextInput * m_textInputCaller;
    ScriptNodeCell m_leafCells[k_maxNumberOfDisplayedRows];
    SelectableTableView m_selectableTableView;
  };
  ContentViewController m_contentViewController;
};

}
#endif
