#include "graph_view.h"
#include <assert.h>

constexpr KDColor kAxisColor = KDColorGray(0x0);
constexpr KDColor kMainGridColor = KDColorGray(0xCC);
constexpr KDColor kSecondaryGridColor = KDColorGray(0xEE);
constexpr int kNumberOfMainGridLines = 5;
constexpr int kNumberOfSecondaryGridLines = 4;

GraphView::GraphView() :
  View(),
  m_cursorView(CursorView()),
  m_cursorPosition(KDPointZero),
  m_xMin(-2.0f),
  m_xMax(2.0f),
  m_yMin(-2.0f),
  m_yMax(2.0f)
{
}

int GraphView::numberOfSubviews() const {
  return 1;
};

View * GraphView::subviewAtIndex(int index) {
  return &m_cursorView;
}

void GraphView::moveCursorRight() {
  m_cursorPosition.x = m_cursorPosition.x + 2;
  layoutSubviews();
}

void GraphView::layoutSubviews() {
  KDRect cursorFrame;
  cursorFrame.origin = m_cursorPosition;
  cursorFrame.width = 10;
  cursorFrame.height = 10;
  m_cursorView.setFrame(cursorFrame);
}

void GraphView::drawRect(KDRect rect) const {
  KDColor backgroundColor = KDColorWhite;
  KDFillRect(rect, &backgroundColor, 1);
  drawGrid(rect);
  drawAxes(rect);
  drawFunction(rect);
}

void GraphView::drawLine(KDRect rect, Axis axis, float coordinate, KDColor color) const {
  KDRect lineRect;
  switch(axis) {
    case Axis::Horizontal:
      lineRect.x = rect.x;
      lineRect.y = floatToPixel(Axis::Vertical, coordinate);
      lineRect.width = rect.width;
      lineRect.height = 1;
      break;
    case Axis::Vertical:
      lineRect.x = floatToPixel(Axis::Horizontal, coordinate);
      lineRect.y = rect.y;
      lineRect.width = 1;
      lineRect.height = rect.height;
      break;
  }
  KDFillRect(lineRect, &color, 1);
}

void GraphView::drawAxes(KDRect rect) const {
  drawLine(rect, Axis::Horizontal, 0.0f, kAxisColor);
  drawLine(rect, Axis::Vertical, 0.0f, kAxisColor);
}

void GraphView::drawGridLines(KDRect rect, Axis axis, int count, KDColor color) const {
  float range = max(axis)-min(axis);
  float step = range/count;
  float start = step*((int)(min(axis)/step));
  for (int i=0; i<count; i++) {
    Axis otherAxis = (axis == Axis::Horizontal) ? Axis::Vertical : Axis::Horizontal;
    drawLine(rect, otherAxis, start+i*step, color);
  }
}

void GraphView::drawGrid(KDRect rect) const {
  drawGridLines(rect, Axis::Horizontal, kNumberOfMainGridLines*kNumberOfSecondaryGridLines, kSecondaryGridColor);
  drawGridLines(rect, Axis::Vertical, kNumberOfMainGridLines*kNumberOfSecondaryGridLines, kSecondaryGridColor);

  drawGridLines(rect, Axis::Horizontal, kNumberOfMainGridLines, kMainGridColor);
  drawGridLines(rect, Axis::Vertical, kNumberOfMainGridLines, kMainGridColor);
}

float GraphView::min(Axis axis) const {
  assert(axis == Axis::Horizontal || axis == Axis::Vertical);
  return (axis == Axis::Horizontal ? m_xMin : m_yMin);
}

float GraphView::max(Axis axis) const {
  assert(axis == Axis::Horizontal || axis == Axis::Vertical);
  return (axis == Axis::Horizontal ? m_xMax : m_yMax);
}

KDCoordinate GraphView::pixelLength(Axis axis) const {
  assert(axis == Axis::Horizontal || axis == Axis::Vertical);
  return (axis == Axis::Horizontal ? m_frame.width : m_frame.height);
}

float GraphView::pixelToFloat(Axis axis, KDCoordinate p) const {
  return min(axis) + p*(max(axis)-min(axis))/pixelLength(axis);
}

KDCoordinate GraphView::floatToPixel(Axis axis, float f) const {
  return pixelLength(axis)*(max(axis)-f)/(max(axis)-min(axis));
}

void GraphView::drawFunction(KDRect rect) const {
  KDPoint p = KDPointZero;
  for (p.x=rect.x; p.x<(rect.x+rect.width); p.x++) {
    float x = pixelToFloat(Axis::Horizontal, p.x);
    float y = (x-1)*(x+1)*x;
    p.y = floatToPixel(Axis::Vertical, y);
    KDSetPixel(p, KDColorRGB(0xCF, 0, 0));
  }
}
