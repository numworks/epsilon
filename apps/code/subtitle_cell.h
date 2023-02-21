#ifndef CODE_SUBTITLE_CELL_H
#define CODE_SUBTITLE_CELL_H

#include <escher/buffer_table_cell.h>
#include <escher/buffer_text_view.h>
#include <escher/metric.h>
#include <ion/display.h>

namespace Code {

class SubtitleCell : public Escher::BufferTableCell {
 public:
  SubtitleCell();
  const Escher::View* labelView() const override { return &m_labelView; }
  void setHighlighted(bool highlight) override { assert(!highlight); }
  // Overriding TableCell row Height
  void layoutSubviews(bool force = false) override;
  KDSize minimalSizeForOptimalDisplay() const override {
    return KDSize(bounds().width(), k_subtitleRowHeight);
  }
  // Actual height is 24 with the overlapping pixel from SelectableTableView.
  constexpr static KDCoordinate k_subtitleRowHeight = 23;

 protected:
  KDColor backgroundColor() const override { return k_backgroundColor; }

 private:
  /* Text from BufferTableCell can be formed from user input (script names).
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
};

}  // namespace Code

#endif
