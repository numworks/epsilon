#ifndef ESCHER_SCROLLABLE_VIEW_H
#define ESCHER_SCROLLABLE_VIEW_H

#include <escher/scroll_view.h>
#include <escher/responder.h>

class ScrollableView : public Responder, public ScrollView {
public:
 ScrollableView(Responder * parentResponder, View * view, ScrollViewDataSource * dataSource);
  bool handleEvent(Ion::Events::Event event) override;
  void reloadScroll(bool forceRelayout = false);
protected:
  void layoutSubviews() override;
  KDPoint m_manualScrollingOffset;
};

#endif


