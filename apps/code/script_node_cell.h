#ifndef CODE_SCRIPT_NODE_CELL_H
#define CODE_SCRIPT_NODE_CELL_H

#include "script_node.h"
#include "script_store.h"
#include <escher/table_cell.h>
#include <ion/charset.h>
#include <kandinsky/coordinate.h>

namespace Code {

class ScriptNodeCell final : public TableCell {
public:
  ScriptNodeCell() : TableCell(), m_scriptNodeView() {}
  void setScriptNode(ScriptNode * node);
  void setScriptStore(ScriptStore * scriptStore) {
    m_scriptNodeView.setScriptStore(scriptStore);
  }

  /* TableCell */
  View * labelView() const override { return const_cast<View *>(static_cast<const View *>(&m_scriptNodeView)); }

  /* HighlightCell */
  void setHighlighted(bool highlight) override;
  void reloadCell() override;

  constexpr static char k_parentheses[] = "()";
  constexpr static char k_parenthesesWithEmpty[] = {'(', Ion::Charset::Empty, ')', 0};
protected:
  class ScriptNodeView : public HighlightCell {
  public:
    ScriptNodeView() : HighlightCell(), m_scriptNode(nullptr), m_scriptStore(nullptr) {}
    void setScriptNode(ScriptNode * scriptNode) { m_scriptNode = scriptNode; }
    void setScriptStore(ScriptStore * scriptStore) { m_scriptStore = scriptStore; }
    void drawRect(KDContext * ctx, KDRect rect) const override;
    virtual KDSize minimalSizeForOptimalDisplay() const override;
  private:
    constexpr static KDText::FontSize k_fontSize = KDText::FontSize::Small;
    constexpr static KDCoordinate k_verticalMargin = 7;
    ScriptNode * m_scriptNode;
    ScriptStore * m_scriptStore;
  };
  ScriptNodeView m_scriptNodeView;
};

}

#endif
