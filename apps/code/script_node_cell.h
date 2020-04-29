#ifndef CODE_SCRIPT_NODE_CELL_H
#define CODE_SCRIPT_NODE_CELL_H

#include "script_node.h"
#include "script_store.h"
#include <escher/table_cell.h>
#include <kandinsky/coordinate.h>

namespace Code {

class ScriptNodeCell : public TableCell {
public:
  static_assert('\x11' == UCodePointEmpty, "Unicode error");
  constexpr static char k_parentheses[] = "()";
  constexpr static char k_parenthesesWithEmpty[] = "(\x11)";
  constexpr static KDCoordinate k_simpleItemHeight = 27;
  constexpr static KDCoordinate k_complexItemHeight = 42;

  ScriptNodeCell() :
    TableCell(),
    m_scriptNodeView()
  {}
  void setScriptNode(ScriptNode * node);
  static bool CanDisplayNameAndSource(int nameLength, const char * source);

  /* TableCell */
  View * labelView() const override { return const_cast<View *>(static_cast<const View *>(&m_scriptNodeView)); }

  /* HighlightCell */
  void setHighlighted(bool highlight) override;
  void reloadCell() override;
  const char * text() const override { return m_scriptNodeView.text(); }

protected:
  class ScriptNodeView : public HighlightCell {
  public:
    constexpr static const KDFont * k_font = KDFont::SmallFont;
    constexpr static KDCoordinate k_optimalWidth = Ion::Display::Width - Metric::PopUpLeftMargin - Metric::PopUpRightMargin;
    ScriptNodeView() :
      HighlightCell(),
      m_scriptNode(nullptr)
    {}
    void setScriptNode(ScriptNode * node) { m_scriptNode = node; }
    void drawRect(KDContext * ctx, KDRect rect) const override;
    virtual KDSize minimalSizeForOptimalDisplay() const override;
    const char * text() const override {
      return m_scriptNode->description();
    }
  private:
    constexpr static KDCoordinate k_bottomMargin = 5;
    constexpr static KDCoordinate k_topMargin = k_bottomMargin + k_separatorThickness;
    ScriptNode * m_scriptNode;
  };
  ScriptNodeView m_scriptNodeView;
};

}

#endif
