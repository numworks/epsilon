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
  void loadFunctionsAndVariables(int scriptIndex, const char * textToAutocomplete, int textToAutocompleteLength);
  const char * autocompletionForText(int scriptIndex, const char * text, int * textToInsertLength);
private:
  constexpr static int k_maxScriptObjectNameSize = 100;
  constexpr static int k_maxNumberOfDisplayedRows = 6; // 240/40
  constexpr static int k_maxScriptNodesCount = 32;
  constexpr static int k_totalBuiltinNodesCount = 101;
  enum class NodeOrigin : uint8_t {
    CurrentScript,
    Builtins,
    Importation
  };
  static int MaxNodesCountForOrigin(NodeOrigin origin);
  /* Returns:
   * - a negative int if the node name is before name in alphabetical
   * order
   * - 0 if they are equal
   * - a positive int if it is after in alphabetical order.
   * strictlyStartsWith is set to True if the node name starts with name but
   * they are not equal.*/
  static int NodeNameCompare(ScriptNode * node, const char * name, int nameLength, bool * strictlyStartsWith = nullptr);
  /* Returns:
   * - a negative int if the node name is before name in alphabetical
   * order or equal to name
   * - 0 if node name strictly starts with name
   * - a positive int if node name is after name in alphabetical order. */
  static int NodeNameStartsWith(ScriptNode * node, const char * name, int nameLength);
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

  void loadBuiltinNodes(const char * textToAutocomplete, int textToAutocompleteLength);
  void loadCurrentAndImportedVariableInScript(Script script, const char * textToAutocomplete, int textToAutocompleteLength);
  void addNodesFromImportMaybe(mp_parse_node_struct_t * parseNode, const char * textToAutocomplete, int textToAutocompleteLength);
  /* Add a node if it completes the text to autocomplete and if it is not
   * already contained in the variable box. */
  bool shouldAddNode(const char * textToAutocomplete, int textToAutocompleteLength, const char * name, int nameLength);
  void checkAndAddNode(const char * textToAutocomplete, int textToAutocompleteLength, ScriptNode::Type type, NodeOrigin origin, const char * name, int nameLength, int scriptIndex = 0);
  void addNode(ScriptNode::Type type, NodeOrigin origin, const char * name, int nameLength, int scriptIndex = 0);
  bool contains(const char * name, int nameLength);


  ScriptNode m_currentScriptNodes[k_maxScriptNodesCount];
  ScriptNode m_builtinNodes[k_totalBuiltinNodesCount];
  ScriptNode m_importedNodes[k_maxScriptNodesCount];
  ScriptNodeCell m_leafCells[k_maxNumberOfDisplayedRows];
  ScriptStore * m_scriptStore;
  // TODO LEA Put these in an array?
  int m_currentScriptNodesCount;
  int m_builtinNodesCount;
  int m_importedNodesCount;
  int m_shortenResultBytesCount; // This is used to send only the completing text when we are autocompleting
};

}
#endif
