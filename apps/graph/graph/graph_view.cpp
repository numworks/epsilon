#include "graph_view.h"
#include <assert.h>

namespace Graph {

constexpr KDColor kAxisColor = KDColor::RGB24(0x000000);
constexpr KDColor kMainGridColor = KDColor::RGB24(0xCCCCCC);
constexpr KDColor kSecondaryGridColor = KDColor::RGB24(0xEEEEEE);
constexpr int kNumberOfMainGridLines = 5;
constexpr int kNumberOfSecondaryGridLines = 4;
constexpr KDCoordinate GraphView::k_stampSize;
constexpr uint8_t GraphView::k_mask[k_stampSize*k_stampSize];

GraphView::GraphView(FunctionStore * functionStore, AxisInterval * axisInterval) :
#if GRAPH_VIEW_IS_TILED
  TiledView(),
#else
  View(),
#endif
  m_cursorView(CursorView()),
  m_cursorPosition(KDPointZero),
  m_axisInterval(axisInterval),
  m_functionStore(functionStore),
  m_evaluateContext(nullptr)
{
}

int GraphView::numberOfSubviews() const {
  return 1;
};

View * GraphView::subviewAtIndex(int index) {
  return &m_cursorView;
}

void GraphView::setContext(Context * context) {
  m_evaluateContext = (EvaluateContext *)context;
}

Context * GraphView::context() const {
  return m_evaluateContext;
}

void GraphView::moveCursorRight() {
  KDPoint offset = KDPoint(2,0);
  m_cursorPosition = m_cursorPosition.translatedBy(offset);
  layoutSubviews();
}

void GraphView::layoutSubviews() {
  KDRect cursorFrame(m_cursorPosition, 10, 10);
  m_cursorView.setFrame(cursorFrame);
}

#if GRAPH_VIEW_IS_TILED
KDColor * GraphView::tile() const {
  return (KDColor *)m_tile;
}

KDSize GraphView::tileSize() const {
  return {kTileWidth, kTileHeight};
}

void GraphView::drawTile(KDContext * ctx, KDRect rect) const {
#else
void GraphView::drawRect(KDContext * ctx, KDRect rect) const {
#endif
  ctx->fillRect(rect, KDColorWhite);
  drawGrid(ctx, rect);
  drawAxes(ctx, rect);
  drawFunction(ctx, rect);
  /*

  constexpr int maskLength = 3;
  uint8_t mask[maskLength] = {
#if 1
    0x10, 0x70, 0xE0,
#else
    0x00, 0x30, 0x73, 0x30, 0x00,
    0x30, 0xfb, 0xff, 0xfb, 0x30,
    0x73, 0xff, 0xff, 0xff, 0x73,
    0x30, 0xfb, 0xff, 0xfb, 0x30,
    0x00, 0x30, 0x73, 0x30, 0x00
#endif
  };

  KDColor red = KDColorRed;
  KDBlitRect(rect, &red, 1, mask, maskLength);
  */
}

void GraphView::drawLine(KDContext * ctx, KDRect rect, Axis axis, float coordinate, KDColor color, KDCoordinate thickness) const {
  KDRect lineRect = KDRectZero;
  switch(axis) {
    case Axis::Horizontal:
      lineRect = KDRect(
          rect.x(), floatToPixel(Axis::Vertical, coordinate),
          rect.width(), thickness
          );
      break;
    case Axis::Vertical:
      lineRect = KDRect(
          floatToPixel(Axis::Horizontal, coordinate), rect.y(),
          thickness, rect.height()
      );
      break;
  }
  ctx->fillRect(lineRect, color);
}

void GraphView::drawAxes(KDContext * ctx, KDRect rect) const {
  drawLine(ctx, rect, Axis::Horizontal, 0.0f, kAxisColor, 2);
  drawLine(ctx, rect, Axis::Vertical, 0.0f, kAxisColor, 2);
}

void GraphView::drawGridLines(KDContext * ctx, KDRect rect, Axis axis, int count, KDColor color) const {
  float range = max(axis)-min(axis);
  float step = range/count;
  float start = step*((int)(min(axis)/step));
  for (int i=0; i<count; i++) {
    Axis otherAxis = (axis == Axis::Horizontal) ? Axis::Vertical : Axis::Horizontal;
    drawLine(ctx, rect, otherAxis, start+i*step, color);
  }
}

void GraphView::drawGrid(KDContext * ctx, KDRect rect) const {
  drawGridLines(ctx, rect, Axis::Horizontal, kNumberOfMainGridLines*kNumberOfSecondaryGridLines, kSecondaryGridColor);
  drawGridLines(ctx, rect, Axis::Vertical, kNumberOfMainGridLines*kNumberOfSecondaryGridLines, kSecondaryGridColor);

  drawGridLines(ctx, rect, Axis::Horizontal, kNumberOfMainGridLines, kMainGridColor);
  drawGridLines(ctx, rect, Axis::Vertical, kNumberOfMainGridLines, kMainGridColor);
}

float GraphView::min(Axis axis) const {
  assert(axis == Axis::Horizontal || axis == Axis::Vertical);
  return (axis == Axis::Horizontal ? m_axisInterval->xMin() : m_axisInterval->yMin());
}

float GraphView::max(Axis axis) const {
  assert(axis == Axis::Horizontal || axis == Axis::Vertical);
  return (axis == Axis::Horizontal ? m_axisInterval->xMax() : m_axisInterval->yMax());
}

KDCoordinate GraphView::pixelLength(Axis axis) const {
  assert(axis == Axis::Horizontal || axis == Axis::Vertical);
  return (axis == Axis::Horizontal ? m_frame.width() : m_frame.height());
}

float GraphView::pixelToFloat(Axis axis, KDCoordinate p) const {
  KDCoordinate pixels = axis == Axis::Horizontal ? p : pixelLength(axis)-p;
  return min(axis) + pixels*(max(axis)-min(axis))/pixelLength(axis);
}

KDCoordinate GraphView::floatToPixel(Axis axis, float f) const {
  float fraction = (f-min(axis))/(max(axis)-min(axis));
  fraction = axis == Axis::Horizontal ? fraction : 1.0f - fraction;
  /* KDCoordinate does not support as big number as float. We thus cap the
   * fraction before casting. */
  fraction = fraction > 2 ? 2.0f : fraction;
  fraction = fraction < -1 ? -1.0f : fraction;
  return pixelLength(axis)*fraction;
}

void GraphView::drawFunction(KDContext * ctx, KDRect rect) const {
  KDColor workingBuffer[k_stampSize*k_stampSize];
  for (int i=0; i<m_functionStore->numberOfActiveFunctions(); i++) {
    Function * f = m_functionStore->activeFunctionAtIndex(i);
    float y = 0.0f;
    float x = 0.0f;
    for (KDCoordinate px = rect.x()-k_stampSize; px<rect.right(); px++) {
      float previousX = x;
      x = pixelToFloat(Axis::Horizontal, px);
      float previousY = y;
      y = f->evaluateAtAbscissa(x, m_evaluateContext);
      KDCoordinate py = floatToPixel(Axis::Vertical, y);
      stampAtLocation(px, py, f->color(), ctx, workingBuffer);
      if (px > rect.x()-k_stampSize) {
        jointDots(previousX, previousY, x, y, f, k_maxNumberOfIterations, ctx, workingBuffer);
      }
    }
  }
}

void GraphView::stampAtLocation(KDCoordinate px, KDCoordinate py, KDColor color, KDContext * ctx, KDColor * workingBuffer) const {
  // We avoid drawing when no part of the stamp is visible
  if (px < -k_stampSize || px > pixelLength(Axis::Horizontal)+k_stampSize || py < -k_stampSize || py > pixelLength(Axis::Vertical)+k_stampSize) {
    return;
  }
  KDRect stampRect(px-k_stampSize/2, py-k_stampSize/2, k_stampSize, k_stampSize);
  ctx->blendRectWithMask(stampRect, color, k_mask, workingBuffer);
}

void GraphView::jointDots(float x, float y, float u, float v, Function * function, int maxNumberOfRecursion, KDContext * ctx, KDColor * workingBuffer) const {
  KDCoordinate px = floatToPixel(Axis::Horizontal, x);
  KDCoordinate pu = floatToPixel(Axis::Horizontal, u);
  KDCoordinate py = floatToPixel(Axis::Vertical, y);
  KDCoordinate pv = floatToPixel(Axis::Vertical, v);
  if (py - k_stampSize/2 < pv && pv < py + k_stampSize/2) {
    // the dots are already joined
    return;
  }
  float middleX = (x + u)/2;
  float middleY = function->evaluateAtAbscissa(middleX, m_evaluateContext);
  KDCoordinate pMiddleX = floatToPixel(Axis::Horizontal, middleX);
  KDCoordinate pMiddleY = floatToPixel(Axis::Vertical, middleY);
  if ((y < middleY && middleY < v) || (v < middleY && middleY < u)) {
    /* As the middle dot is vertically between the two dots, we assume that we
     * can draw a 'straight' line between the two */
    straightJoinDots(px, py, pu, pv, function, ctx, workingBuffer);
    return;
  }
  if (maxNumberOfRecursion > 0) {
    stampAtLocation(pMiddleX, pMiddleY, function->color(), ctx, workingBuffer);
    jointDots(x, y, middleX, middleY, function, maxNumberOfRecursion-1, ctx, workingBuffer);
    jointDots(middleX, middleY, u, v, function, maxNumberOfRecursion-1, ctx, workingBuffer);
  }
}

void GraphView::straightJoinDots(KDCoordinate px, KDCoordinate py, KDCoordinate pu, KDCoordinate pv, Function * function, KDContext * ctx, KDColor * workingBuffer) const {
  if (py < pv) {
    KDCoordinate pm = px;
    KDCoordinate pVerticalMiddle = (py + pv)/2;
    for (KDCoordinate pn = py; pn < pv; pn++) {
      if (pn == pVerticalMiddle) {
        pm = pu;
      }
      stampAtLocation(pm, pn, function->color(), ctx, workingBuffer);
    }
    return;
  }
  straightJoinDots(pu, pv, px, py, function, ctx, workingBuffer);
}

#if 0

void GraphView::drawFunction(KDRect rect) const {
  /* Naive algorithm: iterate on pixels horizontally
   * Actually, this is kinda optimal. If two consecutive pixels don't touch
   * (i.e. they have a y-difference greater than 1), just draw a vertical line.
   * This may seem stupid but is kinda good actually. */
  KDCoordinate previousPixelY;
  KDCoordinate pixelX;

  KDColor curveColor = KDColorRGB(0xFF, 0, 0);
#define ANTI_ALIASING
#ifdef ANTI_ALIASING
  KDColor antiAliasingColor = KDColorRGB(0, 0xFF, 0);
#endif

  for (pixelX=rect.x; pixelX<(rect.x+rect.width); pixelX++) {
    float x = pixelToFloat(Axis::Horizontal, pixelX);
    float y = (x-1)*(x+1)*x;
    KDCoordinate pixelY = floatToPixel(Axis::Vertical, y);
    KDRect r;
    r.x = pixelX;
    if (pixelY < previousPixelY) {
      r.y = pixelY;
      r.height = previousPixelY-pixelY;
      KDPoint p;
      p.x = pixelX-1;
      p.y = previousPixelY+3;
      KDSetPixel(p, KDColorRGB(0x00, 0xFF, 0x00));
      p.x = pixelX;
      p.y = pixelY-1;
      KDSetPixel(p, KDColorRGB(0x00, 0xFF, 0x00));
    } else {
      r.y = previousPixelY;
      r.height = pixelY - previousPixelY;
    }
    if (r.height == 0) {
      r.height = 1;
    }
    r.width = 1;

    r.width += 2;
    r.height += 2;
    //KDFillRectWithPattern(r, antialiasedDot);
    KDFillRect(r, KDColorRGB(0xFF, 0, 0));
    previousPixelY = pixelY;
  }
}
#endif

}
