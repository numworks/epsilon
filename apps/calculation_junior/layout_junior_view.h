#ifndef CALCULATION_JUNIOR_LAYOUT_JUNIOR_VIEW_H
#define CALCULATION_JUNIOR_LAYOUT_JUNIOR_VIEW_H

#include <escher/view.h>
#include <kandinsky/font.h>
#include <poincare_junior/include/layout.h>

namespace CalculationJunior {

class LayoutJuniorView : public Escher::View {
 public:
  LayoutJuniorView(KDFont::Size font = KDFont::Size::Large)
      : View(), m_font(font), m_layout(PoincareJ::Layout()) {}
  void drawRect(KDContext* ctx, KDRect rect) const override;
  void setLayout(PoincareJ::Layout layout);
  const PoincareJ::Layout* layout() const { return &m_layout; }

 protected:
  KDFont::Size m_font;
  PoincareJ::Layout m_layout;
};

}  // namespace CalculationJunior
#endif
