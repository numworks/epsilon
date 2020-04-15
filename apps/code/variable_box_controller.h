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

  /* TableViewDataSource */
  KDCoordinate rowHeight(int j) override;
  int numberOfRows() const override;
  HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtLocation(int i, int j) override;
  /* ListViewDataSource */
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;

  /* VariableBoxController */
  void loadFunctionsAndVariables(int scriptIndex, const char * textToAutocomplete, int textToAutocompleteLength);
  const char * autocompletionForText(int scriptIndex, const char * textToAutocomplete, int * textToInsertLength);

private:
  //TODO LEA use size_t
  constexpr static int k_maxScriptObjectNameSize = 100; //TODO LEA
  constexpr static int k_maxNumberOfDisplayedRows = 8; // (240 - titlebar - margin)/27 //TODO LEA
  constexpr static int k_maxScriptNodesCount = 32; //TODO LEA
  constexpr static int k_totalBuiltinNodesCount = 107;
  constexpr static uint8_t k_scriptOriginsCount = 3;
  constexpr static uint8_t k_subtitleCellType = NodeCellType; // We don't care as it is not selectable
  constexpr static uint8_t k_itemCellType = LeafCellType; // So that upper class NestedMenuController knows it's a leaf
  constexpr static KDCoordinate k_subtitleRowHeight = 23;
  constexpr static KDCoordinate k_simpleItemRowHeight = 27;
  constexpr static KDCoordinate k_complexItemRowHeight = 44;
  enum class NodeOrigin : uint8_t {
    CurrentScript = 0,
    Builtins = 1,
    Importation = 2
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
  int typeAndOriginAtLocation(int i, NodeOrigin * resultOrigin = nullptr, int * cumulatedOriginsCount = nullptr) const;

  // NestedMenuController
  HighlightCell * leafCellAtIndex(int index) override { assert(false); return nullptr; }
  HighlightCell * nodeCellAtIndex(int index) override { assert(false); return nullptr; }
  bool selectLeaf(int rowIndex) override;
  void insertTextInCaller(const char * text, int textLength = -1);

  // Loading
  void loadBuiltinNodes(const char * textToAutocomplete, int textToAutocompleteLength);
  void loadImportedVariablesInScript(const char * scriptContent, const char * textToAutocomplete, int textToAutocompleteLength);
  void loadCurrentVariablesInScript(const char * scriptContent, const char * textToAutocomplete, int textToAutocompleteLength);
  void loadGlobalAndImportedVariablesInScriptAsImported(const char * scriptName, const char * scriptContent, const char * textToAutocomplete, int textToAutocompleteLength);
  // Returns true if this was an import structure
  bool addNodesFromImportMaybe(mp_parse_node_struct_t * parseNode, const char * textToAutocomplete, int textToAutocompleteLength);
  void addImportStruct(mp_parse_node_struct_t * pns, uint structKind, const char * scriptName, const char * textToAutocomplete, int textToAutocompleteLength);
  /* Add a node if it completes the text to autocomplete and if it is not
   * already contained in the variable box. */
  void checkAndAddNode(const char * textToAutocomplete, int textToAutocompleteLength, ScriptNode::Type type, NodeOrigin origin, const char * name, int nameLength, const char * nodeSourceName = nullptr, const char * description = nullptr);
  bool shouldAddNode(const char * textToAutocomplete, int textToAutocompleteLength, const char * name, int nameLength);
  bool contains(const char * name, int nameLength);
  void addNode(ScriptNode::Type type, NodeOrigin origin, const char * name, int nameLength, const char * nodeSourceName = nullptr, const char * description = nullptr);
  ScriptNode m_currentScriptNodes[k_maxScriptNodesCount];
  ScriptNode m_builtinNodes[k_totalBuiltinNodesCount];
  ScriptNode m_importedNodes[k_maxScriptNodesCount];
  ScriptNodeCell m_itemCells[k_maxNumberOfDisplayedRows];
  MessageTableCell m_subtitleCells[k_scriptOriginsCount];
  ScriptStore * m_scriptStore;
  // TODO LEA Put these in an array?
  int m_currentScriptNodesCount;
  int m_builtinNodesCount;
  int m_importedNodesCount;
  int m_shortenResultCharCount; // This is used to send only the completing text when we are autocompleting
};

}
#endif
