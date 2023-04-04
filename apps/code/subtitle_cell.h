#ifndef CODE_SUBTITLE_CELL_H
#define CODE_SUBTITLE_CELL_H

#include <escher/bordered.h>
#include <escher/buffer_text_view.h>
#include <escher/highlight_cell.h>
#include <escher/metric.h>
#include <ion/display.h>

namespace Code {

class SubtitleCell : public Escher::Bordered, public Escher::HighlightCell {
 public:
  SubtitleCell();
  void setHighlighted(bool highlight) override { assert(!highlight); }
  KDSize minimalSizeForOptimalDisplay() const override {
    return KDSize(bounds().width(), k_subtitleRowHeight);
  }
  void drawRect(KDContext* ctx, KDRect rect) const override;
  void layoutSubviews(bool force = false) override;
  // Actual height is 24 with the overlapping pixel from SelectableTableView.
  constexpr static KDCoordinate k_subtitleRowHeight = 23;

  Escher::BufferTextView* textView() { return &m_textView; }

 private:
  /* Text can be formed from user input (script names).
   * A char limit on display is enforced */
  constexpr static int k_maxNumberOfCharsInBuffer =
      (Ion::Display::Width - Escher::Metric::PopUpLeftMargin -
       2 * Escher::Metric::CellSeparatorThickness -
       Escher::Metric::CellLeftMargin - Escher::Metric::CellRightMargin -
       Escher::Metric::PopUpRightMargin) /
      KDFont::GlyphWidth(KDFont::Size::Small);
  static_assert(k_maxNumberOfCharsInBuffer <
                    Escher::BufferTextView::k_maxNumberOfChar,
                "k_maxNumberOfCharsInBuffer is too high");
  constexpr static KDColor k_backgroundColor = Escher::Palette::WallScreen;
  constexpr static KDColor k_textColor = Escher::Palette::BlueishGray;

  int numberOfSubviews() const override { return 1; }
  Escher::View* subviewAtIndex(int index) override { return &m_textView; }
  bool protectedIsSelectable() override { return false; }

  Escher::BufferTextView m_textView;
};

}  // namespace Code

#endif
