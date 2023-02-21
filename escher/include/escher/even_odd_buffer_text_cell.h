#ifndef ESCHER_EVEN_ODD_BUFFER_TEXT_CELL_H
#define ESCHER_EVEN_ODD_BUFFER_TEXT_CELL_H

#include <escher/buffer_text_view.h>
#include <escher/even_odd_cell.h>

namespace Escher {

class EvenOddBufferTextCell : public EvenOddCell {
 public:
  EvenOddBufferTextCell(KDFont::Size font = KDFont::Size::Small,
                        float horizontalAlignment = KDContext::k_alignRight,
                        float verticalAlignment = KDContext::k_alignCenter);
  const char* text() const override;
  void setFont(KDFont::Size font) { m_bufferTextView.setFont(font); }
  KDFont::Size font() const { return m_bufferTextView.font(); }
  void setAlignment(float horizontalAlignment, float verticalAlignment) {
    m_bufferTextView.setAlignment(horizontalAlignment, verticalAlignment);
  }
  void setText(const char* textContent);
  void setTextColor(KDColor textColor);

 protected:
  void updateSubviewsBackgroundAfterChangingState() override;
  int numberOfSubviews() const override;
  View* subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  BufferTextView m_bufferTextView;
};

}  // namespace Escher

#endif
