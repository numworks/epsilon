#pragma once

#include <escher/bordered.h>
#include <escher/buffer_text_view.h>
#include <escher/highlight_cell.h>
#include <escher/metric.h>
#include <escher/theme/components.h>
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

  Escher::AbstractBufferTextView* textView() { return &m_textView; }

 private:
  /* Text can be formed from user input (script names).
   * A char limit on display is enforced */
  constexpr static int k_maxNumberOfCharsInBuffer =
      (Ion::Display::Width - Escher::Metric::PopUpMargins.width() -
       2 * Escher::Metric::CellSeparatorThickness -
       Escher::Metric::CellMargins.width()) /
      KDFont::GlyphWidth(KDFont::Size::Small);
  static_assert(k_maxNumberOfCharsInBuffer <
                    Escher::OneLineBufferTextView<>::MaxTextSize(),
                "k_maxNumberOfCharsInBuffer is too high");
  constexpr static Escher::ThemeComponent k_backgroundColor =
      Escher::ThemeComponent::WallScreen;
  constexpr static Escher::ThemeComponent k_textColor =
      Escher::ThemeComponent::PythonGutterText;

  int numberOfSubviews() const override { return 1; }
  Escher::View* subviewAtIndex(int index) override {
    assert(index == 0);
    return &m_textView;
  }

  Escher::OneLineBufferTextView<> m_textView;
};

}  // namespace Code
