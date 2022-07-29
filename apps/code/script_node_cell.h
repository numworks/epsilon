#ifndef CODE_SCRIPT_NODE_CELL_H
#define CODE_SCRIPT_NODE_CELL_H

#include "script_node.h"
#include <escher/metric.h>
#include <escher/table_cell.h>
#include <escher/buffer_text_view.h>
#include <ion/display.h>

namespace Code {

class ScriptNodeCell : public Escher::TableCell {
public:
  static_assert('\x11' == UCodePointEmpty, "Unicode error");
  constexpr static char k_parentheses[] = "()";
  constexpr static char k_parenthesesWithEmpty[] = "(\x11)";
  // Labels can be formed from user variables, a char limit is enforced.
  constexpr static int k_maxNumberOfCharsInLabel = (Ion::Display::Width - Escher::Metric::PopUpLeftMargin - 2 * Escher::Metric::CellSeparatorThickness - Escher::Metric::CellLeftMargin - Escher::Metric::CellRightMargin - Escher::Metric::PopUpRightMargin) / 10; // With 10 = KDFont::GlyphWidth(KDFont::Size::Large)
  static_assert(k_maxNumberOfCharsInLabel < Escher::BufferTextView::k_maxNumberOfChar, "k_maxNumberOfCharsInLabel is too high");
  ScriptNodeCell() :
    TableCell(),
    m_labelView(KDFont::Size::Large, KDContext::k_alignLeft, KDContext::k_alignCenter, KDColorBlack, KDColorWhite, k_maxNumberOfCharsInLabel),
    m_subLabelView(KDFont::Size::Small, KDContext::k_alignLeft, KDContext::k_alignCenter, Escher::Palette::GrayDark)
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
