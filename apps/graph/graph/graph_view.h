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

  enum class Axis {
    Horizontal = 0,
    Vertical = 1
  };

  float min(Axis axis) const;
  float max(Axis axis) const;
  KDCoordinate pixelLength(Axis axis) const;

  float pixelToFloat(Axis axis, KDCoordinate p) const;
  KDCoordinate floatToPixel(Axis axis, float f) const;

  void drawLine(KDRect rect, Axis axis,
      float coordinate, KDColor color) const;

  void drawAxes(KDRect rect) const;
  void drawGrid(KDRect rect) const;
  void drawGridLines(KDRect rect, Axis axis, int count, KDColor color) const;
  void drawFunction(KDRect rect) const;

  CursorView m_cursorView;
  KDPoint m_cursorPosition;

  float m_xMin;
  float m_xMax;
  float m_yMin;
  float m_yMax;
};

#endif
