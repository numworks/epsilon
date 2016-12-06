#include "curve_view.h"
#include "constant.h"
#include <assert.h>
#include <math.h>
#include <float.h>
#include <string.h>

constexpr KDColor CurveView::k_axisColor;

CurveView::CurveView() :
  View()
{
}

void CurveView::reload() {
  markRectAsDirty(bounds());
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
  return ceilf((max(axis) - min(axis))/(2*scale(axis)));
}

void CurveView::computeLabels(Axis axis) {
  char buffer[Constant::FloatBufferSizeInScientificMode];
  float step = scale(axis);
  for (int index = 0; index < numberOfLabels(axis); index++) {
    // TODO: change the number of digits in mantissa once the numerical mode is implemented
    Float(2.0f*step*(ceilf(min(axis)/(2.0f*step)))+index*2.0f*step).convertFloatToText(buffer, Constant::FloatBufferSizeInScientificMode, Constant::NumberOfDigitsInMantissaInScientificMode);
    //TODO: check for size of label?
    strlcpy(label(axis, index), buffer, strlen(buffer)+1);
  }
}

void CurveView::drawLabels(Axis axis, KDContext * ctx, KDRect rect) const {
  float step = scale(axis);
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
    if (x == 0.0f) {
      origin = KDPoint(floatToPixel(Axis::Horizontal, 0.0f) + k_labelMargin, floatToPixel(Axis::Vertical, 0.0f) + k_labelMargin);
    }
    ctx->drawString(label(axis, i++), origin, KDColorBlack, KDColorWhite);
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
  ctx->fillRect(lineRect, color);
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

void CurveView::drawExpression(Expression * expression, KDColor color, KDContext * ctx, KDRect rect) const {
  float xMin = min(Axis::Horizontal);
  float xMax = max(Axis::Horizontal);
  float xStep = (xMax-xMin)/320.0f;
  for (float x = xMin; x < xMax; x += xStep) {
    float y = evaluateExpressionAtAbscissa(expression, x);
    float pxf = floatToPixel(Axis::Horizontal, x);
    float pyf = floatToPixel(Axis::Vertical, y);
    stampAtLocation(pxf, pyf, color, ctx);
    if (x > xMin) {
      jointDots(expression, x - xStep, evaluateExpressionAtAbscissa(expression, x-xStep), x, y, color, k_maxNumberOfIterations, ctx);
    }
  }
}

void CurveView::stampAtLocation(float pxf, float pyf, KDColor color, KDContext * ctx) const {
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

void CurveView::jointDots(Expression * expression, float x, float y, float u, float v, KDColor color, int maxNumberOfRecursion, KDContext * ctx) const {
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
  float cy = evaluateExpressionAtAbscissa(expression, cx);
  if ((y < cy && cy < v) || (v < cy && cy < y)) {
    /* As the middle dot is vertically between the two dots, we assume that we
     * can draw a 'straight' line between the two */
    float pxf = floatToPixel(Axis::Horizontal, x);
    float puf = floatToPixel(Axis::Horizontal, u);
    straightJoinDots(pxf, pyf, puf, pvf, color, ctx);
    return;
  }
  float pcxf = floatToPixel(Axis::Horizontal, cx);
  float pcyf = floatToPixel(Axis::Vertical, cy);
  if (maxNumberOfRecursion > 0) {
    stampAtLocation(pcxf, pcyf, color, ctx);
    jointDots(expression, x, y, cx, cy, color, maxNumberOfRecursion-1, ctx);
    jointDots(expression, cx, cy, u, v, color, maxNumberOfRecursion-1, ctx);
  }
}

void CurveView::straightJoinDots(float pxf, float pyf, float puf, float pvf, KDColor color, KDContext * ctx) const {
  if (pyf < pvf) {
    for (float pnf = pyf; pnf<pvf; pnf+= 1.0f) {
      float pmf = pxf + (pnf - pyf)*(puf - pxf)/(pvf - pyf);
      stampAtLocation(pmf, pnf, color, ctx);
    }
    return;
  }
  straightJoinDots(puf, pvf, pxf, pyf, color, ctx);
}
