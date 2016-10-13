#ifndef GRAPH_CURSOR_VIEW_H
#define GRAPH_CURSOR_VIEW_H

#include <escher.h>

namespace Graph {
class CursorView : public ChildlessView {
public:
  using ChildlessView::ChildlessView;
  void drawRect(KDContext * ctx, KDRect rect) const override;
};

}

#endif
