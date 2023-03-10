#ifndef ESCHER_SCROLLABLE_EXPRESSION_VIEW_H
#define ESCHER_SCROLLABLE_EXPRESSION_VIEW_H

#include <escher/expression_view.h>
#include <escher/scroll_view_data_source.h>
#include <escher/scrollable_view.h>

namespace Escher {

class ScrollableExpressionView
    : public ScrollableView<ScrollView::ArrowDecorator>,
      public ScrollViewDataSource {
 public:
  ScrollableExpressionView(Responder* parentResponder,
                           KDCoordinate leftRightMargin,
                           KDCoordinate topBottomMargin,
                           KDGlyph::Format format = {});
  Poincare::Layout layout() const;
  KDFont::Size font() const { return m_expressionView.font(); }
  void setFont(KDFont::Size font) {
    m_expressionView.setFont(font);
    decorator()->setFont(font);
  }
  void setLayout(Poincare::Layout layout);
  void setTextColor(KDColor color) { m_expressionView.setTextColor(color); }
  void setBackgroundColor(KDColor backgroundColor) override;
  void setExpressionBackgroundColor(KDColor backgroundColor);

 private:
  ExpressionView m_expressionView;
};

}  // namespace Escher
#endif
