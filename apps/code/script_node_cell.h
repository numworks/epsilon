#ifndef CODE_SCRIPT_NODE_CELL_H
#define CODE_SCRIPT_NODE_CELL_H

#include "script_node.h"
#include <escher/table_cell.h>
#include <escher/buffer_text_view.h>

namespace Code {

class ScriptNodeCell : public Escher::TableCell {
public:
  static_assert('\x11' == UCodePointEmpty, "Unicode error");
  constexpr static char k_parentheses[] = "()";
  constexpr static char k_parenthesesWithEmpty[] = "(\x11)";
  ScriptNodeCell() :
    TableCell(),
    m_labelView(KDFont::LargeFont, 0.0f),
    m_subLabelView(KDFont::SmallFont, 0.0f, 0.5f, Escher::Palette::GrayDark)
  {}
  void setScriptNode(ScriptNode * node);

  /* TableCell */
  const Escher::View * labelView() const override { return &m_labelView; }
  const Escher::View * subLabelView() const override;

  /* HighlightCell */
  void setHighlighted(bool highlight) override;
  void reloadCell() override;
private:
  Escher::BufferTextView m_labelView;
  Escher::BufferTextView m_subLabelView;
};

}

#endif
