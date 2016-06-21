#include "graph_view.h"

constexpr KDColor kAxisColor = KDColorGray(0x0);
constexpr KDColor kMainGridColor = KDColorGray(0xCC);
constexpr KDColor kSecondaryGridColor = KDColorGray(0xEE);
constexpr int kNumberOfMainGridLines = 5;
constexpr int kNumberOfSecondaryGridLines = 5;

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
  KDFillRect(rect, KDColorWhite);
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
  KDFillRect(lineRect, color);
}

void GraphView::drawAxes(KDRect rect) const {
  drawLine(rect, Axis::Horizontal, 0.0f, kAxisColor);
  drawLine(rect, Axis::Vertical, 0.0f, kAxisColor);
}

void GraphView::drawGrid(KDRect rect) const {
  float xRange = m_xMax - m_xMin;
  float xGridStep = xRange/kNumberOfMainGridLines;
  float xGridStart = xGridStep*((int)(m_xMin/xGridStep));
  for (int i=0; i<kNumberOfMainGridLines; i++) {
    drawLine(rect, Axis::Vertical, xGridStart+i*xGridStep, kMainGridColor);
  }

  float yRange = m_yMax - m_yMin;
  float yGridStep = yRange/kNumberOfMainGridLines;
  float yGridStart = yGridStep*((int)(m_yMin/yGridStep));
  for (int i=0; i<kNumberOfMainGridLines; i++) {
    drawLine(rect, Axis::Horizontal, yGridStart+i*yGridStep, kMainGridColor);
  }
}

float GraphView::pixelToFloat(Axis axis, KDCoordinate p) const {
  float min, max;
  KDCoordinate length;

  switch (axis) {
    case Axis::Horizontal:
      min = m_xMin;
      max = m_xMax;
      length = m_frame.width;
      break;
    case Axis::Vertical:
      min = m_yMin;
      max = m_yMax;
      length = m_frame.height;
      break;
  }

  return min + p*(max-min)/length;
}

KDCoordinate GraphView::floatToPixel(Axis axis, float f) const {
  float min, max;
  KDCoordinate length;

  switch (axis) {
    case Axis::Horizontal:
      min = m_xMin;
      max = m_xMax;
      length = m_frame.width;
      break;
    case Axis::Vertical:
      min = m_yMin;
      max = m_yMax;
      length = m_frame.height;
      break;
  }

  return length*(max-f)/(max-min);

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
