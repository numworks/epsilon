#ifndef CODE_VARIABLE_BOX_CONTROLLER_H
#define CODE_VARIABLE_BOX_CONTROLLER_H

#include <escher.h>
#include "script_node.h"
#include "script_node_cell.h"
#include "script_store.h"

namespace Code {

class VariableBoxController : public NestedMenuController {
public:
  VariableBoxController(ScriptStore * scriptStore);

  /* Responder */
  bool handleEvent(Ion::Events::Event event) override;
  void didEnterResponderChain(Responder * previousFirstResponder) override;

  /* ListViewDataSource */
  int numberOfRows() const override;
  int reusableCellCount(int type) override {
    assert(type == 0);
    return k_maxNumberOfDisplayedRows;
  }
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  int typeAtLocation(int i, int j) override { return 0; }

  /* VariableBoxController */
  void loadFunctionsAndVariables(int scriptIndex, const char * textToAutocomplete);
  const char * autocompletionForText(int scriptIndex, const char * text);
private:
  constexpr static int k_maxScriptObjectNameSize = 100;
  constexpr static int k_maxNumberOfDisplayedRows = 6; // 240/40
  constexpr static int k_maxScriptNodesCount = 32;
  constexpr static int k_builtinNodesCount = 61;
  HighlightCell * leafCellAtIndex(int index) override;
  HighlightCell * nodeCellAtIndex(int index) override { return nullptr; }
  bool selectLeaf(int rowIndex) override;
  void insertTextInCaller(const char * text, int textLength = -1);
  void addFunctionAtIndex(const char * functionName, int nameLength, int scriptIndex);
  void addVariableAtIndex(const char * variableName, int nameLength, int scriptIndex);
  ScriptNode * scriptNodeAtIndex(int index);
  ScriptNode m_currentScriptNodes[k_maxScriptNodesCount];
  ScriptNode m_builtinNodes[k_builtinNodesCount];
  ScriptNode m_importedNodes[k_maxScriptNodesCount];
  ScriptNodeCell m_leafCells[k_maxNumberOfDisplayedRows];
  ScriptStore * m_scriptStore;
  int m_currentScriptNodesCount;
  int m_importedNodesCount;
};

}
#endif
