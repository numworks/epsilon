#pragma once

#include <escher/scrollable_multiple_layouts_view.h>

namespace Escher {

class ScrollableTwoLayoutsCell : public EvenOddCell, public Responder {
 public:
  ScrollableTwoLayoutsCell(Responder* parentResponder = nullptr,
                           float horizontalAlignment = KDGlyph::k_alignLeft,
                           KDFont::Size font = KDFont::Size::Large);
  void setLayouts(const Poincare::Layout& exactLayout,
                  const Poincare::Layout& approximateLayout) {
    m_view.setLayouts(Poincare::Layout(), exactLayout, approximateLayout);
  }
  void resetLayouts() { return m_view.resetLayouts(); }
  void setComparisonSign(bool isEqual, bool isVisible) {
    return m_view.setComparisonSign(isEqual, isVisible);
  }
  void setHighlighted(bool highlight) override {
    m_view.evenOddCell()->setHighlighted(highlight);
  }
  void setEven(bool even) override;
  Responder* responder() override { return this; }
  const Poincare::Layout& layout() const override { return m_view.layout(); }
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
