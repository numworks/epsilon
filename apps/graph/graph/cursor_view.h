#ifndef GRAPH_CURSOR_VIEW_H
#define GRAPH_CURSOR_VIEW_H

#include <escher.h>

class CursorView : public ChildlessView {
public:
  using ChildlessView::ChildlessView;
  void drawRect(KDRect rect) const override;
};

#endif
