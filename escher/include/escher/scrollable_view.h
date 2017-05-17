#ifndef ESCHER_SCROLLABLE_VIEW_H
#define ESCHER_SCROLLABLE_VIEW_H

#include <escher/scroll_view.h>
#include <escher/responder.h>

class ScrollableView : public Responder, public ScrollView {
public:
 ScrollableView(Responder * parentResponder, View * view, ScrollViewDataSource * dataSource);
  bool handleEvent(Ion::Events::Event event) override;
  void reloadScroll();
protected:
  void layoutSubviews() override;
  bool rightViewIsInvisible();
  KDCoordinate m_manualScrolling;
private:
  virtual View * view() = 0;
};

#endif


