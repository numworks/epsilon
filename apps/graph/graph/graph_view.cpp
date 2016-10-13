#include "graph_view.h"
#include <assert.h>

constexpr KDColor kAxisColor = KDColor(0x000000);
constexpr KDColor kMainGridColor = KDColor(0xCCCCCC);
constexpr KDColor kSecondaryGridColor = KDColor(0xEEEEEE);
constexpr int kNumberOfMainGridLines = 5;
constexpr int kNumberOfSecondaryGridLines = 4;

GraphView::GraphView(Graph::FunctionStore * functionStore, Graph::EvaluateContext * evaluateContext) :
#if GRAPH_VIEW_IS_TILED
  TiledView(),
#else
  View(),
#endif
  m_cursorView(CursorView()),
  m_cursorPosition(KDPointZero),
  m_xMin(-2.0f),
  m_xMax(2.0f),
  m_yMin(-2.0f),
  m_yMax(2.0f),
  m_functionStore(functionStore),
  m_evaluateContext(evaluateContext)
{
}

int GraphView::numberOfSubviews() const {
  return 1;
};

View * GraphView::subviewAtIndex(int index) {
  return &m_cursorView;
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
  return (axis == Axis::Horizontal ? m_xMin : m_yMin);
}

float GraphView::max(Axis axis) const {
  assert(axis == Axis::Horizontal || axis == Axis::Vertical);
  return (axis == Axis::Horizontal ? m_xMax : m_yMax);
}

KDCoordinate GraphView::pixelLength(Axis axis) const {
  assert(axis == Axis::Horizontal || axis == Axis::Vertical);
  return (axis == Axis::Horizontal ? m_frame.width() : m_frame.height());
}

float GraphView::pixelToFloat(Axis axis, KDCoordinate p) const {
  return min(axis) + p*(max(axis)-min(axis))/pixelLength(axis);
}

KDCoordinate GraphView::floatToPixel(Axis axis, float f) const {
  return pixelLength(axis)*(max(axis)-f)/(max(axis)-min(axis));
}

void GraphView::drawFunction(KDContext * ctx, KDRect rect) const {

  constexpr KDCoordinate stampSize = 5;

  uint8_t mask[stampSize*stampSize] = {
    0x00, 0x30, 0x73, 0x30, 0x00,
    0x30, 0xfb, 0xff, 0xfb, 0x30,
    0x73, 0xff, 0xff, 0xff, 0x73,
    0x30, 0xfb, 0xff, 0xfb, 0x30,
    0x00, 0x30, 0x73, 0x30, 0x00
  };

  KDColor workingBuffer[stampSize*stampSize];

  for (KDCoordinate px = rect.x()-stampSize; px<rect.right(); px++) {
    float x = pixelToFloat(Axis::Horizontal, px);
    for (int i=0; i<m_functionStore->numberOfFunctions(); i++) {
      Graph::Function * f = m_functionStore->functionAtIndex(i);
      if (f->isActive()) {
        float y = f->evaluateAtAbscissa(x, m_evaluateContext);
        KDCoordinate py = floatToPixel(Axis::Vertical, y);
        KDRect stampRect(px, py, stampSize, stampSize);
        ctx->fillRectWithMask(stampRect, f->color(), mask, workingBuffer);
      }
    }
  }
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
