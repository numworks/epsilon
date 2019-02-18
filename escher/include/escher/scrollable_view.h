#ifndef ESCHER_SCROLLABLE_VIEW_H
#define ESCHER_SCROLLABLE_VIEW_H

#include <escher/palette.h>
#include <escher/responder.h>
#include <escher/scroll_view.h>

class ScrollableView : public Responder, public ScrollView {
public:
  ScrollableView(Responder * parentResponder, View * view, ScrollViewDataSource * dataSource);
  bool handleEvent(Ion::Events::Event event) override;
  void reloadScroll(bool forceRelayout = false);
protected:
  KDSize contentSize() const override;
  KDPoint m_manualScrollingOffset;
};

#endif


