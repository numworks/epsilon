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
  drawAxis(rect);
  drawFunction(rect);
}

void GraphView::drawHorizontalLine(KDRect rect, float y, KDColor color) const {
  KDRect lineRect;
  lineRect.x = rect.x;
  lineRect.y = floatToYPixel(y);
  lineRect.width = rect.width;
  lineRect.height = 1;
  KDFillRect(lineRect, color);
}

void GraphView::drawVerticalLine(KDRect rect, float x, KDColor color) const {
  KDRect lineRect;
  lineRect.x = floatToXPixel(x);
  lineRect.y = rect.y;
  lineRect.width = 1;
  lineRect.height = rect.height;
  KDFillRect(lineRect, color);
}

void GraphView::drawAxis(KDRect rect) const {
  drawHorizontalLine(rect, 0.0f, kAxisColor);
  drawVerticalLine(rect, 0.0f, kAxisColor);
}

void GraphView::drawGrid(KDRect rect) const {
  float xRange = m_xMax - m_xMin;
  float xGridStep = xRange/kNumberOfMainGridLines;
  float xGridStart = xGridStep*((int)(m_xMin/xGridStep));
  for (int i=0; i<kNumberOfMainGridLines; i++) {
    drawVerticalLine(rect, xGridStart+i*xGridStep, kMainGridColor);
  }

  float yRange = m_yMax - m_yMin;
  float yGridStep = yRange/kNumberOfMainGridLines;
  float yGridStart = yGridStep*((int)(m_yMin/yGridStep));
  for (int i=0; i<kNumberOfMainGridLines; i++) {
    drawHorizontalLine(rect, yGridStart+i*yGridStep, kMainGridColor);
  }
}

float GraphView::xPixelToFloat(int xPixel) const {
  return m_xMin + xPixel*(m_xMax-m_xMin)/m_frame.width;
}

float GraphView::yPixelToFloat(int yPixel) const {
  return m_yMin + yPixel*(m_yMax-m_yMin)/m_frame.height;
}

KDCoordinate GraphView::floatToYPixel(float y) const {
  return m_frame.height*(m_yMax-y)/(m_yMax-m_yMin);
}

KDCoordinate GraphView::floatToXPixel(float x) const {
  return m_frame.width*(m_xMax-x)/(m_xMax-m_xMin);
}

void GraphView::drawFunction(KDRect rect) const {
  KDPoint p = KDPointZero;
  for (p.x=rect.x; p.x<(rect.x+rect.width); p.x++) {
    float x = xPixelToFloat(p.x);
    float y = (x-1)*(x+1)*x;
    p.y = floatToYPixel(y);
    KDSetPixel(p, KDColorRGB(0x7F, 0, 0));
  }
}
