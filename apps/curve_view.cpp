#include "curve_view.h"
#include "constant.h"
#include <assert.h>
#include <math.h>
#include <float.h>
#include <string.h>

constexpr KDColor CurveView::k_axisColor;
constexpr KDColor CurveView::k_gridColor;

CurveView::CurveView(CurveViewRange * curveViewRange, CurveViewCursor * curveViewCursor, View * bannerView,
    View * cursorView, float topMarginFactor, float rightMarginFactor, float bottomMarginFactor,
    float leftMarginFactor) :
  View(),
  m_curveViewRange(curveViewRange),
  m_curveViewCursor(curveViewCursor),
  m_bannerView(bannerView),
  m_cursorView(cursorView),
  m_topMarginFactor(topMarginFactor),
  m_bottomMarginFactor(bottomMarginFactor),
  m_leftMarginFactor(leftMarginFactor),
  m_rightMarginFactor(rightMarginFactor)
{
}

void CurveView::reload() {
  markRectAsDirty(bounds());
  if (label(Axis::Horizontal, 0) != nullptr) {
    computeLabels(Axis::Horizontal);
  }
  if (label(Axis::Vertical, 0) != nullptr) {
    computeLabels(Axis::Vertical);
  }
  layoutSubviews();
}

void CurveView::reloadSelection() {
  if (m_curveViewCursor != nullptr) {
    float pixelXSelection = roundf(floatToPixel(Axis::Horizontal, m_curveViewCursor->x()));
    float pixelYSelection = roundf(floatToPixel(Axis::Vertical, m_curveViewCursor->y()));
    KDRect dirtyZone(KDRect(pixelXSelection - k_cursorSize/2, pixelYSelection - k_cursorSize/2, k_cursorSize, k_cursorSize));
    markRectAsDirty(dirtyZone);
    layoutSubviews();
  }
}

bool CurveView::isMainViewSelected() const {
  return m_mainViewSelected;
}

void CurveView::selectMainView(bool mainViewSelected) {
  if (m_mainViewSelected != mainViewSelected) {
    m_mainViewSelected = mainViewSelected;
    reloadSelection();
    layoutSubviews();
  }
}

void CurveView::setCurveViewRange(CurveViewRange * curveViewRange) {
  m_curveViewRange = curveViewRange;
}

float CurveView::min(Axis axis) const {
  assert(axis == Axis::Horizontal || axis == Axis::Vertical);
  float range = axis == Axis::Horizontal ? m_curveViewRange->xMax() - m_curveViewRange->xMin() : m_curveViewRange->yMax() - m_curveViewRange->yMin();
  float absoluteMin = axis == Axis::Horizontal ? m_curveViewRange->xMin(): m_curveViewRange->yMin();
  float marginFactor = axis == Axis::Horizontal ? m_leftMarginFactor : m_bottomMarginFactor;
  return absoluteMin - marginFactor*range;
}

float CurveView::max(Axis axis) const {
  assert(axis == Axis::Horizontal || axis == Axis::Vertical);
  float range = axis == Axis::Horizontal ? m_curveViewRange->xMax() - m_curveViewRange->xMin() : m_curveViewRange->yMax() - m_curveViewRange->yMin();
  float absoluteMax = (axis == Axis::Horizontal ? m_curveViewRange->xMax() : m_curveViewRange->yMax());
  float marginFactor = axis == Axis::Horizontal ? m_rightMarginFactor : m_topMarginFactor;
  return absoluteMax + marginFactor*range;
}

float CurveView::gridUnit(Axis axis) const {
  return (axis == Axis::Horizontal ? m_curveViewRange->xGridUnit() : m_curveViewRange->yGridUnit());
}

KDCoordinate CurveView::pixelLength(Axis axis) const {
  assert(axis == Axis::Horizontal || axis == Axis::Vertical);
  return (axis == Axis::Horizontal ? m_frame.width() : m_frame.height());
}

float CurveView::pixelToFloat(Axis axis, KDCoordinate p) const {
  KDCoordinate pixels = axis == Axis::Horizontal ? p : pixelLength(axis)-p;
  return min(axis) + pixels*(max(axis)-min(axis))/pixelLength(axis);
}

float CurveView::floatToPixel(Axis axis, float f) const {
  float fraction = (f-min(axis))/(max(axis)-min(axis));
  fraction = axis == Axis::Horizontal ? fraction : 1.0f - fraction;
  return pixelLength(axis)*fraction;
}

void CurveView::computeLabels(Axis axis) {
  char buffer[Constant::FloatBufferSizeInDecimalMode];
  float step = gridUnit(axis);
  for (int index = 0; index < numberOfLabels(axis); index++) {
    // TODO: change the number of digits in mantissa once the numerical mode is implemented
    Float(2.0f*step*(ceilf(min(axis)/(2.0f*step)))+index*2.0f*step).convertFloatToText(buffer, Constant::FloatBufferSizeInDecimalMode, Constant::NumberOfDigitsInMantissaInDecimalMode, Float::DisplayMode::Decimal);
    //TODO: check for size of label?
    strlcpy(label(axis, index), buffer, strlen(buffer)+1);
  }
}

void CurveView::drawLabels(KDContext * ctx, KDRect rect, Axis axis, bool shiftOrigin) const {
  float step = gridUnit(axis);
  float start = 2.0f*step*(ceilf(min(axis)/(2.0f*step)));
  float end = max(axis);
  int i = 0;
  for (float x = start; x < end; x += 2.0f*step) {
    KDSize textSize = KDText::stringSize(label(axis, i));
    KDPoint origin(floatToPixel(Axis::Horizontal, x) - textSize.width()/2, floatToPixel(Axis::Vertical, 0.0f)  + k_labelMargin);
    if (axis == Axis::Vertical) {
      origin = KDPoint(floatToPixel(Axis::Horizontal, 0.0f) + k_labelMargin, floatToPixel(Axis::Vertical, x) - textSize.height()/2);
    }
    if (-step < x && x < step && shiftOrigin) {
      origin = KDPoint(floatToPixel(Axis::Horizontal, 0.0f) + k_labelMargin, floatToPixel(Axis::Vertical, 0.0f) + k_labelMargin);
    }
    if (rect.intersects(KDRect(origin, KDText::stringSize(label(axis, i))))) {
      ctx->blendString(label(axis, i), origin, KDColorBlack);
    }
    i++;
  }
}

void CurveView::drawLine(KDContext * ctx, KDRect rect, Axis axis, float coordinate, KDColor color, KDCoordinate thickness) const {
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
  if (rect.intersects(lineRect)) {
    ctx->fillRect(lineRect, color);
  }
}

void CurveView::drawSegment(KDContext * ctx, KDRect rect, Axis axis, float coordinate, float lowerBound, float upperBound, KDColor color, KDCoordinate thickness) const {
  KDRect lineRect = KDRectZero;
  switch(axis) {
    // WARNING TODO: anti-aliasing?
    case Axis::Horizontal:
      lineRect = KDRect(
          floorf(floatToPixel(Axis::Horizontal, lowerBound)), floatToPixel(Axis::Vertical, coordinate),
          ceilf(floatToPixel(Axis::Horizontal, upperBound) - floatToPixel(Axis::Horizontal, lowerBound)), thickness
          );
      break;
    case Axis::Vertical:
      lineRect = KDRect(
          floatToPixel(Axis::Horizontal, coordinate), floorf(floatToPixel(Axis::Vertical, upperBound)),
          thickness,  ceilf(floatToPixel(Axis::Vertical, lowerBound) - floatToPixel(Axis::Vertical, upperBound))
      );
      break;
  }
  if (rect.intersects(lineRect)) {
    ctx->fillRect(lineRect, color);
  }
}

void CurveView::drawDot(KDContext * ctx, KDRect rect, float x, float y, KDColor color, KDSize size) const {
  KDCoordinate px = roundf(floatToPixel(Axis::Horizontal, x));
  KDCoordinate py = roundf(floatToPixel(Axis::Vertical, y));
  if ((px + size.width() < rect.left() - k_externRectMargin || px - size.width() > rect.right() + k_externRectMargin) &&
      (py + size.height() < rect.top() - k_externRectMargin || py - size.height() > rect.bottom() + k_externRectMargin)) {
    return;
  }
  KDRect dotRect = KDRect(px - size.width()/2, py-size.height()/2, size.width(), size.height());
  ctx->fillRect(dotRect, color);
}

void CurveView::drawGridLines(KDContext * ctx, KDRect rect, Axis axis, float step, KDColor color) const {
  float rectMin = pixelToFloat(Axis::Horizontal, rect.left());
  float rectMax = pixelToFloat(Axis::Horizontal, rect.right());
  if (axis == Axis::Vertical) {
    rectMax = pixelToFloat(Axis::Vertical, rect.top());
    rectMin = pixelToFloat(Axis::Vertical, rect.bottom());
  }
  float start = step*((int)(min(axis)/step));
  Axis otherAxis = (axis == Axis::Horizontal) ? Axis::Vertical : Axis::Horizontal;
  for (float x =start; x < max(axis); x += step) {
    if (rectMin <= x && x <= rectMax) {
      drawLine(ctx, rect, otherAxis, x, color);
    }
  }
}

void CurveView::drawGrid(KDContext * ctx, KDRect rect) const {
  drawGridLines(ctx, rect, Axis::Horizontal, m_curveViewRange->xGridUnit(), k_gridColor);
  drawGridLines(ctx, rect, Axis::Vertical, m_curveViewRange->yGridUnit(), k_gridColor);
}

void CurveView::drawAxes(KDContext * ctx, KDRect rect, Axis axis) const {
  drawLine(ctx, rect, axis, 0.0f, k_axisColor, 2);
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
constexpr static int k_resolution = 320.0f;

void CurveView::drawCurve(KDContext * ctx, KDRect rect, Model * curve, KDColor color, bool colorUnderCurve, float colorLowerBound, float colorUpperBound, bool continuously) const {
  float xMin = min(Axis::Horizontal);
  float xMax = max(Axis::Horizontal);
  float xStep = (xMax-xMin)/k_resolution;
  float rectMin = pixelToFloat(Axis::Horizontal, rect.left() - k_externRectMargin);
  float rectMax = pixelToFloat(Axis::Horizontal, rect.right() + k_externRectMargin);
  for (float x = rectMin; x < rectMax; x += xStep) {
    float y = evaluateModelWithParameter(curve, x);
    if (isnan(y)) {
      continue;
    }
    float pxf = floatToPixel(Axis::Horizontal, x);
    float pyf = floatToPixel(Axis::Vertical, y);
    if (colorUnderCurve && x > colorLowerBound && x < colorUpperBound) {
      KDRect colorRect((int)pxf, roundf(pyf), 1, floatToPixel(Axis::Vertical, 0.0f) - roundf(pyf));
      if (floatToPixel(Axis::Vertical, 0.0f) < roundf(pyf)) {
        colorRect = KDRect((int)pxf, floatToPixel(Axis::Vertical, 0.0f), 2, roundf(pyf) - floatToPixel(Axis::Vertical, 0.0f));
      }
      ctx->fillRect(colorRect, color);
    }
    stampAtLocation(ctx, rect, pxf, pyf, color);
    if (x <= rectMin || isnan(evaluateModelWithParameter(curve, x-xStep))) {
      continue;
    }
    if (continuously) {
      float puf = floatToPixel(Axis::Horizontal, x - xStep);
      float pvf = floatToPixel(Axis::Vertical, evaluateModelWithParameter(curve, x-xStep));
      straightJoinDots(ctx, rect, puf, pvf, pxf, pyf, color);
    } else {
      jointDots(ctx, rect, curve, x - xStep, evaluateModelWithParameter(curve, x-xStep), x, y, color, k_maxNumberOfIterations);
    }
  }
}

void CurveView::drawHistogram(KDContext * ctx, KDRect rect, Model * model, float firstBarAbscissa, float barWidth,
    bool fillBar, KDColor defaultColor, KDColor highlightColor,  float highlightLowerBound, float highlightUpperBound) const {
  KDCoordinate pixelBarWidth = fillBar ? floatToPixel(Axis::Horizontal, barWidth) - floatToPixel(Axis::Horizontal, 0.0f) : 2;
  float rectMin = pixelToFloat(Axis::Horizontal, rect.left());
  int rectMinBinNumber = floorf((rectMin - firstBarAbscissa)/barWidth);
  float rectMinLowerBound = firstBarAbscissa + rectMinBinNumber*barWidth;
  float rectMax = pixelToFloat(Axis::Horizontal, rect.right());
  int rectMaxBinNumber = floorf((rectMax - firstBarAbscissa)/barWidth);
  float rectMaxUpperBound = firstBarAbscissa + (rectMaxBinNumber+1)*barWidth + barWidth;
  for (float x = rectMinLowerBound; x < rectMaxUpperBound; x += barWidth) {
    float y = evaluateModelWithParameter(model, x+barWidth/2.0f);
    if (isnan(y)) {
      continue;
    }
    float pxf = floatToPixel(Axis::Horizontal, x);
    float pyf = floatToPixel(Axis::Vertical, y);
    KDRect binRect(pxf, roundf(pyf), pixelBarWidth+1 , floatToPixel(Axis::Vertical, 0.0f) - roundf(pyf));
    if (floatToPixel(Axis::Vertical, 0.0f) < roundf(pyf)) {
      binRect = KDRect(pxf, floatToPixel(Axis::Vertical, 0.0f), pixelBarWidth+1, roundf(pyf) - floatToPixel(Axis::Vertical, 0.0f));
    }
    KDColor binColor = defaultColor;
    if (x + barWidth/2.0f >= highlightLowerBound && x + barWidth/2.0f <= highlightUpperBound) {
      binColor = highlightColor;
    }
    ctx->fillRect(binRect, binColor);
  }
}

int CurveView::numberOfLabels(Axis axis) const {
  Axis otherAxis = axis == Axis::Horizontal ? Axis::Vertical : Axis::Horizontal;
  if (min(otherAxis) > 0.0f || max(otherAxis) < 0.0f) {
    return 0;
  }
  return ceilf((max(axis) - min(axis))/(2*gridUnit(axis)));
}

float CurveView::evaluateModelWithParameter(Model * curve, float t) const {
  return 0.0f;
}

void CurveView::jointDots(KDContext * ctx, KDRect rect, Model * curve, float x, float y, float u, float v, KDColor color, int maxNumberOfRecursion) const {
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
  float cy = evaluateModelWithParameter(curve, cx);
  if ((y < cy && cy < v) || (v < cy && cy < y)) {
    /* As the middle dot is vertically between the two dots, we assume that we
     * can draw a 'straight' line between the two */
    float pxf = floatToPixel(Axis::Horizontal, x);
    float puf = floatToPixel(Axis::Horizontal, u);
    straightJoinDots(ctx, rect, pxf, pyf, puf, pvf, color);
    return;
  }
  float pcxf = floatToPixel(Axis::Horizontal, cx);
  float pcyf = floatToPixel(Axis::Vertical, cy);
  if (maxNumberOfRecursion > 0) {
    stampAtLocation(ctx, rect, pcxf, pcyf, color);
    jointDots(ctx, rect, curve, x, y, cx, cy, color, maxNumberOfRecursion-1);
    jointDots(ctx, rect, curve, cx, cy, u, v, color, maxNumberOfRecursion-1);
  }
}

void CurveView::straightJoinDots(KDContext * ctx, KDRect rect, float pxf, float pyf, float puf, float pvf, KDColor color) const {
  if (pyf <= pvf) {
    for (float pnf = pyf; pnf<pvf; pnf+= 1.0f) {
      float pmf = pxf + (pnf - pyf)*(puf - pxf)/(pvf - pyf);
      stampAtLocation(ctx, rect, pmf, pnf, color);
    }
    return;
  }
  straightJoinDots(ctx, rect, puf, pvf, pxf, pyf, color);
}

void CurveView::stampAtLocation(KDContext * ctx, KDRect rect, float pxf, float pyf, KDColor color) const {
  // We avoid drawing when no part of the stamp is visible
  if (pyf < -stampSize || pyf > pixelLength(Axis::Vertical)+stampSize) {
    return;
  }
  KDCoordinate px = pxf;
  KDCoordinate py = pyf;
  KDRect stampRect(px-circleDiameter/2, py-circleDiameter/2, stampSize, stampSize);
  if (!rect.intersects(stampRect)) {
    return;
  }
  uint8_t shiftedMask[stampSize][stampSize];
  KDColor workingBuffer[stampSize*stampSize];
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
  ctx->blendRectWithMask(stampRect, color, (const uint8_t *)shiftedMask, workingBuffer);
}

void CurveView::layoutSubviews() {
  if (m_curveViewCursor != nullptr && m_cursorView != nullptr) {
    KDCoordinate xCursorPixelPosition = roundf(floatToPixel(Axis::Horizontal, m_curveViewCursor->x()));
    KDCoordinate yCursorPixelPosition = roundf(floatToPixel(Axis::Vertical, m_curveViewCursor->y()));
    KDRect cursorFrame(xCursorPixelPosition - k_cursorSize/2, yCursorPixelPosition - k_cursorSize/2, k_cursorSize, k_cursorSize);
    if (!m_mainViewSelected) {
      cursorFrame = KDRectZero;
    }
    m_cursorView->setFrame(cursorFrame);
  }
  if (m_bannerView != nullptr) {
    m_bannerView->setFrame(bounds());
    KDCoordinate bannerHeight = m_bannerView->minimalSizeForOptimalDisplay().height();
    KDRect bannerFrame(KDRect(0, bounds().height()- bannerHeight, bounds().width(), bannerHeight));
    if (!m_mainViewSelected) {
      bannerFrame = KDRectZero;
    }
    m_bannerView->setFrame(bannerFrame);
  }
}

int CurveView::numberOfSubviews() const {
  return (m_bannerView != nullptr) + (m_cursorView != nullptr);
};

View * CurveView::subviewAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return m_bannerView;
  }
  return m_cursorView;
}
