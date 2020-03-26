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
  const char * autocompletionForText(int scriptIndex, const char * text, int * textToInsertLength);
private:
  constexpr static int k_maxScriptObjectNameSize = 100;
  constexpr static int k_maxNumberOfDisplayedRows = 6; // 240/40
  constexpr static int k_maxScriptNodesCount = 32;
  constexpr static int k_builtinNodesCount = 64;
  enum class NodeOrigin : uint8_t {
    CurrentScript,
    Builtins,
    Importation
  };
  enum class NodeType : uint8_t {
    Variable,
    Function
  };
  static int MaxNodesCountForOrigin(NodeOrigin origin);
  /* Return a negative int if the node name is before name in alphabetical
   * order, 0 if they are equal, a positive int if it is after in alphabetical
   * order. */
  static int NodeNameCompare(ScriptNode * node, const char * name, int nameLength);
  int * nodesCountPointerForOrigin(NodeOrigin origin);
  int nodesCountForOrigin(NodeOrigin origin) const;
  ScriptNode * nodesForOrigin(NodeOrigin origin);
  ScriptNode * scriptNodeAtIndex(int index);
  HighlightCell * leafCellAtIndex(int index) override {
    assert(index >= 0 && index < k_maxNumberOfDisplayedRows);
    return &m_leafCells[index];
  }
  HighlightCell * nodeCellAtIndex(int index) override { return nullptr; }

  bool selectLeaf(int rowIndex) override;
  void insertTextInCaller(const char * text, int textLength = -1);
  void addNode(NodeType type, NodeOrigin origin, const char * name, int nameLength, int scriptIndex = 0);

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
