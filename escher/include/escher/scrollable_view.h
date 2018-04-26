#ifndef ESCHER_SCROLLABLE_VIEW_H
#define ESCHER_SCROLLABLE_VIEW_H

#include <escher/palette.h>
#include <escher/responder.h>
#include <escher/scroll_view.h>

class ScrollableView : public Responder, public ScrollView {
public:
 ScrollableView(Responder * parentResponder, View * view, ScrollViewDataSource * dataSource,
     KDCoordinate leftMargin = 0, KDCoordinate rightMargin = 0, KDCoordinate topMargin = 0,
     KDCoordinate bottomMargin = 0, bool showIndicators = false, bool colorBackground = false,
     KDColor backgroundColor = Palette::WallScreen);
  bool handleEvent(Ion::Events::Event event) override;
  void reloadScroll(bool forceRelayout = false);
protected:
  void layoutSubviews() override;
  KDPoint m_manualScrollingOffset;
};

#endif


