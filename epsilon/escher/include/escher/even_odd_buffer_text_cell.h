#pragma once

#include <escher/buffer_text_view.h>
#include <escher/even_odd_cell.h>

namespace Escher {

class AbstractEvenOddTableBufferTextCell : public EvenOddCell {
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
  void setTextColor(ThemeComponent textColor) {
    bufferTextView()->setTextColor(textColor);
  }
  void setHighlighted(bool highlighted) override;

 protected:
  void updateSubviewsBackgroundAfterChangingState() override;
  int numberOfSubviews() const override;
  View* subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  ThemeComponent baseColor() const override { return BaseTableColor(m_even); }
  virtual AbstractBufferTextView* bufferTextView() = 0;
  AbstractBufferTextView* bufferTextView() const {
    return const_cast<AbstractEvenOddTableBufferTextCell*>(this)
        ->bufferTextView();
  };
};

template <class BufferTextViewClass>
class EvenOddTableBufferTextCell : public AbstractEvenOddTableBufferTextCell {
#ifndef PLATFORM_DEVICE
  // Poor man's constraint
  static_assert(std::is_base_of<AbstractBufferTextView, BufferTextViewClass>(),
                "EvenOddTableBufferTextCell must be templated with a "
                "BufferTextView class");
#endif
 public:
  EvenOddTableBufferTextCell(
      const TextFormat& format = k_smallCellDefaultFormat)
      : AbstractEvenOddTableBufferTextCell(), m_bufferTextView(format) {}

 private:
  AbstractBufferTextView* bufferTextView() override {
    return &m_bufferTextView;
  }
  BufferTextViewClass m_bufferTextView;
};

/* Overrides EvenOddTableBufferTextCell baseColor() to use the default
 * (non-table) even/odd color scheme */
template <class BufferTextViewClass>
class EvenOddBufferTextCell
    : public EvenOddTableBufferTextCell<BufferTextViewClass> {
 public:
  using EvenOddTableBufferTextCell<
      BufferTextViewClass>::EvenOddTableBufferTextCell;

 protected:
  ThemeComponent baseColor() const override { return EvenOddCell::baseColor(); }
};

template <int numberOfSignificantDigits =
              AbstractEvenOddTableBufferTextCell::k_defaultPrecision>
using FloatEvenOddTableBufferTextCell =
    EvenOddTableBufferTextCell<FloatBufferTextView<numberOfSignificantDigits>>;
using SmallFontEvenOddTableBufferTextCell =
    EvenOddTableBufferTextCell<OneLineBufferTextView<KDFont::Size::Small>>;
using SmallFontEvenOddBufferTextCell =
    EvenOddBufferTextCell<OneLineBufferTextView<KDFont::Size::Small>>;

}  // namespace Escher
