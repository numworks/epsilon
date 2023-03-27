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
  void reloadScroll();

 protected:
  KDSize contentSize() const override;
};

template <typename T>
class ScrollableView : public AbstractScrollableView {
  using AbstractScrollableView::AbstractScrollableView;

 protected:
  T* decorator() override { return &m_decorator; }

 private:
  T m_decorator;
};

}  // namespace Escher
#endif
