#ifndef ESCHER_SCROLLABLE_VIEW_H
#define ESCHER_SCROLLABLE_VIEW_H

#include <escher/palette.h>
#include <escher/responder.h>
#include <escher/scroll_view.h>

namespace Escher {

class AbstractScrollableView : public Responder, public ScrollView {
 public:
  AbstractScrollableView(Responder* parentResponder, View* view,
                         ScrollViewDataSource* dataSource);
  bool handleEvent(Ion::Events::Event event) override;
  void reloadScroll(bool forceRelayout = false);

 protected:
  KDSize contentSize() const override;
};

template <typename D>
class ScrollableView : public AbstractScrollableView {
  using AbstractScrollableView::AbstractScrollableView;
  Decorator* decorator() override { return &m_decorator; }
  D m_decorator;
};

template <>
class ScrollableView<ScrollView::ArrowDecorator>
    : public AbstractScrollableView {
  using AbstractScrollableView::AbstractScrollableView;

 protected:
  void setDecoratorFont(KDFont::Size font) { m_decorator.setFont(font); }

 private:
  Decorator* decorator() override { return &m_decorator; }
  ArrowDecorator m_decorator;
};

}  // namespace Escher
#endif
