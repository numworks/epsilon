#ifndef SHARED_BUFFER_FUNCTION_TITLE_CELL_H
#define SHARED_BUFFER_FUNCTION_TITLE_CELL_H

#include <escher/even_odd_buffer_text_cell.h>
#include <escher/even_odd_message_text_cell.h>

#include "function_title_cell.h"

namespace Shared {

class BufferFunctionTitleCell : public FunctionTitleCell {
 public:
  BufferFunctionTitleCell(KDGlyph::Format format = {
                              .horizontalAlignment = KDGlyph::k_alignCenter});
  void setEven(bool even) override;
  void setHighlighted(bool highlight) override;
  void setColor(KDColor color) override;
  void setText(const char* textContent);
  void setFont(KDFont::Size font) { m_bufferTextView.setFont(font); }
  void setHorizontalAlignment(const float alignment) {
    return m_bufferTextView.setAlignment(alignment, k_verticalAlignment);
  }
  const char* text() const override { return m_bufferTextView.text(); }
  int numberOfSubviews() const override;
  View* subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;

 protected:
  KDRect bufferTextViewFrame() const;
  Escher::EvenOddBufferTextCell* bufferTextView() { return &m_bufferTextView; }

 private:
  constexpr static float k_verticalAlignment = KDGlyph::k_alignCenter;

  Escher::EvenOddBufferTextCell m_bufferTextView;
};

}  // namespace Shared

#endif
