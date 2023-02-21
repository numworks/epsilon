#ifndef CODE_VARIABLE_BOX_CONTROLLER_H
#define CODE_VARIABLE_BOX_CONTROLLER_H

#include <escher/toolbox_message_tree.h>

#include "alternate_empty_nested_menu_controller.h"
#include "script_node.h"
#include "script_node_cell.h"
#include "script_store.h"
#include "subtitle_cell.h"
#include "variable_box_empty_controller.h"

namespace Code {

class VariableBoxController : public AlternateEmptyNestedMenuController {
 public:
  VariableBoxController(ScriptStore* scriptStore);

  /* Responder */
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  void didEnterResponderChain(Responder* previousFirstResponder) override;

  /* MemoizedListViewDataSource */
  KDCoordinate nonMemoizedRowHeight(int j) override;
  int numberOfRows() const override;
  Escher::HighlightCell* reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  int typeAtIndex(int i) const override;
  void willDisplayCellForIndex(Escher::HighlightCell* cell, int index) override;
  /* SelectableTableViewDelegate */
  void tableViewDidChangeSelection(
      Escher::SelectableTableView* t, int previousSelectedCellX,
      int previousSelectedCellY,
      bool withinTemporarySelection = false) override;

  // AlternateEmptyNestedMenuController
  Escher::ViewController* emptyViewController() override {
    return &m_variableBoxEmptyController;
  }

  /* VariableBoxController */
  void setDisplaySubtitles(bool display) { m_displaySubtitles = display; }
  void loadFunctionsAndVariables(int scriptIndex,
                                 const char* textToAutocomplete,
                                 int textToAutocompleteLength);
  const char* autocompletionAlternativeAtIndex(int textToAutocompleteLength,
                                               int* textToInsertLength,
                                               bool* addParentheses, int index,
                                               int* indexToUpdate = nullptr);
  void loadVariablesImportedFromScripts();
  void empty();
  void insertAutocompletionResultAtIndex(int index);

 private:
  constexpr static size_t k_maxNumberOfDisplayedItems =
      Escher::Metric::MinimalNumberOfScrollableRowsToFillDisplayHeight(
          Escher::TableCell::k_minimalSmallFontCellHeight,
          Escher::Metric::PopUpTopMargin);
  constexpr static size_t k_maxNumberOfDisplayedSubtitles =
      Escher::Metric::MinimalNumberOfScrollableRowsToFillDisplayHeight(
          SubtitleCell::k_subtitleRowHeight +
              Escher::TableCell::k_minimalSmallFontCellHeight,
          Escher::Metric::PopUpTopMargin);
  constexpr static size_t k_totalBuiltinNodesCount = 107;
  // Chosen without particular reasons
  constexpr static size_t k_maxOtherScriptNodesCount = 32;
  // CurrentScriptOrigin + BuiltinsOrigin + ImportedOrigin
  constexpr static size_t k_maxScriptNodesCount = k_maxOtherScriptNodesCount +
                                                  k_totalBuiltinNodesCount +
                                                  k_maxOtherScriptNodesCount;
  // currentScriptOrigin + builtinsOrigin + 8 importedOrigins max
  constexpr static uint8_t k_maxOrigins = 10;
  // We don't care as it is not selectable
  constexpr static uint8_t k_subtitleCellType = k_nodeCellType;
  // So that upper class NestedMenuController knows it's a leaf
  constexpr static uint8_t k_itemCellType = k_leafCellType;
  constexpr static uint8_t k_currentScriptOrigin = 0;
  constexpr static uint8_t k_builtinsOrigin = 1;
  // And above for other imported nodes
  constexpr static uint8_t k_importedOrigin = 2;
  static_assert(
      k_currentScriptOrigin == 0 && k_builtinsOrigin == 1 &&
          k_importedOrigin == 2,
      "These origin index must start at 0 and leave no gaps. k_importedOrigin "
      "must be the last one. Otherwise, for loops on origin must be updated.");

  /* Returns:
   * - a negative int if the node name is before name in alphabetical
   * order
   * - 0 if they are equal
   * - a positive int if it is after in alphabetical order.
   * strictlyStartsWith is set to True if the node name starts with name but
   * they are not equal.*/
  static int NodeNameCompare(ScriptNode* node, const char* name, int nameLength,
                             bool* strictlyStartsWith = nullptr);

  // Nodes and nodes count
  bool maxNodesReachedForOrigin(uint8_t origin) const;
  size_t nodesCountForOrigin(uint8_t origin) const;
  ScriptNode* scriptNodeAtIndex(int index);

  // Cell getters
  int typeAndOriginAtLocation(int i, uint8_t* resultOrigin = nullptr,
                              int* cumulatedOriginsCount = nullptr) const;

  // NestedMenuController
  Escher::HighlightCell* leafCellAtIndex(int index) override {
    assert(false);
    return nullptr;
  }
  Escher::HighlightCell* nodeCellAtIndex(int index) override {
    assert(false);
    return nullptr;
  }
  I18n::Message subTitle() override {
    assert(false);
    return (I18n::Message)0;
  }
  bool selectLeaf(int rowIndex) override;
  void insertTextInCaller(const char* text, int textLength = -1);

  // Loading
  void loadBuiltinNodes(const char* textToAutocomplete,
                        int textToAutocompleteLength);
  void loadImportedVariablesInScript(const char* scriptContent,
                                     const char* textToAutocomplete,
                                     int textToAutocompleteLength);
  void loadCurrentVariablesInScript(const char* scriptContent,
                                    const char* textToAutocomplete,
                                    int textToAutocompleteLength);
  void loadGlobalAndImportedVariablesInScriptAsImported(
      Script script, const char* textToAutocomplete,
      int textToAutocompleteLength, bool importFromModules = true);
  // Returns true if this was an import structure
  bool addNodesFromImportMaybe(mp_parse_node_struct_t* parseNode,
                               const char* textToAutocomplete,
                               int textToAutocompleteLength,
                               bool importFromModules = true);
  const char* importationSourceNameFromNode(mp_parse_node_t& node);
  bool importationSourceIsModule(
      const char* sourceName,
      const Escher::ToolboxMessageTree** moduleChildren = nullptr,
      int* numberOfModuleChildren = nullptr);
  bool importationSourceIsScript(const char* sourceName,
                                 const char** scriptFullName,
                                 Script* retreivedScript = nullptr);
  bool addImportStructFromScript(mp_parse_node_struct_t* pns, uint structKind,
                                 const char* scriptName,
                                 const char* textToAutocomplete,
                                 int textToAutocompleteLength);
  /* Add a node if it completes the text to autocomplete and if it is not
   * already contained in the variable box. The returned boolean means we
   * should escape the node scanning process (due to the lexicographical order
   * or full node table). */
  bool addNodeIfMatches(const char* textToAutocomplete,
                        int textToAutocompleteLength, ScriptNode::Type type,
                        uint8_t origin, const char* nodeName,
                        int nodeNameLength = -1,
                        const char* nodeSourceName = nullptr,
                        const char* description = nullptr);
  VariableBoxEmptyController m_variableBoxEmptyController;
  ScriptNode m_scriptNodes[k_maxScriptNodesCount];
  ScriptNodeCell m_itemCells[k_maxNumberOfDisplayedItems];
  SubtitleCell m_subtitleCells[k_maxNumberOfDisplayedSubtitles];
  ScriptStore* m_scriptStore;
  size_t m_nodesCount;                      // Number of nodes
  uint8_t m_originsCount;                   // Number of origins
  size_t m_rowsPerOrigins[k_maxOrigins];    // Nodes per origins
  const char* m_originsName[k_maxOrigins];  // Text of origins
  // This is used to send only the completing text when we are autocompleting
  int m_shortenResultCharCount;
  bool m_displaySubtitles;
};

}  // namespace Code
#endif
