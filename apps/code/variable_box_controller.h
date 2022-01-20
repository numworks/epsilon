#ifndef CODE_VARIABLE_BOX_CONTROLLER_H
#define CODE_VARIABLE_BOX_CONTROLLER_H

#include <apps/alternate_empty_nested_menu_controller.h>
#include <escher/message_table_cell.h>
#include <escher/toolbox_message_tree.h>
#include "script_node.h"
#include "script_node_cell.h"
#include "script_store.h"
#include "variable_box_empty_controller.h"

namespace Code {

class VariableBoxController : public AlternateEmptyNestedMenuController {
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
  /* SelectableTableViewDelegate */
  void tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection = false) override;

  //AlternateEmptyNestedMenuController
  ViewController * emptyViewController() override { return &m_variableBoxEmptyController; }

  /* VariableBoxController */
  void setDisplaySubtitles(bool display) { m_displaySubtitles = display; }
  void loadFunctionsAndVariables(int scriptIndex, const char * textToAutocomplete, int textToAutocompleteLength);
  const char * autocompletionAlternativeAtIndex(int textToAutocompleteLength, int * textToInsertLength, bool * addParentheses, int index, int * indexToUpdate = nullptr);
  void loadVariablesImportedFromScripts();
  void empty();
  void insertAutocompletionResultAtIndex(int index);

private:
  constexpr static size_t k_maxNumberOfDisplayedItems = (Ion::Display::Height - Metric::TitleBarHeight - Metric::PopUpTopMargin) / ScriptNodeCell::k_simpleItemHeight + 2; // +2 if the cells are cropped on top and at the bottom
  constexpr static size_t k_maxScriptNodesCount = 64; // Chosen without particular reasons (Number of functions in the variables box)
  constexpr static int k_totalBuiltinNodesCount = 107;
  constexpr static uint8_t k_scriptOriginsCount = 8; // Number of scripts loaded in the variable box
  constexpr static uint8_t k_subtitleCellType = NodeCellType; // We don't care as it is not selectable
  constexpr static uint8_t k_itemCellType = LeafCellType; // So that upper class NestedMenuController knows it's a leaf
  constexpr static KDCoordinate k_subtitleRowHeight = 23;

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

  // Nodes and nodes count
  static size_t MaxNodesCountForOrigin(NodeOrigin origin) {
    return origin == NodeOrigin::Builtins ? k_totalBuiltinNodesCount : k_maxScriptNodesCount;
  }
  int nodesCountForOrigin(NodeOrigin origin) const;
  size_t * nodesCountPointerForOrigin(NodeOrigin origin);
  ScriptNode * nodesForOrigin(NodeOrigin origin);
  ScriptNode * scriptNodeAtIndex(int index);

  // Cell getters
  int typeAndOriginAtLocation(int i, NodeOrigin * resultOrigin = nullptr, int * cumulatedOriginsCount = nullptr) const;

  // NestedMenuController
  HighlightCell * leafCellAtIndex(int index) override { assert(false); return nullptr; }
  HighlightCell * nodeCellAtIndex(int index) override { assert(false); return nullptr; }
  bool selectLeaf(int rowIndex, bool quitToolbox) override;
  void insertTextInCaller(const char * text, int textLength = -1);

  // Loading
  void loadBuiltinNodes(const char * textToAutocomplete, int textToAutocompleteLength);
  void loadImportedVariablesInScript(const char * scriptContent, const char * textToAutocomplete, int textToAutocompleteLength);
  void loadCurrentVariablesInScript(const char * scriptContent, const char * textToAutocomplete, int textToAutocompleteLength);
  void loadGlobalAndImportedVariablesInScriptAsImported(Script script, const char * textToAutocomplete, int textToAutocompleteLength, bool importFromModules = true);
  // Returns true if this was an import structure
  bool addNodesFromImportMaybe(mp_parse_node_struct_t * parseNode, const char * textToAutocomplete, int textToAutocompleteLength, bool importFromModules = true);
  const char * importationSourceNameFromNode(mp_parse_node_t & node);
  bool importationSourceIsModule(const char * sourceName, const ToolboxMessageTree * * moduleChildren = nullptr, int * numberOfModuleChildren = nullptr);
  bool importationSourceIsScript(const char * sourceName, const char * * scriptFullName, Script * retrievedScript = nullptr);
  bool addImportStructFromScript(mp_parse_node_struct_t * pns, uint structKind, const char * scriptName, const char * textToAutocomplete, int textToAutocompleteLength);
  /* Add a node if it completes the text to autocomplete and if it is not
   * already contained in the variable box. The returned boolean means we
   * should escape the node scanning process (due to the lexicographical order
   * or full node table). */
  bool addNodeIfMatches(const char * textToAutocomplete, int textToAutocompleteLength, ScriptNode::Type type, NodeOrigin origin, const char * nodeName, int nodeNameLength = -1, const char * nodeSourceName = nullptr, const char * description = nullptr);
  VariableBoxEmptyController m_variableBoxEmptyController;
  ScriptNode m_currentScriptNodes[k_maxScriptNodesCount];
  ScriptNode m_builtinNodes[k_totalBuiltinNodesCount];
  ScriptNode m_importedNodes[k_maxScriptNodesCount];
  ScriptNodeCell m_itemCells[k_maxNumberOfDisplayedItems];
  MessageTableCell m_subtitleCells[k_scriptOriginsCount];
  ScriptStore * m_scriptStore;
  size_t m_currentScriptNodesCount;
  size_t m_builtinNodesCount;
  size_t m_importedNodesCount;
  int m_shortenResultCharCount; // This is used to send only the completing text when we are autocompleting
  bool m_displaySubtitles;
};

}
#endif
