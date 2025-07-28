#ifndef ESCHER_TEXT_HIGHLIGHT_VIEW_H
#define ESCHER_TEXT_HIGHLIGHT_VIEW_H

#include <escher/buffer_text_view.h>
#include <escher/highlight_cell.h>

namespace Escher {

class SmallBufferTextHighlightCell : public HighlightCell {
 public:
  void setText(const char* text) { m_textView.setText(text); }
  View* subviewAtIndex(int i) override { return &m_textView; }
  int numberOfSubviews() const override { return 1; }
  KDSize minimalSizeForOptimalDisplay() const override {
    return m_textView.minimalSizeForOptimalDisplay();
  }
  void layoutSubviews(bool force) override {
    setChildFrame(&m_textView, bounds(), force);
  }
  void setHighlighted(bool highlight) override {
    m_textView.setBackgroundColor(highlight ? Palette::Select : KDColorWhite);
  }

 private:
  // ad-hoc currently µ1-µ2≠1.123E7
  constexpr static size_t k_bufferSize = 20;
  BufferTextView<k_bufferSize> m_textView;
};

}  // namespace Escher

#endif
