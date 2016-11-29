#ifndef GRAPH_CURSOR_VIEW_H
#define GRAPH_CURSOR_VIEW_H

#include <escher.h>

namespace Graph {
class CursorView : public View {
public:
  using View::View;
  void setPosition(float xPosition, float yPosition);
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  float m_xPosition;
  float m_yPosition;
};

}

#endif
