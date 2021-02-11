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
    m_labelView(KDFont::LargeFont),
    m_subLabelView(KDFont::SmallFont, 0.5f, 0.5f, Escher::Palette::GrayDark),
    m_accessoryView(KDFont::SmallFont, 0.5f, 0.5f)
  {}
  void setScriptNode(ScriptNode * node);

  /* TableCell */
  Escher::View * labelView() const override { return const_cast<Escher::View *>(static_cast<const Escher::View *>(&m_labelView)); }
  Escher::View * subLabelView() const override { return const_cast<Escher::View *>(static_cast<const Escher::View *>(&m_subLabelView)); }
  // TODO : Remove source from cell, and handle it as a subtitle Cell in VarBox.
  Escher::View * accessoryView() const override { return const_cast<Escher::View *>(static_cast<const Escher::View *>(&m_accessoryView)); }

  /* HighlightCell */
  void setHighlighted(bool highlight) override;
  void reloadCell() override;
private:
  Escher::BufferTextView m_labelView;
  Escher::BufferTextView m_subLabelView;
  Escher::BufferTextView m_accessoryView;
};

}

#endif
