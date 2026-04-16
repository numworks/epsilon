#pragma once

#include <escher/even_odd_cell.h>
#include <escher/layout_view.h>
#include <kandinsky/margins.h>

namespace Escher {

class EvenOddExpressionCell : public EvenOddCell {
 public:
  EvenOddExpressionCell(const TextFormat& format = {});
  void setLayout(const Poincare::Layout& layout);
  void setBackgroundColor(ThemeComponent backgroundColor);
  void setTextColor(ThemeComponent textColor);
  void setFont(KDFont::Size font) { m_layoutView.setFont(font); }
  KDSize minimalSizeForOptimalDisplay() const override;
  void setAlignment(float horizontalAlignment, float verticalAlignment) {
    m_layoutView.setAlignment(horizontalAlignment, verticalAlignment);
  }
  KDHorizontalMargins margins() const { return m_margins; }
  void setMargins(KDHorizontalMargins margins);
  KDPoint drawingOrigin() const { return m_layoutView.drawingOrigin(); }
  const Poincare::Layout& layout() const override {
    return m_layoutView.layout();
  }
  KDFont::Size font() const { return m_layoutView.font(); }
  void drawRect(KDContext* ctx, KDRect rect) const override;

 protected:
  void updateSubviewsBackgroundAfterChangingState() override;
  int numberOfSubviews() const override;
  View* subviewAtIndex(int index) override;
  void layoutSubviews(bool force = false) override;
  LayoutView m_layoutView;
  KDHorizontalMargins m_margins;
};

}  // namespace Escher
