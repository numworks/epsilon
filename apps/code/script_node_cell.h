#ifndef CODE_SCRIPT_NODE_CELL_H
#define CODE_SCRIPT_NODE_CELL_H

#include "script_node.h"
#include "script_store.h"
#include <escher/table_cell.h>
#include <kandinsky/coordinate.h>

namespace Code {

class ScriptNodeCell : public TableCell {
public:
  ScriptNodeCell();
  void setScriptNode(ScriptNode * node);
  void setScriptStore(ScriptStore * scriptStore);

  /* TableCell */
  View * labelView() const override { return const_cast<View *>(static_cast<const View *>(&m_scriptNodeView)); }

  /* HighlightCell */
  void setHighlighted(bool highlight) override;
  void reloadCell() override;

  constexpr static char k_parentheses[] = "()";
protected:
  class ScriptNodeView : public HighlightCell {
  public:
    ScriptNodeView();
    void setScriptNode(ScriptNode * scriptNode);
    void setScriptStore(ScriptStore * scriptStore);
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
