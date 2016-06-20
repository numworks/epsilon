#ifndef GRAPH_GRAPH_VIEW_H
#define GRAPH_GRAPH_VIEW_H

#include <escher.h>
#include "cursor_view.h"

class GraphView : public View {
public:
  GraphView();
  void drawRect(KDRect rect) const override;
  void moveCursorRight();
private:
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;

  CursorView m_cursorView;
  KDPoint m_cursorPosition;
};

#endif
