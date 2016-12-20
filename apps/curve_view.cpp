#include "curve_view.h"
#include "constant.h"
#include <assert.h>
#include <math.h>
#include <float.h>
#include <string.h>

constexpr KDColor CurveView::k_axisColor;

CurveView::CurveView(CurveViewWindow * curveViewWindow) :
  View(),
  m_curveViewWindow(curveViewWindow)
{
}

void CurveView::setCurveViewWindow(CurveViewWindow * curveViewWindow) {
  m_curveViewWindow = curveViewWindow;
}

void CurveView::reload() {
  markRectAsDirty(bounds());
}

float CurveView::min(Axis axis) const {
  assert(axis == Axis::Horizontal || axis == Axis::Vertical);
  float range = axis == Axis::Horizontal ? m_curveViewWindow->xMax() - m_curveViewWindow->xMin() : m_curveViewWindow->yMax() - m_curveViewWindow->yMin();
  float absoluteMin = (axis == Axis::Horizontal ? m_curveViewWindow->xMin(): m_curveViewWindow->yMin());
  return absoluteMin - k_marginFactor*range;
}

float CurveView::max(Axis axis) const {
  assert(axis == Axis::Horizontal || axis == Axis::Vertical);
  float range = axis == Axis::Horizontal ? m_curveViewWindow->xMax() - m_curveViewWindow->xMin() : m_curveViewWindow->yMax() - m_curveViewWindow->yMin();
  float absoluteMax = (axis == Axis::Horizontal ? m_curveViewWindow->xMax() : m_curveViewWindow->yMax());
  return absoluteMax + k_marginFactor*range;
}

float CurveView::gridUnit(Axis axis) const {
  return (axis == Axis::Horizontal ? m_curveViewWindow->xGridUnit() : m_curveViewWindow->yGridUnit());
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

int CurveView::numberOfLabels(Axis axis) const {
  Axis otherAxis = axis == Axis::Horizontal ? Axis::Vertical : Axis::Horizontal;
  if (min(otherAxis) > 0.0f || max(otherAxis) < 0.0f) {
    return 0;
  }
  return ceilf((max(axis) - min(axis))/(2*gridUnit(axis)));
}

void CurveView::computeLabels(Axis axis) {
  char buffer[Constant::FloatBufferSizeInScientificMode];
  float step = gridUnit(axis);
  for (int index = 0; index < numberOfLabels(axis); index++) {
    // TODO: change the number of digits in mantissa once the numerical mode is implemented
    Float(2.0f*step*(ceilf(min(axis)/(2.0f*step)))+index*2.0f*step).convertFloatToText(buffer, Constant::FloatBufferSizeInScientificMode, Constant::NumberOfDigitsInMantissaInScientificMode);
    //TODO: check for size of label?
    strlcpy(label(axis, index), buffer, strlen(buffer)+1);
  }
}

void CurveView::drawLabels(Axis axis, bool shiftOrigin, KDContext * ctx, KDRect rect) const {
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
    // TODO: Find another way to avoid float comparison.
    if (x == 0.0f && shiftOrigin) {
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

void CurveView::drawAxes(Axis axis, KDContext * ctx, KDRect rect) const {
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
constexpr static int k_externRectMargin = 1;

void CurveView::drawCurve(void * curve, KDColor color, KDContext * ctx, KDRect rect, bool colorUnderCurve, float colorLowerBound, float colorUpperBound, bool continuously) const {
  float xMin = min(Axis::Horizontal);
  float xMax = max(Axis::Horizontal);
  float xStep = (xMax-xMin)/k_resolution;
  float rectMin = pixelToFloat(Axis::Horizontal, rect.left() - k_externRectMargin);
  float rectMax = pixelToFloat(Axis::Horizontal, rect.right() + k_externRectMargin);
  for (float x = rectMin; x < rectMax; x += xStep) {
    float y = evaluateCurveAtAbscissa(curve, x);
    float pxf = floatToPixel(Axis::Horizontal, x);
    float pyf = floatToPixel(Axis::Vertical, y);
    if (colorUnderCurve && x > colorLowerBound && x < colorUpperBound) {
      KDRect colorRect((int)pxf, roundf(pyf), 1, floatToPixel(Axis::Vertical, 0.0f) - roundf(pyf));
      if (floatToPixel(Axis::Vertical, 0.0f) < roundf(pyf)) {
        colorRect = KDRect((int)pxf, floatToPixel(Axis::Vertical, 0.0f), 1, roundf(pyf) - floatToPixel(Axis::Vertical, 0.0f));
      }
      ctx->fillRect(colorRect, color);
    }
    stampAtLocation(pxf, pyf, color, ctx, rect);
    if (x > rectMin) {
      if (continuously) {
        float puf = floatToPixel(Axis::Horizontal, x - xStep);
        float pvf = floatToPixel(Axis::Vertical, evaluateCurveAtAbscissa(curve, x-xStep));
        straightJoinDots(puf, pvf, pxf, pyf, color, ctx, rect);
      } else {
        jointDots(curve, x - xStep, evaluateCurveAtAbscissa(curve, x-xStep), x, y, color, k_maxNumberOfIterations, ctx, rect);
      }
    }
  }
}

void CurveView::drawHistogram(void * curve, KDColor color, KDContext * ctx, KDRect rect, bool colorUnderCurve, KDColor highlightColor, float colorLowerBound, float colorUpperBound) const {
  int rectMin = ceilf(pixelToFloat(Axis::Horizontal, rect.left()));
  int rectMax = pixelToFloat(Axis::Horizontal, rect.right());
  for (int x = rectMin; x < rectMax; x += 1) {
    float y = evaluateCurveAtAbscissa(curve, x);
    if (!isnan(y)) {
      float pxf = floatToPixel(Axis::Horizontal, x);
      float pyf = floatToPixel(Axis::Vertical, y);
      KDRect binRect((int)pxf - 1, roundf(pyf), 3, floatToPixel(Axis::Vertical, 0.0f) - roundf(pyf));
      if (floatToPixel(Axis::Vertical, 0.0f) < roundf(pyf)) {
        binRect = KDRect((int)pxf - 1, floatToPixel(Axis::Vertical, 0.0f), 3, roundf(pyf) - floatToPixel(Axis::Vertical, 0.0f));
      }
      KDColor binColor = color;
      if (colorUnderCurve && x >= colorLowerBound && x <= colorUpperBound) {
        binColor = highlightColor;
      }
      ctx->fillRect(binRect, binColor);
    }
  }
}

void CurveView::stampAtLocation(float pxf, float pyf, KDColor color, KDContext * ctx, KDRect rect) const {
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

void CurveView::jointDots(void * curve, float x, float y, float u, float v, KDColor color, int maxNumberOfRecursion, KDContext * ctx, KDRect rect) const {
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
  float cy = evaluateCurveAtAbscissa(curve, cx);
  if ((y < cy && cy < v) || (v < cy && cy < y)) {
    /* As the middle dot is vertically between the two dots, we assume that we
     * can draw a 'straight' line between the two */
    float pxf = floatToPixel(Axis::Horizontal, x);
    float puf = floatToPixel(Axis::Horizontal, u);
    straightJoinDots(pxf, pyf, puf, pvf, color, ctx, rect);
    return;
  }
  float pcxf = floatToPixel(Axis::Horizontal, cx);
  float pcyf = floatToPixel(Axis::Vertical, cy);
  if (maxNumberOfRecursion > 0) {
    stampAtLocation(pcxf, pcyf, color, ctx, rect);
    jointDots(curve, x, y, cx, cy, color, maxNumberOfRecursion-1, ctx, rect);
    jointDots(curve, cx, cy, u, v, color, maxNumberOfRecursion-1, ctx, rect);
  }
}

void CurveView::straightJoinDots(float pxf, float pyf, float puf, float pvf, KDColor color, KDContext * ctx, KDRect rect) const {
  if (pyf <= pvf) {
    for (float pnf = pyf; pnf<pvf; pnf+= 1.0f) {
      float pmf = pxf + (pnf - pyf)*(puf - pxf)/(pvf - pyf);
      stampAtLocation(pmf, pnf, color, ctx, rect);
    }
    return;
  }
  straightJoinDots(puf, pvf, pxf, pyf, color, ctx, rect);
}
