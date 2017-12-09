#ifndef CODE_VARIABLE_BOX_CONTROLLER_H
#define CODE_VARIABLE_BOX_CONTROLLER_H

#include <escher.h>
#include "menu_controller.h"
#include "script_node.h"
#include "script_node_cell.h"
#include "script_store.h"

namespace Code {

class VariableBoxController final : public StackViewController {
public:
  VariableBoxController(MenuController * menuController, ScriptStore * scriptStore);
  void didBecomeFirstResponder() override;
  void setTextInputCaller(TextInput * textInput) {
    m_contentViewController.setTextInputCaller(textInput);
  }
  void viewWillAppear() override;
  void viewDidDisappear() override;
private:
  class ContentViewController : public ViewController, public SimpleListViewDataSource, public SelectableTableViewDataSource {
  public:
    ContentViewController(Responder * parentResponder, MenuController * menuController, ScriptStore * scriptStore);
    void setTextInputCaller(TextInput * textInput) {
      m_textInputCaller = textInput;
    }
    void reloadData() {
      m_selectableTableView.reloadData();
    }

    void addFunctionAtIndex(const char * functionName, int scriptIndex) {
      m_scriptNodes[m_scriptNodesCount] = ScriptNode::FunctionNode(functionName, scriptIndex);
      m_scriptNodesCount++;
    }
    void addVariableAtIndex(const char * variableName, int scriptIndex) {
      m_scriptNodes[m_scriptNodesCount] = ScriptNode::VariableNode(variableName, scriptIndex);
      m_scriptNodesCount++;
    }

    /* ViewController */
    const char * title() override { return I18n::translate(I18n::Message::FunctionsAndVariables); }
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
