#include "graph_view.h"

GraphView::GraphView() :
  View(),
  m_cursorView(CursorView()),
  m_cursorPosition(KDPointZero)
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
  KDCoordinate x_grid_step = m_frame.width/10;
  KDCoordinate y_grid_step = m_frame.height/10;
  KDColor gridColor = KDColorGray(0xEE);
  for (KDCoordinate x=0; x<m_frame.width; x += x_grid_step) {
    KDRect verticalGridRect;
    verticalGridRect.x = x;
    verticalGridRect.y = 0;
    verticalGridRect.width = 1;
    verticalGridRect.height = m_frame.height;
    KDFillRect(verticalGridRect, gridColor);
  }
  for (KDCoordinate y=0; y<m_frame.height; y += y_grid_step) {
    KDRect horizontalGridRect;
    horizontalGridRect.x = 0;
    horizontalGridRect.y = y;
    horizontalGridRect.width = m_frame.width;
    horizontalGridRect.height = 1;
    KDFillRect(horizontalGridRect, gridColor);
  }
  for (int i=0; i<m_frame.width; i++) {
    KDPoint p;
    p.x = i;
    p.y = (i*i)/m_frame.height;
    KDSetPixel(p, KDColorRGB(0x7F, 0, 0));
  }
};
