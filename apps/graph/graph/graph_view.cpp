#include "graph_view.h"
#include "../../constant.h"
#include <assert.h>
#include <math.h>
#include <float.h>

namespace Graph {

constexpr KDColor GraphView::k_axisColor;
constexpr KDColor GraphView::k_gridColor;

GraphView::GraphView(FunctionStore * functionStore, AxisInterval * axisInterval) :
#if GRAPH_VIEW_IS_TILED
  TiledView(),
#else
  View(),
#endif
  m_cursorView(CursorView()),
  m_xCursorPosition(0.0f),
  m_yCursorPosition(0.0f),
  m_axisInterval(axisInterval),
  m_functionStore(functionStore),
  m_evaluateContext(nullptr)
{
}

int GraphView::numberOfSubviews() const {
  return 1 + numberOfXLabels() + numberOfYLabels();
};

View * GraphView::subviewAtIndex(int index) {
  if (index < numberOfXLabels()) {
    float step = m_axisInterval->xScale();
    char buffer[Constant::FloatBufferSizeInScientificMode];
    // TODO: change the number of digits in mantissa once the numerical mode is implemented
    Float(2.0f*step*(ceilf(min(Axis::Horizontal)/(2.0f*step)))+index*2.0f*step).convertFloatToText(buffer, Constant::FloatBufferSizeInScientificMode, Constant::NumberOfDigitsInMantissaInScientificMode);
    m_xLabels[index].setText(buffer);
    return &m_xLabels[index];
  }
  if (index < numberOfXLabels() + numberOfYLabels()) {
    float step = m_axisInterval->yScale();
    char buffer[Constant::FloatBufferSizeInScientificMode];
    int newIndex = index - numberOfXLabels();
    Float(2.0f*step*(ceilf(min(Axis::Vertical)/(2.0f*step)))+newIndex*2.0f*step).convertFloatToText(buffer, Constant::FloatBufferSizeInScientificMode, Constant::NumberOfDigitsInMantissaInScientificMode);
    m_yLabels[newIndex].setText(buffer);
    return &m_yLabels[newIndex];
  }
  if (index == numberOfXLabels() + numberOfYLabels()) {
    return &m_cursorView;
  }
  assert(false);
}

void GraphView::setContext(Context * context) {
  m_evaluateContext = (EvaluateContext *)context;
}

Context * GraphView::context() const {
  return m_evaluateContext;
}

void GraphView::reload() {
  markRectAsDirty(bounds());
  initCursorPosition();
  layoutSubviews();
}

int GraphView::numberOfXLabels() const {
  if (min(Axis::Vertical) > 0.0f || max(Axis::Vertical) < 0.0f) {
    return 0;
  }
  return ceilf((max(Axis::Horizontal) - min(Axis::Horizontal))/(2*m_axisInterval->xScale()));
}

int GraphView::numberOfYLabels() const {
  if (min(Axis::Horizontal) > 0.0f || max(Axis::Horizontal) < 0.0f) {
    return 0;
  }
  return ceilf((max(Axis::Vertical) - min(Axis::Vertical))/(2*m_axisInterval->yScale()));
}

void GraphView::initCursorPosition() {
  float center = (min(Axis::Horizontal)+max(Axis::Horizontal))/2.0f;
  m_indexFunctionSelectedByCursor = 0;
  Function * firstFunction = m_functionStore->activeFunctionAtIndex(0);
  float fCenter = firstFunction->evaluateAtAbscissa(center, m_evaluateContext);
  m_xCursorPosition = (bounds().width()-1.0f)/2.0f;
  m_yCursorPosition = floatToPixel(Axis::Vertical, fCenter);
}

void GraphView::moveCursorHorizontally(KDCoordinate xOffset) {
  m_xCursorPosition = m_xCursorPosition + xOffset;
  Function * f = m_functionStore->activeFunctionAtIndex(m_indexFunctionSelectedByCursor);
  float ordinate = f->evaluateAtAbscissa(pixelToFloat(Axis::Horizontal, m_xCursorPosition), m_evaluateContext);
  m_yCursorPosition = floatToPixel(Axis::Vertical, ordinate);
  layoutSubviews();
}

Function * GraphView::moveCursorUp() {
  float x = pixelToFloat(Axis::Horizontal, m_xCursorPosition);
  Function * actualFunction = m_functionStore->activeFunctionAtIndex(m_indexFunctionSelectedByCursor);
  float y = actualFunction->evaluateAtAbscissa(x, m_evaluateContext);
  Function * nextFunction = actualFunction;
  float nextY = FLT_MAX;
  for (int i = 0; i < m_functionStore->numberOfActiveFunctions(); i++) {
    Function * f = m_functionStore->activeFunctionAtIndex(i);
    float newY = f->evaluateAtAbscissa(x, m_evaluateContext);
    if (newY > y && newY < nextY) {
      m_indexFunctionSelectedByCursor = i;
      nextY = newY;
      nextFunction = f;
    }
  }
  if (nextFunction == actualFunction) {
    return nullptr;
  }
  m_yCursorPosition = floatToPixel(Axis::Vertical, nextY);
  layoutSubviews();
  return nextFunction;
}

Function * GraphView::moveCursorDown() {
  float x = pixelToFloat(Axis::Horizontal, m_xCursorPosition);
  Function * actualFunction = m_functionStore->activeFunctionAtIndex(m_indexFunctionSelectedByCursor);
  float y = actualFunction->evaluateAtAbscissa(x, m_evaluateContext);
  Function * nextFunction = actualFunction;
  float nextY = -FLT_MAX;
  for (int i = 0; i < m_functionStore->numberOfActiveFunctions(); i++) {
    Function * f = m_functionStore->activeFunctionAtIndex(i);
    float newY = f->evaluateAtAbscissa(x, m_evaluateContext);
    if (newY < y && newY > nextY) {
      m_indexFunctionSelectedByCursor = i;
      nextY = newY;
      nextFunction = f;
    }
  }
  if (nextFunction == actualFunction) {
    return nullptr;
  }
  m_yCursorPosition = floatToPixel(Axis::Vertical, nextY);
  layoutSubviews();
  return nextFunction;
}

void GraphView::layoutSubviews() {
  KDRect cursorFrame((int)m_xCursorPosition - (k_cursorSize+1)/2+1, (int)m_yCursorPosition - (k_cursorSize+1)/2+1, k_cursorSize, k_cursorSize);
  m_cursorView.setPosition(m_xCursorPosition, m_yCursorPosition);
  m_cursorView.setFrame(cursorFrame);
  float step = m_axisInterval->xScale();
  float start = 2.0f*step*(ceilf(min(Axis::Horizontal)/(2.0f*step)));
  int i = 0;
  for (float x = start; x < max(Axis::Horizontal); x += 2.0f*step) {
    KDRect labelFrame(floatToPixel(Axis::Horizontal, x) - k_labelWidth/2, floatToPixel(Axis::Vertical, 0.0f) + k_labelMargin, k_labelWidth, k_labelHeight);
    // TODO: Find another way to avoid float comparison.
    if (x == 0.0f) {
      labelFrame = KDRect(floatToPixel(Axis::Horizontal, 0.0f) + k_labelMargin, floatToPixel(Axis::Vertical, 0.0f) + k_labelMargin, k_labelWidth, k_labelHeight);
    }
    m_xLabels[i++].setFrame(labelFrame);
  }
  for (int k = i; k < k_maxNumberOfXLabels; k++) {
    m_xLabels[k].setFrame(KDRectZero);
  }
  step = m_axisInterval->yScale();
  start = 2.0f*step*(ceilf(min(Axis::Vertical)/(2.0f*step)));
  int j = 0;
  for (float y = start; y < max(Axis::Vertical); y += 2.0f*step) {
    KDRect labelFrame(floatToPixel(Axis::Horizontal, 0.0f) + k_labelMargin, floatToPixel(Axis::Vertical, y) - k_labelHeight/2, k_labelWidth, k_labelHeight);
    if (y == 0.0f) {
      labelFrame = KDRectZero;
    }
    m_yLabels[j++].setFrame(labelFrame);
  }
  for (int k = j; k < k_maxNumberOfYLabels; k++) {
    m_yLabels[k].setFrame(KDRectZero);
  }
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
    // WARNING TODO: anti-aliasing?
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
  drawLine(ctx, rect, Axis::Horizontal, 0.0f, k_axisColor, 2);
  drawLine(ctx, rect, Axis::Vertical, 0.0f, k_axisColor, 2);
}

void GraphView::drawGridLines(KDContext * ctx, KDRect rect, Axis axis, float step, KDColor color) const {
  float start = step*((int)(min(axis)/step));
  Axis otherAxis = (axis == Axis::Horizontal) ? Axis::Vertical : Axis::Horizontal;
  for (float x =start; x < max(axis); x += step) {
    drawLine(ctx, rect, otherAxis, x, color);
  }
}

void GraphView::drawGrid(KDContext * ctx, KDRect rect) const {
  drawGridLines(ctx, rect, Axis::Horizontal, m_axisInterval->xScale(), k_gridColor);
  drawGridLines(ctx, rect, Axis::Vertical, m_axisInterval->yScale(), k_gridColor);
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

float GraphView::floatToPixel(Axis axis, float f) const {
  float fraction = (f-min(axis))/(max(axis)-min(axis));
  fraction = axis == Axis::Horizontal ? fraction : 1.0f - fraction;
  return pixelLength(axis)*fraction;
}

#define LINE_THICKNESS 3

#if LINE_THICKNESS == 3

constexpr KDCoordinate circleDiameter = 3;
constexpr KDCoordinate stampSize = circleDiameter+1;
const uint8_t stampMask[stampSize+1][stampSize+1] = {
  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
  {0xFF, 0x7A, 0x0C, 0x7A, 0xFF},
  {0xFF, 0x0C, 0x00, 0x0C, 0xFF},
  {0xFF, 0x7A, 0x0C, 0x7A, 0xFF},
  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
};

#elif LINE_THICKNESS == 5

constexpr KDCoordinate circleDiameter = 5;
constexpr KDCoordinate stampSize = circleDiameter+1;
const uint8_t stampMask[stampSize+1][stampSize+1] = {
  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
  {0xFF, 0xE1, 0x45, 0x0C, 0x45, 0xE1, 0xFF},
  {0xFF, 0x45, 0x00, 0x00, 0x00, 0x45, 0xFF},
  {0xFF, 0x0C, 0x00, 0x00, 0x00, 0x0C, 0xFF},
  {0xFF, 0x45, 0x00, 0x00, 0x00, 0x45, 0xFF},
  {0xFF, 0xE1, 0x45, 0x0C, 0x45, 0xE1, 0xFF},
  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
};

#endif

constexpr static int k_maxNumberOfIterations = 10;

void GraphView::drawFunction(KDContext * ctx, KDRect rect) const {
  float xMin = m_axisInterval->xMin();
  float xMax = m_axisInterval->xMax();
  float xStep = (xMax-xMin)/320.0f;
  for (int i = 0; i < m_functionStore->numberOfActiveFunctions(); i++) {
    Function * f = m_functionStore->activeFunctionAtIndex(i);
    for (float x = xMin; x < xMax; x += xStep) {
      float y = f->evaluateAtAbscissa(x, m_evaluateContext);
      float pxf = floatToPixel(Axis::Horizontal, x);
      float pyf = floatToPixel(Axis::Vertical, y);
      stampAtLocation(pxf, pyf, f->color(), ctx);
      if (x > xMin) {
        jointDots(x - xStep, f->evaluateAtAbscissa(x-xStep, m_evaluateContext), x, y, f, k_maxNumberOfIterations, ctx);
      }
    }
  }
}

void GraphView::stampAtLocation(float pxf, float pyf, KDColor color, KDContext * ctx) const {
  // We avoid drawing when no part of the stamp is visible
  if (pyf < -stampSize || pyf > pixelLength(Axis::Vertical)+stampSize) {
    return;
  }
  uint8_t shiftedMask[stampSize][stampSize];
  KDColor workingBuffer[stampSize*stampSize];
  KDCoordinate px = pxf;
  KDCoordinate py = pyf;
  float dx = pxf - floorf(pxf);
  float dy = pyf - floorf(pyf);
  /* TODO: this could be optimized by precomputing 10 or 100 shifted masks. The
   * dx and dy would be rounded to one tenth or one hundredth to choose the
   * right shifted mask. */
  for (int i=0; i<stampSize; i++) {
    for (int j=0; j<stampSize; j++) {
      shiftedMask[i][j] = dx * (stampMask[i][j]*dy+stampMask[i+1][j]*(1.0f-dy))
        + (1.0f-dx) * (stampMask[i][j+1]*dy + stampMask[i+1][j+1]*(1.0f-dy));
    }
  }
  KDRect stampRect(px-circleDiameter/2, py-circleDiameter/2, stampSize, stampSize);
  ctx->blendRectWithMask(stampRect, color, (const uint8_t *)shiftedMask, workingBuffer);
}

void GraphView::jointDots(float x, float y, float u, float v, Function * function, int maxNumberOfRecursion, KDContext * ctx) const {
  float pyf = floatToPixel(Axis::Vertical, y);
  float pvf = floatToPixel(Axis::Vertical, v);
  // No need to draw if both dots are outside visible area
  if ((pyf < -stampSize && pvf < -stampSize) || (pyf > pixelLength(Axis::Vertical)+stampSize && pvf > pixelLength(Axis::Vertical)+stampSize)) {
    return;
  }
  // If one of the dot is infinite, we cap it with a dot outside area
  if (isinf(pyf)) {
    pyf = pyf > 0 ? pixelLength(Axis::Vertical)+stampSize : -stampSize;
  }
  if (isinf(pvf)) {
    pvf = pvf > 0 ? pixelLength(Axis::Vertical)+stampSize : -stampSize;
  }
  if (pyf - (float)circleDiameter/2.0f < pvf && pvf < pyf + (float)circleDiameter/2.0f) {
    // the dots are already joined
    return;
  }
  // C is the dot whose abscissa is between x and u
  float cx = (x + u)/2.0f;
  float cy = function->evaluateAtAbscissa(cx, m_evaluateContext);
  if ((y < cy && cy < v) || (v < cy && cy < y)) {
    /* As the middle dot is vertically between the two dots, we assume that we
     * can draw a 'straight' line between the two */
    float pxf = floatToPixel(Axis::Horizontal, x);
    float puf = floatToPixel(Axis::Horizontal, u);
    straightJoinDots(pxf, pyf, puf, pvf, function->color(), ctx);
    return;
  }
  float pcxf = floatToPixel(Axis::Horizontal, cx);
  float pcyf = floatToPixel(Axis::Vertical, cy);
  if (maxNumberOfRecursion > 0) {
    stampAtLocation(pcxf, pcyf, function->color(), ctx);
    jointDots(x, y, cx, cy, function, maxNumberOfRecursion-1, ctx);
    jointDots(cx, cy, u, v, function, maxNumberOfRecursion-1, ctx);
  }
}

void GraphView::straightJoinDots(float pxf, float pyf, float puf, float pvf, KDColor color, KDContext * ctx) const {
  if (pyf < pvf) {
    for (float pnf = pyf; pnf<pvf; pnf+= 1.0f) {
      float pmf = pxf + (pnf - pyf)*(puf - pxf)/(pvf - pyf);
      stampAtLocation(pmf, pnf, color, ctx);
    }
    return;
  }
  straightJoinDots(puf, pvf, pxf, pyf, color, ctx);
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
