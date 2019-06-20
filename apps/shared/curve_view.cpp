#include "curve_view.h"
#include "../constant.h"
#include <poincare/print_float.h>
#include <assert.h>
#include <string.h>
#include <cmath>
#include <float.h>

using namespace Poincare;

namespace Shared {

static inline int minInt(int x, int y) { return x < y ? x : y; }

static inline float minFloat(float x, float y) { return x < y ? x : y; }
static inline float maxFloat(float x, float y) { return x > y ? x : y; }

CurveView::CurveView(CurveViewRange * curveViewRange, CurveViewCursor * curveViewCursor, BannerView * bannerView,
    View * cursorView, View * okView, bool displayBanner) :
  View(),
  m_bannerView(bannerView),
  m_curveViewCursor(curveViewCursor),
  m_curveViewRange(curveViewRange),
  m_cursorView(cursorView),
  m_okView(okView),
  m_forceOkDisplay(false),
  m_mainViewSelected(false),
  m_drawnRangeVersion(0)
{
}

void CurveView::reload() {
  uint32_t rangeVersion = m_curveViewRange->rangeChecksum();
  if (m_drawnRangeVersion != rangeVersion) {
    // FIXME: This should also be called if the *curve* changed
    m_drawnRangeVersion = rangeVersion;
    KDCoordinate bannerHeight = (m_bannerView != nullptr) ? m_bannerView->bounds().height() : 0;
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

/* We need to locate physical points on the screen more precisely than pixels,
 * hence by floating-point coordinates. We agree that the coordinates of the
 * center of a pixel corresponding to KDPoint(x,y) are precisely (x,y). In
 * particular, the coordinates of a pixel's corners are not integers but half
 * integers. Finally, a physical point with floating-point coordinates (x,y)
 * is located in the pixel with coordinates (std::round(x), std::round(y)).
 *
 * Translating CurveViewRange coordinates to pixel coordinates on the screen:
 *   Along the horizontal axis
 *     Pixel / physical coordinate     CurveViewRange coordinate
 *       0                               xMin()
 *       m_frame.width() - 1             xMax()
 *   Along the vertical axis
 *     Pixel / physical coordinate     CurveViewRange coordinate
 *       0                               yMax()
 *       m_frame.height() - 1            yMin()
 */

const float CurveView::pixelWidth() const {
  return (m_curveViewRange->xMax() - m_curveViewRange->xMin()) / (m_frame.width() - 1);
}

const float CurveView::pixelHeight() const {
  return (m_curveViewRange->yMax() - m_curveViewRange->yMin()) / (m_frame.height() - 1);
}

float CurveView::pixelToFloat(Axis axis, KDCoordinate p) const {
  return (axis == Axis::Horizontal) ?
    m_curveViewRange->xMin() + p * pixelWidth() :
    m_curveViewRange->yMax() - p * pixelHeight();
}

float CurveView::floatToPixel(Axis axis, float f) const {
  float result = (axis == Axis::Horizontal) ?
    (f - m_curveViewRange->xMin()) / pixelWidth() :
    (m_curveViewRange->yMax() - f) / pixelHeight();
  /* Make sure that the returned value is between the maximum and minimum
   * possible values of KDCoordinate. */
  if (result == NAN) {
    return NAN;
  } else if (result < KDCOORDINATE_MIN) {
    return KDCOORDINATE_MIN;
  } else if (result > KDCOORDINATE_MAX) {
    return KDCOORDINATE_MAX;
  } else {
    return result;
  }
}

void CurveView::drawGridLines(KDContext * ctx, KDRect rect, Axis axis, float step, KDColor boldColor, KDColor lightColor) const {
  Axis otherAxis = (axis == Axis::Horizontal) ? Axis::Vertical : Axis::Horizontal;
  /* We translate the pixel coordinates into floats, adding/subtracting 1 to
   * account for conversion errors. */
  float otherAxisMin = pixelToFloat(otherAxis, otherAxis == Axis::Horizontal ? rect.left() - 1 : rect.bottom() + 1);
  float otherAxisMax = pixelToFloat(otherAxis, otherAxis == Axis::Horizontal ? rect.right() + 1 : rect.top() - 1);
  const int start = otherAxisMin/step;
  const int end = otherAxisMax/step;
  for (int i = start; i <= end; i++) {
    drawLine(ctx, rect, axis, i * step, i % 2 == 0 ? boldColor : lightColor);
  }
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

int CurveView::numberOfLabels(Axis axis) const {
  float labelStep = 2.0f * gridUnit(axis);
  float minLabel = std::ceil(min(axis)/labelStep);
  float maxLabel = std::floor(max(axis)/labelStep);
  return maxLabel - minLabel + 1;
}

void CurveView::computeLabels(Axis axis) {
  float step = gridUnit(axis);
  int axisLabelsCount = numberOfLabels(axis);
  for (int i = 0; i < axisLabelsCount; i++) {
    float labelValue = labelValueAtIndex(axis, i);
    /* Label cannot hold more than k_labelBufferMaxSize characters to prevent
     * them from overprinting one another.*/
    int labelMaxSize = k_labelBufferMaxSize;
    if (axis == Axis::Horizontal) {
      float pixelsPerLabel = ((float)Ion::Display::Width)/((float)axisLabelsCount) - k_labelMargin;
      labelMaxSize = minInt(k_labelBufferMaxSize, pixelsPerLabel/k_font->glyphSize().width());
    }

    if (labelValue < step && labelValue > -step) {
      // Make sure the 0 value is really written 0
      labelValue = 0.0f;
    }

    /* Label cannot hold more than k_labelBufferSize characters to prevent them
     * from overprinting one another. */

    char * labelBuffer = label(axis, i);
    PrintFloat::convertFloatToText<float>(
        labelValue,
        labelBuffer,
        labelMaxSize,
        k_numberSignificantDigits,
        Preferences::PrintFloatMode::Decimal,
        axis == Axis::Vertical);

    if (axis == Axis::Horizontal) {
      if (labelBuffer[0] == 0) {
        /* Some labels are too big and may overlap their neighbours. We write the
         * extrema labels only. */
        computeHorizontalExtremaLabels();
        return;
      }
      if (i > 0 && strcmp(labelBuffer, label(axis, i-1)) == 0) {
        /* We need to increase the number if significant digits, otherwise some
         * labels are rounded to the same value. */
        computeHorizontalExtremaLabels(true);
        return;
      }
    }
  }
}

enum class FloatingPosition : uint8_t {
  None,
  Min,
  Max
};

void CurveView::simpleDrawBothAxesLabels(KDContext * ctx, KDRect rect) const {
  drawLabels(ctx, rect, Axis::Vertical, true);
  drawLabels(ctx, rect, Axis::Horizontal, true);
}

void CurveView::drawLabels(KDContext * ctx, KDRect rect, Axis axis, bool shiftOrigin, bool graduationOnly, bool fixCoordinate, KDCoordinate fixedCoordinate, KDColor backgroundColor) const {
  int numberLabels = numberOfLabels(axis);
  if (numberLabels <= 1) {
    return;
  }

  float verticalCoordinate = fixCoordinate ? fixedCoordinate : std::round(floatToPixel(Axis::Vertical, 0.0f));
  float horizontalCoordinate = fixCoordinate ? fixedCoordinate : std::round(floatToPixel(Axis::Horizontal, 0.0f));

  int viewHeight = bounds().height() - (bannerIsVisible() ? m_bannerView->minimalSizeForOptimalDisplay().height() : 0);

  /* If the axis is not visible, draw floating labels on the edge of the screen.
   * The X axis floating status is needed when drawing both axes labels. */
  FloatingPosition floatingHorizontalLabels = FloatingPosition::None;
  if (verticalCoordinate > viewHeight - k_font->glyphSize().height() - k_labelMargin) {
    floatingHorizontalLabels = FloatingPosition::Max;
  } else if (max(Axis::Vertical) < 0.0f) {
    floatingHorizontalLabels = FloatingPosition::Min;
  }

  FloatingPosition floatingLabels = FloatingPosition::None;
  if (axis == Axis::Horizontal) {
    floatingLabels = floatingHorizontalLabels;
  } else {
    if (horizontalCoordinate < k_labelMargin + k_font->glyphSize().width() * 3) { // We want do display at least 3 characters left of the Y axis
      floatingLabels = FloatingPosition::Min;
    } else if (max(Axis::Horizontal) < 0.0f) {
      floatingLabels = FloatingPosition::Max;
    }
  }

  /* There might be less labels than graduations, if the extrema labels are too
   * close to the screen edge to write them. We must thus draw the graduations
   * separately from the labels. */

  float labelStep = 2.0f * gridUnit(axis);
  int minLabelPixelPosition = std::round(floatToPixel(axis, labelStep * std::ceil(min(axis)/labelStep)));
  int maxLabelPixelPosition = std::round(floatToPixel(axis, labelStep * std::floor(max(axis)/labelStep)));

  // Draw the graduations

  int minDrawnLabel = 0;
  int maxDrawnLabel = numberLabels;
  if (axis == Axis::Vertical) {
    /* Do not draw an extremal vertical label if it collides with the horizontal
     * labels */
    int horizontalLabelsMargin = k_font->glyphSize().height() * 2;
    if (floatingHorizontalLabels == FloatingPosition::Min
        && maxLabelPixelPosition < horizontalLabelsMargin) {
      maxDrawnLabel--;
    } else if (floatingHorizontalLabels == FloatingPosition::Max
        && minLabelPixelPosition > viewHeight - horizontalLabelsMargin)
    {
      minDrawnLabel++;
    }
  }

  if (floatingLabels == FloatingPosition::None) {
    for (int i = minDrawnLabel; i < maxDrawnLabel; i++) {
      KDCoordinate labelPosition = std::round(floatToPixel(axis, labelValueAtIndex(axis, i)));
      KDRect graduation = axis == Axis::Horizontal ?
        KDRect(
            labelPosition,
            verticalCoordinate -(k_labelGraduationLength-2)/2,
            1,
            k_labelGraduationLength) :
        KDRect(
            horizontalCoordinate-(k_labelGraduationLength-2)/2,
            labelPosition,
            k_labelGraduationLength,
            1);
      ctx->fillRect(graduation, KDColorBlack);
    }
  }

  if (graduationOnly) {
    return;
  }

  // Draw the labels
  for (int i = minDrawnLabel; i < maxDrawnLabel; i++) {
    KDCoordinate labelPosition = std::round(floatToPixel(axis, labelValueAtIndex(axis, i)));
    char * labelI = label(axis, i);
    KDSize textSize = k_font->stringSize(labelI);
    float xPosition = 0.0f;
    float yPosition = 0.0f;

    bool positioned = false;
    if (strcmp(labelI, "0") == 0) {
      if (floatingLabels != FloatingPosition::None) {
        // Do not draw the zero, it is symbolized by the other axis
        continue;
      }
      if (shiftOrigin && floatingLabels == FloatingPosition::None) {
        xPosition = horizontalCoordinate - k_labelMargin - textSize.width();
        yPosition = verticalCoordinate + k_labelMargin;
        positioned = true;
      }
    }
    if (!positioned) {
      if (axis == Axis::Horizontal) {
        xPosition = labelPosition - textSize.width()/2;
        if (floatingLabels == FloatingPosition::None) {
          yPosition = verticalCoordinate + k_labelMargin;
        } else if (floatingLabels == FloatingPosition::Min) {
          yPosition = k_labelMargin;
        } else {
          yPosition = viewHeight - k_font->glyphSize().height() - k_labelMargin;
        }
      } else {
        yPosition = labelPosition - textSize.height()/2;
        if (floatingLabels == FloatingPosition::None) {
          xPosition = horizontalCoordinate - k_labelMargin - textSize.width();
        } else if (floatingLabels == FloatingPosition::Min) {
          xPosition = k_labelMargin;
        } else {
          xPosition = Ion::Display::Width - textSize.width() - k_labelMargin;
        }
      }
    }
    KDPoint origin = KDPoint(xPosition, yPosition);
    if (rect.intersects(KDRect(origin, textSize))) {
      ctx->drawString(labelI, origin, k_font, KDColorBlack, backgroundColor);
    }
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
        std::round(floatToPixel(Axis::Horizontal, upperBound)) - std::round(floatToPixel(Axis::Horizontal, lowerBound)), thickness
      );
      break;
    case Axis::Vertical:
      lineRect = KDRect(
        std::round(floatToPixel(Axis::Horizontal, coordinate)), std::round(floatToPixel(Axis::Vertical, upperBound)),
        thickness, std::round(floatToPixel(Axis::Vertical, lowerBound)) - std::round(floatToPixel(Axis::Vertical, upperBound))
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

void CurveView::drawDot(KDContext * ctx, KDRect rect, float x, float y, KDColor color, bool oversize) const {
  const KDCoordinate diameter = oversize ? oversizeDotDiameter : dotDiameter;
  KDCoordinate px = std::round(floatToPixel(Axis::Horizontal, x));
  KDCoordinate py = std::round(floatToPixel(Axis::Vertical, y));
  KDRect dotRect(px - diameter/2, py - diameter/2, diameter, diameter);
  if (!rect.intersects(dotRect)) {
    return;
  }
  KDColor workingBuffer[oversizeDotDiameter*oversizeDotDiameter];
  ctx->blendRectWithMask(
    dotRect, color,
    oversize ? (const uint8_t *)oversizeDotMask : (const uint8_t *)dotMask,
    workingBuffer
  );
}

void CurveView::drawGrid(KDContext * ctx, KDRect rect) const {
  KDColor boldColor = Palette::GreyMiddle;
  KDColor lightColor = Palette::GreyWhite;
  drawGridLines(ctx, rect, Axis::Vertical, m_curveViewRange->xGridUnit(), boldColor, lightColor);
  drawGridLines(ctx, rect, Axis::Horizontal, m_curveViewRange->yGridUnit(), boldColor, lightColor);
}

void CurveView::drawAxes(KDContext * ctx, KDRect rect) const {
  drawAxis(ctx, rect, Axis::Vertical);
  drawAxis(ctx, rect, Axis::Horizontal);
}

void CurveView::drawAxis(KDContext * ctx, KDRect rect, Axis axis) const {
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

void CurveView::drawCurve(KDContext * ctx, KDRect rect, EvaluateModelWithParameter evaluation, void * model, void * context, KDColor color, bool colorUnderCurve, float colorLowerBound, float colorUpperBound) const {
  const float xStep = pixelWidth();
  float rectMin = pixelToFloat(Axis::Horizontal, rect.left() - k_externRectMargin);
  float rectMax = pixelToFloat(Axis::Horizontal, rect.right() + k_externRectMargin);

  float previousY = NAN;
  float y = NAN;
  for (float x = rectMin; x < rectMax; x += xStep) {
    /* When |rectMin| >> xStep, rectMin + xStep = rectMin. In that case, quit
     * the infinite loop. */
    if (x == x-xStep || x == x+xStep) {
      return;
    }
    previousY = y;
    y = evaluation(x, model, context);
    if (std::isnan(y)|| std::isinf(y)) {
      continue;
    }
    if (colorUnderCurve && colorLowerBound < x && x < colorUpperBound) {
      drawSegment(ctx, rect, Axis::Vertical, x, minFloat(0.0f, y), maxFloat(0.0f, y), color, 1);
    }
    float pxf = floatToPixel(Axis::Horizontal, x);
    float pyf = floatToPixel(Axis::Vertical, y);
    stampAtLocation(ctx, rect, pxf, pyf, color);
    if (std::isnan(previousY)) {
      continue;
    }
    jointDots(ctx, rect, evaluation, model, context, x - xStep, previousY, x, y, color, k_maxNumberOfIterations);
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
  const float step = std::fmax(barWidth, pixelWidth());
  for (float x = rectMinLowerBound; x < rectMaxUpperBound; x += step) {
    /* When |rectMinLowerBound| >> step, rectMinLowerBound + step = rectMinLowerBound.
     * In that case, quit the infinite loop. */
    if (x == x-step || x == x+step) {
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

void CurveView::jointDots(KDContext * ctx, KDRect rect, EvaluateModelWithParameter evaluation, void * model, void * context, float x, float y, float u, float v, KDColor color, int maxNumberOfRecursion) const {
  float pxf = floatToPixel(Axis::Horizontal, x);
  float pyf = floatToPixel(Axis::Vertical, y);
  float puf = floatToPixel(Axis::Horizontal, u);
  float pvf = floatToPixel(Axis::Vertical, v);
  if (std::isnan(pyf) || std::isnan(pvf)) {
    return;
  }
  const float deltaX = pxf - puf;
  const float deltaY = pyf - pvf;
  if (deltaX*deltaX + deltaY*deltaY < circleDiameter * circleDiameter / 4.0f) {
    // the dots are already joined
    return;
  }
  // No need to draw if both dots are outside visible area
  if ((pyf < -stampSize && pvf < -stampSize) || (pyf > m_frame.height()+stampSize && pvf > m_frame.height()+stampSize)) {
    return;
  }
  // If one of the dot is infinite, we cap it with a dot outside area
  if (std::isinf(pyf)) {
    pyf = pyf > 0 ? m_frame.height()+stampSize : -stampSize;
  }
  if (std::isinf(pvf)) {
    pvf = pvf > 0 ? m_frame.height()+stampSize : -stampSize;
  }
  // C is the dot whose abscissa is between x and u
  float cx = (x + u)/2.0f;
  float cy = evaluation(cx, model, context);
  if ((y <= cy && cy <= v) || (v <= cy && cy <= y)) {
    /* As the middle dot is vertically between the two dots, we assume that we
     * can draw a 'straight' line between the two */
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
  const float deltaX = pxf - puf;
  const float deltaY = pyf - pvf;
  const float normsRatio = std::sqrt(deltaX*deltaX + deltaY*deltaY) / (circleDiameter / 2.0f);
  const float stepX = deltaX / normsRatio ;
  const float stepY = deltaY / normsRatio;
  const int numberOfStamps = std::floor(normsRatio);
  for (int i = 0; i < numberOfStamps; i++) {
    stampAtLocation(ctx, rect, puf, pvf, color);
    puf += stepX;
    pvf += stepY;
  }
}

void CurveView::stampAtLocation(KDContext * ctx, KDRect rect, float pxf, float pyf, KDColor color) const {
  // We avoid drawing when no part of the stamp is visible
  if (pyf < -stampSize - FLT_EPSILON || pyf > m_frame.height()+stampSize + FLT_EPSILON) {
    return;
  }
  /* When converting floats to KDCoordinate, we need to add -1 if the float is
   * negative, otherwise all floats in ]-1.0;1.0[ are converted to 0 and there
   * is a blob for x = 0. Try for instance f(x)=cos(x), the blob is at the
   * intersection of the curve with the left of the screen. */
  KDCoordinate px = pxf + (pxf >= 0 ? 0 : -1);
  KDCoordinate py = pyf + (pyf >= 0 ? 0 : -1);
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
      KDCoordinate bannerHeight = (m_bannerView != nullptr) ? m_bannerView->minimalSizeForOptimalDisplay().height() : 0;
      cursorFrame = KDRect(xCursorPixelPosition - (cursorSize.width()-1)/2, 0, cursorSize.width(),bounds().height()-bannerHeight);
    }
  }
  return cursorFrame;
}

KDRect CurveView::bannerFrame() {
  KDRect bannerFrame = KDRectZero;
  if (bannerIsVisible()) {
    KDCoordinate bannerHeight = m_bannerView->minimalSizeForOptimalDisplay().height();
    bannerFrame = KDRect(0, bounds().height()- bannerHeight, bounds().width(), bannerHeight);
  }
  return bannerFrame;
}

KDRect CurveView::okFrame() {
  KDRect okFrame = KDRectZero;
  if (m_okView && (m_mainViewSelected || m_forceOkDisplay)) {
    KDCoordinate bannerHeight = 0;
    if (m_bannerView != nullptr) {
      bannerHeight = m_bannerView->minimalSizeForOptimalDisplay().height();
    }
    KDSize okSize = m_okView->minimalSizeForOptimalDisplay();
    okFrame = KDRect(bounds().width()- okSize.width()-k_okHorizontalMargin, bounds().height()- bannerHeight-okSize.height()-k_okVerticalMargin, okSize);
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
      if (m_cursorView != nullptr) {
        return m_cursorView;
      }
    }
  }
  if (index == 1 && m_cursorView != nullptr && m_okView != nullptr) {
    return m_cursorView;
  }
  return m_bannerView;
}

void CurveView::computeHorizontalExtremaLabels(bool increaseNumberOfSignificantDigits) {
  Axis axis = Axis::Horizontal;
  int axisLabelsCount = numberOfLabels(axis);
  float minA = min(axis);

  /* We want to draw the extrema labels (0 and numberOfLabels -1), but if they
   * might not be fully visible, draw the labels 1 and numberOfLabels - 2. */
  bool skipExtremaLabels =
    (axisLabelsCount >= 4)
    && ((labelValueAtIndex(axis, 0) - minA)/(max(axis) - minA) < k_labelsHorizontalMarginRatio+FLT_EPSILON);
  int firstLabel = skipExtremaLabels ? 1 : 0;
  int lastLabel = axisLabelsCount - (skipExtremaLabels ? 2 : 1);

  assert(firstLabel != lastLabel);

  // All labels but the extrema are empty
  for (int i = 0; i < firstLabel; i++) {
    label(axis, i)[0] = 0;
  }
  for (int i = firstLabel + 1; i < lastLabel; i++) {
    label(axis, i)[0] = 0;
  }
  for (int i = lastLabel + 1; i < axisLabelsCount; i++) {
    label(axis, i)[0] = 0;
  }

  int minMax[] = {firstLabel, lastLabel};
  for (int i : minMax) {
    // Compute the minimal and maximal label
    PrintFloat::convertFloatToText<float>(
        labelValueAtIndex(axis, i),
        label(axis, i),
        k_labelBufferMaxSize,
        increaseNumberOfSignificantDigits ? k_bigNumberSignificantDigits : k_numberSignificantDigits,
        Preferences::PrintFloatMode::Decimal,
        false);
  }
}

float CurveView::labelValueAtIndex(Axis axis, int i) const {
  assert(i >= 0 && i < numberOfLabels(axis));
  float labelStep = 2.0f * gridUnit(axis);
  return labelStep*(std::ceil(min(axis)/labelStep)+i);
}

bool CurveView::bannerIsVisible() const {
  return m_bannerView && m_mainViewSelected;
}

}
