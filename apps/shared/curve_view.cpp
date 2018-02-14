#include "curve_view.h"
#include "../constant.h"
#include <assert.h>
#include <string.h>
#include <cmath>
#include <float.h>

using namespace Poincare;

namespace Shared {

CurveView::CurveView(CurveViewRange * curveViewRange, CurveViewCursor * curveViewCursor, BannerView * bannerView,
    View * cursorView, View * okView) :
  View(),
  m_bannerView(bannerView),
  m_curveViewCursor(curveViewCursor),
  m_curveViewRange(curveViewRange),
  m_cursorView(cursorView),
  m_okView(okView),
  m_mainViewSelected(false),
  m_drawnRangeVersion(0)
{
}

void CurveView::reload() {
  uint32_t rangeVersion = m_curveViewRange->rangeChecksum();
  if (m_drawnRangeVersion != rangeVersion) {
    // FIXME: This should also be called if the *curve* changed
    m_drawnRangeVersion = rangeVersion;
    KDCoordinate bannerHeight = m_bannerView != nullptr ? m_bannerView->bounds().height() : 0;
    markRectAsDirty(KDRect(0, 0, bounds().width(), bounds().height() - bannerHeight));
    if (label(Axis::Horizontal, 0) != nullptr) {
      computeLabels(Axis::Horizontal);
    }
    if (label(Axis::Vertical, 0) != nullptr) {
      computeLabels(Axis::Vertical);
    }
  }
  layoutSubviews();
}

bool CurveView::isMainViewSelected() const {
  return m_mainViewSelected;
}

void CurveView::selectMainView(bool mainViewSelected) {
  if (m_mainViewSelected != mainViewSelected) {
    m_mainViewSelected = mainViewSelected;
    reload();
  }
}

void CurveView::setCurveViewRange(CurveViewRange * curveViewRange) {
  m_curveViewRange = curveViewRange;
}

/* When setting cursor, banner or ok view we first dirty the former element
 * frame (in case we set the new element to be nullptr or the new element frame
 * does not recover the former element frame) and then we dirty the new element
 * frame (most of the time it is automatically done by the layout but the frame
 * might be identical to the previous one and in that case layoutSubviews will
 * do nothing). */

void CurveView::setCursorView(View * cursorView) {
  markRectAsDirty(cursorFrame());
  m_cursorView = cursorView;
  markRectAsDirty(cursorFrame());
  layoutSubviews();
}

void CurveView::setBannerView(View * bannerView) {
  markRectAsDirty(bannerFrame());
  m_bannerView = bannerView;
  layoutSubviews();
}

void CurveView::setOkView(View * okView) {
  markRectAsDirty(okFrame());
  m_okView = okView;
  layoutSubviews();
}

float CurveView::resolution() const {
  return bounds().width()*samplingRatio();
}

float CurveView::samplingRatio() const {
  return 1.1f;
}

float CurveView::min(Axis axis) const {
  assert(axis == Axis::Horizontal || axis == Axis::Vertical);
  return (axis == Axis::Horizontal ? m_curveViewRange->xMin(): m_curveViewRange->yMin());
}

float CurveView::max(Axis axis) const {
  assert(axis == Axis::Horizontal || axis == Axis::Vertical);
  return (axis == Axis::Horizontal ? m_curveViewRange->xMax() : m_curveViewRange->yMax());
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
  return min(axis) + pixels*((max(axis)-min(axis))/pixelLength(axis));
}

float CurveView::floatToPixel(Axis axis, float f) const {
  float fraction = (f-min(axis))/(max(axis)-min(axis));
  fraction = axis == Axis::Horizontal ? fraction : 1.0f - fraction;
  /* Fraction is a float that translates the relative position of f on the axis.
   * When fraction is between 0 and 1, f is visible. Otherwise, f is out of the
   * visible window. We need to clip fraction to avoid big float issue (often
   * due to float to int transformation). However, we cannot clip fraction
   * between 0 and 1 because drawing a sized stamp on the extern boarder of the
   * window should still be visible. We thus arbitrarily clip fraction between
   * -10 and 10. */
  fraction = fraction < -10.0f ? -10.0f : fraction;
  fraction = fraction > 10.0f ? 10.0f : fraction;
  return pixelLength(axis)*fraction;
}

void CurveView::computeLabels(Axis axis) {
  char buffer[PrintFloat::bufferSizeForFloatsWithPrecision(Constant::ShortNumberOfSignificantDigits)];
  float step = gridUnit(axis);
  for (int index = 0; index < numberOfLabels(axis); index++) {
    float labelValue = 2.0f*step*(std::ceil(min(axis)/(2.0f*step)))+index*2.0f*step;
    if (labelValue < step && labelValue > -step) {
      labelValue = 0.0f;
    }
    PrintFloat::convertFloatToText<float>(labelValue, buffer,
      PrintFloat::bufferSizeForFloatsWithPrecision(Constant::ShortNumberOfSignificantDigits),
      Constant::ShortNumberOfSignificantDigits, PrintFloat::Mode::Decimal);
    //TODO: check for size of label?
    strlcpy(label(axis, index), buffer, strlen(buffer)+1);
  }
}

void CurveView::drawLabels(KDContext * ctx, KDRect rect, Axis axis, bool shiftOrigin) const {
  float step = gridUnit(axis);
  float start = 2.0f*step*(std::ceil(min(axis)/(2.0f*step)));
  float end = max(axis);
  int i = 0;
  for (float x = start; x < end; x += 2.0f*step) {
    /* When |start| >> step, start + step = start. In that case, quit the
     * infinite loop. */
    if (x == x-step || x == x+step) {
      return;
    }
    KDSize textSize = KDText::stringSize(label(axis, i), KDText::FontSize::Small);
    KDPoint origin(std::round(floatToPixel(Axis::Horizontal, x)) - textSize.width()/2, std::round(floatToPixel(Axis::Vertical, 0.0f))  + k_labelMargin);
    KDRect graduation(std::round(floatToPixel(Axis::Horizontal, x)), std::round(floatToPixel(Axis::Vertical, 0.0f)) -(k_labelGraduationLength-2)/2, 1, k_labelGraduationLength);
    if (axis == Axis::Vertical) {
      origin = KDPoint(std::round(floatToPixel(Axis::Horizontal, 0.0f)) + k_labelMargin, std::round(floatToPixel(Axis::Vertical, x)) - textSize.height()/2);
      graduation = KDRect(std::round(floatToPixel(Axis::Horizontal, 0.0f))-(k_labelGraduationLength-2)/2, std::round(floatToPixel(Axis::Vertical, x)), k_labelGraduationLength, 1);
    }
    if (-step < x && x < step && shiftOrigin) {
      origin = KDPoint(std::round(floatToPixel(Axis::Horizontal, 0.0f)) + k_labelMargin, std::round(floatToPixel(Axis::Vertical, 0.0f)) + k_labelMargin);
    }
    if (rect.intersects(KDRect(origin, KDText::stringSize(label(axis, i), KDText::FontSize::Small)))) {
      ctx->blendString(label(axis, i), origin, KDText::FontSize::Small, KDColorBlack);
    }
    ctx->fillRect(graduation, KDColorBlack);
    i++;
  }
}

void CurveView::drawLine(KDContext * ctx, KDRect rect, Axis axis, float coordinate, KDColor color, KDCoordinate thickness) const {
  KDRect lineRect = KDRectZero;
  switch(axis) {
    case Axis::Horizontal:
      lineRect = KDRect(
          rect.x(), std::round(floatToPixel(Axis::Vertical, coordinate)),
          rect.width(), thickness
          );
      break;
    case Axis::Vertical:
      lineRect = KDRect(
          std::round(floatToPixel(Axis::Horizontal, coordinate)), rect.y(),
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
    case Axis::Horizontal:
      lineRect = KDRect(
          std::round(floatToPixel(Axis::Horizontal, lowerBound)), std::round(floatToPixel(Axis::Vertical, coordinate)),
          std::round(floatToPixel(Axis::Horizontal, upperBound) - floatToPixel(Axis::Horizontal, lowerBound)), thickness
          );
      break;
    case Axis::Vertical:
      lineRect = KDRect(
          std::round(floatToPixel(Axis::Horizontal, coordinate)), std::round(floatToPixel(Axis::Vertical, upperBound)),
          thickness,  std::round(floatToPixel(Axis::Vertical, lowerBound) - floatToPixel(Axis::Vertical, upperBound))
      );
      break;
  }
  if (rect.intersects(lineRect)) {
    ctx->fillRect(lineRect, color);
  }
}

constexpr KDCoordinate dotDiameter = 5;
const uint8_t dotMask[dotDiameter][dotDiameter] = {
  {0xE1, 0x45, 0x0C, 0x45, 0xE1},
  {0x45, 0x00, 0x00, 0x00, 0x45},
  {0x00, 0x00, 0x00, 0x00, 0x00},
  {0x45, 0x00, 0x00, 0x00, 0x45},
  {0xE1, 0x45, 0x0C, 0x45, 0xE1},
};

constexpr KDCoordinate oversizeDotDiameter = 7;
const uint8_t oversizeDotMask[oversizeDotDiameter][oversizeDotDiameter] = {
  {0xE1, 0x45, 0x0C, 0x00, 0x0C, 0x45, 0xE1},
  {0x45, 0x0C, 0x00, 0x00, 0x00, 0x0C, 0x45},
  {0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C},
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C},
  {0x45, 0x0C, 0x00, 0x00, 0x00, 0x0C, 0x45},
  {0xE1, 0x45, 0x0C, 0x00, 0x0C, 0x45, 0xE1},

};

KDColor s_dotWorkingBuffer[dotDiameter*dotDiameter];
KDColor s_oversizeDotWorkingBuffer[oversizeDotDiameter*oversizeDotDiameter];

void CurveView::drawDot(KDContext * ctx, KDRect rect, float x, float y, KDColor color, bool oversize) const {
  KDCoordinate px = std::round(floatToPixel(Axis::Horizontal, x));
  KDCoordinate py = std::round(floatToPixel(Axis::Vertical, y));
  if ((px + dotDiameter < rect.left() - k_externRectMargin || px - dotDiameter > rect.right() + k_externRectMargin) ||
      (py + dotDiameter < rect.top() - k_externRectMargin || py - dotDiameter > rect.bottom() + k_externRectMargin)) {
    return;
  }
  KDRect dotRect = KDRect(px - dotDiameter/2, py-dotDiameter/2, dotDiameter, dotDiameter);
  ctx->blendRectWithMask(dotRect, color, (const uint8_t *)dotMask, s_dotWorkingBuffer);
  if (oversize) {
    KDRect oversizeDotRect = KDRect(px - oversizeDotDiameter/2, py-oversizeDotDiameter/2, oversizeDotDiameter, oversizeDotDiameter);
    ctx->blendRectWithMask(oversizeDotRect, color, (const uint8_t *)oversizeDotMask, s_oversizeDotWorkingBuffer);
  }
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
    /* When |start| >> step, start + step = start. In that case, quit the
     * infinite loop. */
    if (x == x-step || x == x+step) {
      return;
    }
    if (rectMin <= x && x <= rectMax) {
      drawLine(ctx, rect, otherAxis, x, color);
    }
  }
}

void CurveView::drawGrid(KDContext * ctx, KDRect rect) const {
  drawGridLines(ctx, rect, Axis::Horizontal, m_curveViewRange->xGridUnit(), Palette::GreyWhite);
  drawGridLines(ctx, rect, Axis::Vertical, m_curveViewRange->yGridUnit(), Palette::GreyWhite);
}

void CurveView::drawAxes(KDContext * ctx, KDRect rect, Axis axis) const {
  drawLine(ctx, rect, axis, 0.0f, KDColorBlack, 1);
}

#define LINE_THICKNESS 2

#if LINE_THICKNESS == 1

constexpr KDCoordinate circleDiameter = 1;
constexpr KDCoordinate stampSize = circleDiameter+1;
const uint8_t stampMask[stampSize+1][stampSize+1] = {
  {0xFF, 0xE1, 0xFF},
  {0xE1, 0x00, 0xE1},
  {0xFF, 0xE1, 0xFF},
};

#elif LINE_THICKNESS == 2

constexpr KDCoordinate circleDiameter = 2;
constexpr KDCoordinate stampSize = circleDiameter+1;
const uint8_t stampMask[stampSize+1][stampSize+1] = {
  {0xFF, 0xE6, 0xE6, 0xFF},
  {0xE6, 0x33, 0x33, 0xE6},
  {0xE6, 0x33, 0x33, 0xE6},
  {0xFF, 0xE6, 0xE6, 0xFF},
};

#elif LINE_THICKNESS == 3

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

void CurveView::drawCurve(KDContext * ctx, KDRect rect, EvaluateModelWithParameter evaluation, void * model, void * context, KDColor color, bool colorUnderCurve, float colorLowerBound, float colorUpperBound, bool continuously) const {
  float xMin = min(Axis::Horizontal);
  float xMax = max(Axis::Horizontal);
  float xStep = (xMax-xMin)/resolution();
  float rectMin = pixelToFloat(Axis::Horizontal, rect.left() - k_externRectMargin);
  float rectMax = pixelToFloat(Axis::Horizontal, rect.right() + k_externRectMargin);

  float pixelColorLowerBound = std::round(floatToPixel(Axis::Horizontal, colorLowerBound));
  float pixelColorUpperBound = std::round(floatToPixel(Axis::Horizontal, colorUpperBound));

  for (float x = rectMin; x < rectMax; x += xStep) {
    /* When |rectMin| >> xStep, rectMin + xStep = rectMin. In that case, quit
     * the infinite loop. */
    if (x == x-xStep || x == x+xStep) {
      return;
    }
    float y = evaluation(x, model, context);
    if (std::isnan(y)|| std::isinf(y)) {
      continue;
    }
    float pxf = floatToPixel(Axis::Horizontal, x);
    float pyf = floatToPixel(Axis::Vertical, y);
    if (colorUnderCurve && pxf > pixelColorLowerBound && pxf < pixelColorUpperBound) {
      KDRect colorRect((int)pxf, std::round(pyf), 1, std::round(floatToPixel(Axis::Vertical, 0.0f)) - std::round(pyf));
      if (floatToPixel(Axis::Vertical, 0.0f) < std::round(pyf)) {
        colorRect = KDRect((int)pxf, std::round(floatToPixel(Axis::Vertical, 0.0f)), 1, std::round(pyf) - std::round(floatToPixel(Axis::Vertical, 0.0f)));
      }
      ctx->fillRect(colorRect, color);
    }
    stampAtLocation(ctx, rect, pxf, pyf, color);
    if (x <= rectMin || std::isnan(evaluation(x-xStep, model, context))) {
      continue;
    }
    if (continuously) {
      float puf = floatToPixel(Axis::Horizontal, x - xStep);
      float pvf = floatToPixel(Axis::Vertical, evaluation(x-xStep, model, context));
      straightJoinDots(ctx, rect, puf, pvf, pxf, pyf, color);
    } else {
      jointDots(ctx, rect, evaluation, model, context, x - xStep, evaluation(x-xStep, model, context), x, y, color, k_maxNumberOfIterations);
    }
  }
}

void CurveView::drawHistogram(KDContext * ctx, KDRect rect, EvaluateModelWithParameter evaluation, void * model, void * context, float firstBarAbscissa, float barWidth,
    bool fillBar, KDColor defaultColor, KDColor highlightColor,  float highlightLowerBound, float highlightUpperBound) const {
  float rectMin = pixelToFloat(Axis::Horizontal, rect.left());
  float rectMinBinNumber = std::floor((rectMin - firstBarAbscissa)/barWidth);
  float rectMinLowerBound = firstBarAbscissa + rectMinBinNumber*barWidth;
  float rectMax = pixelToFloat(Axis::Horizontal, rect.right());
  float rectMaxBinNumber = std::floor((rectMax - firstBarAbscissa)/barWidth);
  float rectMaxUpperBound = firstBarAbscissa + (rectMaxBinNumber+1)*barWidth + barWidth;
  float pHighlightLowerBound = floatToPixel(Axis::Horizontal, highlightLowerBound);
  float pHighlightUpperBound = floatToPixel(Axis::Horizontal, highlightUpperBound);
  for (float x = rectMinLowerBound; x < rectMaxUpperBound; x += barWidth) {
    /* When |rectMinLowerBound| >> barWidth, rectMinLowerBound + barWidth = rectMinLowerBound.
     * In that case, quit the infinite loop. */
    if (x == x-barWidth || x == x+barWidth) {
      return;
    }
    float centerX = fillBar ? x+barWidth/2.0f : x;
    float y = evaluation(centerX, model, context);
    if (std::isnan(y)) {
      continue;
    }
    KDCoordinate pxf = std::round(floatToPixel(Axis::Horizontal, x));
    KDCoordinate pyf = std::round(floatToPixel(Axis::Vertical, y));
    KDCoordinate pixelBarWidth = fillBar ? std::round(floatToPixel(Axis::Horizontal, x+barWidth)) - std::round(floatToPixel(Axis::Horizontal, x))-1 : 2;
    KDRect binRect(pxf, pyf, pixelBarWidth, std::round(floatToPixel(Axis::Vertical, 0.0f)) - pyf);
    if (floatToPixel(Axis::Vertical, 0.0f) < pyf) {
      binRect = KDRect(pxf, std::round(floatToPixel(Axis::Vertical, 0.0f)), pixelBarWidth+1, pyf - std::round(floatToPixel(Axis::Vertical, 0.0f)));
    }
    KDColor binColor = defaultColor;
    bool shouldColorBin = fillBar ? centerX >= highlightLowerBound && centerX <= highlightUpperBound : pxf >= floorf(pHighlightLowerBound) && pxf <= floorf(pHighlightUpperBound);
    if (shouldColorBin) {
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
  return std::ceil((max(axis) - min(axis))/(2*gridUnit(axis)));
}

void CurveView::jointDots(KDContext * ctx, KDRect rect, EvaluateModelWithParameter evaluation, void * model, void * context, float x, float y, float u, float v, KDColor color, int maxNumberOfRecursion) const {
  float pyf = floatToPixel(Axis::Vertical, y);
  float pvf = floatToPixel(Axis::Vertical, v);
  if (std::isnan(pyf) || std::isnan(pvf)) {
    return;
  }
  // No need to draw if both dots are outside visible area
  if ((pyf < -stampSize && pvf < -stampSize) || (pyf > pixelLength(Axis::Vertical)+stampSize && pvf > pixelLength(Axis::Vertical)+stampSize)) {
    return;
  }
  // If one of the dot is infinite, we cap it with a dot outside area
  if (std::isinf(pyf)) {
    pyf = pyf > 0 ? pixelLength(Axis::Vertical)+stampSize : -stampSize;
  }
  if (std::isinf(pvf)) {
    pvf = pvf > 0 ? pixelLength(Axis::Vertical)+stampSize : -stampSize;
  }
  if (pyf - (float)circleDiameter/2.0f < pvf && pvf < pyf + (float)circleDiameter/2.0f) {
    // the dots are already joined
    return;
  }
  // C is the dot whose abscissa is between x and u
  float cx = (x + u)/2.0f;
  float cy = evaluation(cx, model, context);
  if ((y <= cy && cy <= v) || (v <= cy && cy <= y)) {
    /* As the middle dot is vertically between the two dots, we assume that we
     * can draw a 'straight' line between the two */
    float pxf = floatToPixel(Axis::Horizontal, x);
    float puf = floatToPixel(Axis::Horizontal, u);
    if (std::isnan(pxf) || std::isnan(puf)) {
      return;
    }
    straightJoinDots(ctx, rect, pxf, pyf, puf, pvf, color);
    return;
  }
  float pcxf = floatToPixel(Axis::Horizontal, cx);
  float pcyf = floatToPixel(Axis::Vertical, cy);
  if (maxNumberOfRecursion > 0) {
    stampAtLocation(ctx, rect, pcxf, pcyf, color);
    jointDots(ctx, rect, evaluation, model, context, x, y, cx, cy, color, maxNumberOfRecursion-1);
    jointDots(ctx, rect, evaluation, model, context, cx, cy, u, v, color, maxNumberOfRecursion-1);
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
  KDRect stampRect(px-(circleDiameter-2)/2, py-(circleDiameter-2)/2, stampSize, stampSize);
  if (!rect.intersects(stampRect)) {
    return;
  }
  uint8_t shiftedMask[stampSize][stampSize];
  KDColor workingBuffer[stampSize*stampSize];
  float dx = pxf - std::floor(pxf);
  float dy = pyf - std::floor(pyf);
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
    m_cursorView->setFrame(cursorFrame());
  }
  if (m_bannerView != nullptr) {
    m_bannerView->setFrame(bannerFrame());
  }
  if (m_okView != nullptr) {
    m_okView->setFrame(okFrame());
  }
}

KDRect CurveView::cursorFrame() {
  KDRect cursorFrame = KDRectZero;
  if (m_cursorView && m_mainViewSelected && !std::isnan(m_curveViewCursor->x()) && !std::isnan(m_curveViewCursor->y())) {
    KDSize cursorSize = m_cursorView->minimalSizeForOptimalDisplay();
    KDCoordinate xCursorPixelPosition = std::round(floatToPixel(Axis::Horizontal, m_curveViewCursor->x()));
    KDCoordinate yCursorPixelPosition = std::round(floatToPixel(Axis::Vertical, m_curveViewCursor->y()));
    cursorFrame = KDRect(xCursorPixelPosition - (cursorSize.width()-1)/2, yCursorPixelPosition - (cursorSize.height()-1)/2, cursorSize.width(), cursorSize.height());
    if (cursorSize.height() == 0) {
      KDCoordinate bannerHeight = m_bannerView != nullptr ? m_bannerView->minimalSizeForOptimalDisplay().height() : 0;
      cursorFrame = KDRect(xCursorPixelPosition - (cursorSize.width()-1)/2, 0, cursorSize.width(),bounds().height()-bannerHeight);
    }
  }
  return cursorFrame;
}

KDRect CurveView::bannerFrame() {
  KDRect bannerFrame = KDRectZero;
  if (m_bannerView && m_mainViewSelected) {
    KDCoordinate bannerHeight = m_bannerView->minimalSizeForOptimalDisplay().height();
    bannerFrame = KDRect(0, bounds().height()- bannerHeight, bounds().width(), bannerHeight);
  }
  return bannerFrame;
}

KDRect CurveView::okFrame() {
  KDRect okFrame = KDRectZero;
  if (m_okView && m_mainViewSelected) {
    KDCoordinate bannerHeight = 0;
    if (m_bannerView != nullptr) {
      bannerHeight = m_bannerView->minimalSizeForOptimalDisplay().height();
    }
    KDSize okSize = m_okView->minimalSizeForOptimalDisplay();
    okFrame = KDRect(bounds().width()- okSize.width()-k_okMargin, bounds().height()- bannerHeight-okSize.height()-k_okMargin, okSize);
  }
  return okFrame;
}

int CurveView::numberOfSubviews() const {
  return (m_bannerView != nullptr) + (m_cursorView != nullptr) + (m_okView != nullptr);
};

View * CurveView::subviewAtIndex(int index) {
  assert(index >= 0 && index < 3);
  /* If all subviews exist, we want Ok view to be the first child to avoid
   * redrawing it because it falls in the union of dirty rectangles linked to
   * the banner view and curve view */
  if (index == 0) {
    if (m_okView != nullptr) {
      return m_okView;
    } else {
      if (m_bannerView != nullptr) {
        return m_bannerView;
      }
    }
  }
  if (index == 1 && m_bannerView != nullptr && m_okView != nullptr) {
    return m_bannerView;
  }
  return m_cursorView;
}

}
