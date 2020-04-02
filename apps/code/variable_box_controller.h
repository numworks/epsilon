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
  int numberOfRows() const override {
    return m_currentScriptNodesCount + m_builtinNodesCount + m_importedNodesCount;
  }
  int reusableCellCount(int type) override {
    assert(type == 0);
    return k_maxNumberOfDisplayedRows;
  }
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  int typeAtLocation(int i, int j) override { return 0; }

  /* VariableBoxController */
  void loadFunctionsAndVariables(int scriptIndex, const char * textToAutocomplete, int textToAutocompleteLength);
  const char * autocompletionForText(int scriptIndex, const char * textToAutocomplete, int * textToInsertLength);

private:
  //TODO LEA use size_t
  constexpr static int k_maxScriptObjectNameSize = 100; //TODO LEA
  constexpr static int k_maxNumberOfDisplayedRows = 6; // 240/40
  constexpr static int k_maxScriptNodesCount = 32; //TODO LEA
  constexpr static int k_totalBuiltinNodesCount = 98;
  enum class NodeOrigin : uint8_t {
    CurrentScript,
    Builtins,
    Importation
  };

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

  // Nodes and nodes count
  static int MaxNodesCountForOrigin(NodeOrigin origin) {
    assert(origin == NodeOrigin::CurrentScript || origin == NodeOrigin::Importation);
    return k_maxScriptNodesCount;
  }
  int nodesCountForOrigin(NodeOrigin origin) const;
  int * nodesCountPointerForOrigin(NodeOrigin origin);
  ScriptNode * nodesForOrigin(NodeOrigin origin);
  ScriptNode * scriptNodeAtIndex(int index);

  // Cell getters
  HighlightCell * leafCellAtIndex(int index) override {
    assert(index >= 0 && index < k_maxNumberOfDisplayedRows);
    return &m_leafCells[index];
  }
  HighlightCell * nodeCellAtIndex(int index) override { return nullptr; }

  // NestedMenuController
  bool selectLeaf(int rowIndex) override;
  void insertTextInCaller(const char * text, int textLength = -1);

  // Loading
  void loadBuiltinNodes(const char * textToAutocomplete, int textToAutocompleteLength);
  void loadImportedVariablesInScript(const char * scriptContent, const char * textToAutocomplete, int textToAutocompleteLength);
  void loadCurrentVariablesInScript(const char * scriptContent, const char * textToAutocomplete, int textToAutocompleteLength);
  void loadGlobalAndImportedVariablesInScriptAsImported(const char * scriptContent, const char * textToAutocomplete, int textToAutocompleteLength);
  // Returns true if this was an import structure
  bool addNodesFromImportMaybe(mp_parse_node_struct_t * parseNode, const char * textToAutocomplete, int textToAutocompleteLength);
  void addImportStruct(mp_parse_node_struct_t * pns, uint structKind, const char * textToAutocomplete, int textToAutocompleteLength);
  /* Add a node if it completes the text to autocomplete and if it is not
   * already contained in the variable box. */
  void checkAndAddNode(const char * textToAutocomplete, int textToAutocompleteLength, ScriptNode::Type type, NodeOrigin origin, const char * name, int nameLength, int scriptIndex = 0);
  bool shouldAddNode(const char * textToAutocomplete, int textToAutocompleteLength, const char * name, int nameLength);
  bool contains(const char * name, int nameLength);
  void addNode(ScriptNode::Type type, NodeOrigin origin, const char * name, int nameLength, int scriptIndex = 0);

  ScriptNode m_currentScriptNodes[k_maxScriptNodesCount];
  ScriptNode m_builtinNodes[k_totalBuiltinNodesCount];
  ScriptNode m_importedNodes[k_maxScriptNodesCount];
  ScriptNodeCell m_leafCells[k_maxNumberOfDisplayedRows];
  ScriptStore * m_scriptStore;
  // TODO LEA Put these in an array?
  int m_currentScriptNodesCount;
  int m_builtinNodesCount;
  int m_importedNodesCount;
  int m_shortenResultCharCount; // This is used to send only the completing text when we are autocompleting
};

}
#endif
