#ifndef ESCHER_SCROLLABLE_TWO_LAYOUTS_CELL_H
#define ESCHER_SCROLLABLE_TWO_LAYOUTS_CELL_H

#include <escher/scrollable_multiple_layouts_view.h>

namespace Escher {

class ScrollableTwoLayoutsCell : public EvenOddCell, public Responder {
 public:
  ScrollableTwoLayoutsCell(Responder* parentResponder = nullptr,
                           float horizontalAlignment = KDGlyph::k_alignLeft,
                           KDFont::Size font = KDFont::Size::Large);
  void setLayouts(Poincare::Layout exactLayout,
                  Poincare::Layout approximateLayout) {
    m_view.setLayouts(Poincare::Layout(), exactLayout, approximateLayout);
  }
  void resetLayouts() { return m_view.resetLayouts(); }
  void setExactAndApproximateAreStriclyEqual(bool isEqual) {
    return m_view.setExactAndApproximateAreStriclyEqual(isEqual);
  }
  void setHighlighted(bool highlight) override {
    m_view.evenOddCell()->setHighlighted(highlight);
  }
  void setEven(bool even) override;
  Responder* responder() override { return this; }
  Poincare::Layout layout() const override { return m_view.layout(); }
  KDSize minimalSizeForOptimalDisplay() const override {
    return m_view.minimalSizeForOptimalDisplay();
  }
  void reinitSelection();

 protected:
  void handleResponderChainEvent(ResponderChainEvent event) override;

 private:
  int numberOfSubviews() const override { return 1; }
  View* subviewAtIndex(int index) override { return &m_view; }
  void layoutSubviews(bool force = false) override {
    setChildFrame(&m_view, bounds(), force);
  }
  ScrollableTwoLayoutsView m_view;
};

}  // namespace Escher

#endif
