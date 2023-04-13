#ifndef ESCHER_EVEN_ODD_BUFFER_TEXT_CELL_H
#define ESCHER_EVEN_ODD_BUFFER_TEXT_CELL_H

#include <escher/buffer_text_view.h>
#include <escher/even_odd_cell.h>

namespace Escher {

class AbstractEvenOddBufferTextCell : public EvenOddCell {
 public:
  constexpr static int k_defaultPrecision =
      Poincare::Preferences::VeryLargeNumberOfSignificantDigits;
  const char* text() const override { return bufferTextView()->text(); }
  void setFont(KDFont::Size font) { bufferTextView()->setFont(font); }
  KDFont::Size font() const { return bufferTextView()->font(); }
  void setAlignment(float horizontalAlignment, float verticalAlignment) {
    bufferTextView()->setAlignment(horizontalAlignment, verticalAlignment);
  }
  void setText(const char* textContent) {
    bufferTextView()->setText(textContent);
  }
  void setTextColor(KDColor textColor) {
    bufferTextView()->setTextColor(textColor);
  }

 protected:
  void updateSubviewsBackgroundAfterChangingState() override;
  int numberOfSubviews() const override;
  View* subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  virtual AbstractBufferTextView* bufferTextView() = 0;
  AbstractBufferTextView* bufferTextView() const {
    return const_cast<AbstractEvenOddBufferTextCell*>(this)->bufferTextView();
  };
};

template <class BufferTextViewClass>
class EvenOddBufferTextCell : public AbstractEvenOddBufferTextCell {
#ifndef PLATFORM_DEVICE
  // Poor's man constraint
  static_assert(
      std::is_base_of<AbstractBufferTextView, BufferTextViewClass>(),
      "EvenOddBufferTextCell must be templated with a BufferTextView class");
#endif
 public:
  EvenOddBufferTextCell(KDGlyph::Format format = k_smallCellDefaultFormat)
      : AbstractEvenOddBufferTextCell(), m_bufferTextView(format) {}

 private:
  AbstractBufferTextView* bufferTextView() override {
    return &m_bufferTextView;
  }
  BufferTextViewClass m_bufferTextView;
};

template <int numberOfSignificantDigits =
              AbstractEvenOddBufferTextCell::k_defaultPrecision>
using FloatEvenOddBufferTextCell =
    EvenOddBufferTextCell<FloatBufferTextView<numberOfSignificantDigits>>;
using SmallFontEvenOddBufferTextCell =
    EvenOddBufferTextCell<OneLineBufferTextView<KDFont::Size::Small>>;

}  // namespace Escher

#endif
