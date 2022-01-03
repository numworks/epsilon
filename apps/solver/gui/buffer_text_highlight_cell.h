#ifndef SOLVER_GUI_TEXT_HIGHLIGHT_VIEW_H
#define SOLVER_GUI_TEXT_HIGHLIGHT_VIEW_H

#include <escher/buffer_text_view.h>
#include <escher/highlight_cell.h>

namespace Solver {

class BufferTextHighlightCell : public Escher::HighlightCell {
public:
  void setText(const char * text) { m_textView.setText(text); }
  Escher::View * subviewAtIndex(int i) override { return &m_textView; }
  int numberOfSubviews() const override { return 1; }
  KDSize minimalSizeForOptimalDisplay() const override {
    return m_textView.minimalSizeForOptimalDisplay();
  }
  void layoutSubviews(bool force) override { m_textView.setFrame(bounds(), force); }
  void setHighlighted(bool highlight) override {
    m_textView.setBackgroundColor(highlight ? Escher::Palette::Select : KDColorWhite);
  }

private:
  Escher::BufferTextView m_textView;
};

}  // namespace Solver

#endif /* SOLVER_GUI_TEXT_HIGHLIGHT_VIEW_H */
