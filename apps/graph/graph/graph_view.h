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

  float xPixelToFloat(int xPixel) const;
  float yPixelToFloat(int yPixel) const;
  KDCoordinate floatToXPixel(float x) const;
  KDCoordinate floatToYPixel(float y) const;

  void drawHorizontalLine(KDRect rect, float y, KDColor color) const;
  void drawVerticalLine(KDRect rect, float x, KDColor color) const;

  void drawAxis(KDRect rect) const;
  void drawGrid(KDRect rect) const;
  void drawFunction(KDRect rect) const;

  CursorView m_cursorView;
  KDPoint m_cursorPosition;

  float m_xMin;
  float m_xMax;
  float m_yMin;
  float m_yMax;
};

#endif
