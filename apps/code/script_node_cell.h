#ifndef CODE_SCRIPT_NODE_CELL_H
#define CODE_SCRIPT_NODE_CELL_H

#include <escher/buffer_text_view.h>
#include <escher/metric.h>
#include <escher/table_cell.h>
#include <ion/display.h>

#include "script_node.h"

namespace Code {

class ScriptNodeCell : public Escher::TableCell {
 public:
  static_assert('\x11' == UCodePointEmpty, "Unicode error");
  constexpr static char k_parentheses[] = "()";
  constexpr static char k_parenthesesWithEmpty[] = "(\x11)";
  // Labels can be formed from user variables, a char limit is enforced.
  constexpr static int k_maxNumberOfCharsInLabel =
      (Ion::Display::Width - Escher::Metric::PopUpLeftMargin -
       2 * Escher::Metric::CellSeparatorThickness -
       Escher::Metric::CellLeftMargin - Escher::Metric::CellRightMargin -
       Escher::Metric::PopUpRightMargin) /
      KDFont::GlyphWidth(KDFont::Size::Large);
  static_assert(k_maxNumberOfCharsInLabel <
                    Escher::BufferTextView::k_maxNumberOfChar,
                "k_maxNumberOfCharsInLabel is too high");
  ScriptNodeCell();
  void setScriptNode(ScriptNode* node);

  /* TableCell */
  const Escher::View* labelView() const override { return &m_labelView; }
  const Escher::View* subLabelView() const override {
    return m_subLabelView.text()[0] != 0 ? &m_subLabelView : nullptr;
  }

  /* HighlightCell */
  void setHighlighted(bool highlight) override;
  void reloadCell() override;

 private:
  Escher::BufferTextView m_labelView;
  Escher::BufferTextView m_subLabelView;
};

}  // namespace Code

#endif
