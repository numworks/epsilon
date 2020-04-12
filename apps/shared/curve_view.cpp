#include "curve_view.h"
#include "../constant.h"
#include "dots.h"
#include <poincare/print_float.h>
#include <assert.h>
#include <string.h>
#include <algorithm>
#include <cmath>
#include <float.h>

using namespace Poincare;

namespace Shared {

CurveView::CurveView(CurveViewRange * curveViewRange, CurveViewCursor * curveViewCursor, BannerView * bannerView,
    CursorView * cursorView, View * okView, bool displayBanner) :
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

void CurveView::setCursorView(CursorView * cursorView) {
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

float CurveView::pixelWidth() const {
  return (m_curveViewRange->xMax() - m_curveViewRange->xMin()) / (m_frame.width() - 1);
}

float CurveView::pixelHeight() const {
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
  int numberOfLabels = maxLabel - minLabel + 1;
  assert(numberOfLabels <= (axis == Axis::Horizontal ? k_maxNumberOfXLabels : k_maxNumberOfYLabels));
  return numberOfLabels;
}

void CurveView::computeLabels(Axis axis) {
  float step = gridUnit(axis);
  int axisLabelsCount = numberOfLabels(axis);
  for (int i = 0; i < axisLabelsCount; i++) {
    float labelValue = labelValueAtIndex(axis, i);
    /* Label cannot hold more than k_labelBufferMaxGlyphLength characters to prevent
     * them from overprinting one another.*/
    int labelMaxGlyphLength = labelMaxGlyphLengthSize();
    if (axis == Axis::Horizontal) {
      float pixelsPerLabel = std::max(0.0f, ((float)Ion::Display::Width)/((float)axisLabelsCount) - k_labelMargin);
      labelMaxGlyphLength = std::min<int>(labelMaxGlyphLengthSize(), pixelsPerLabel/k_font->glyphSize().width());
    }

    if (labelValue < step && labelValue > -step) {
      // Make sure the 0 value is really written 0
      labelValue = 0.0f;
    }

    /* Label cannot hold more than k_labelBufferSize characters to prevent them
     * from overprinting one another. */

    char * labelBuffer = label(axis, i);
    PrintFloat::ConvertFloatToText<float>(
        labelValue,
        labelBuffer,
        k_labelBufferMaxSize,
        labelMaxGlyphLength,
        k_numberSignificantDigits,
        Preferences::PrintFloatMode::Decimal);

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

void CurveView::simpleDrawBothAxesLabels(KDContext * ctx, KDRect rect) const {
  drawLabelsAndGraduations(ctx, rect, Axis::Vertical, true);
  drawLabelsAndGraduations(ctx, rect, Axis::Horizontal, true);
}

KDPoint CurveView::positionLabel(KDCoordinate xPosition, KDCoordinate yPosition, KDSize labelSize, RelativePosition horizontalPosition, RelativePosition verticalPosition) const {
  switch (horizontalPosition) {
    case RelativePosition::Before: // Left
      xPosition -= labelSize.width() + k_labelMargin;
      break;
    case RelativePosition::After: // Right
      xPosition += k_labelMargin;
      break;
    default:
      xPosition -= labelSize.width()/2;
  }
  switch (verticalPosition) {
    case RelativePosition::After: // Above
      yPosition -= labelSize.height() + k_labelMargin;
      break;
    case RelativePosition::Before: // Below
      yPosition += k_labelMargin;
      break;
    default:
      yPosition -= labelSize.height()/2;
  }
  return KDPoint(xPosition, yPosition);
}

void CurveView::drawLabel(KDContext * ctx, KDRect rect, float xPosition, float yPosition, const char * label, KDColor color, RelativePosition horizontalPosition, RelativePosition verticalPosition) const {
  KDSize labelSize = k_font->stringSize(label);
  KDCoordinate xCoordinate = std::round(floatToPixel(Axis::Horizontal, xPosition));
  KDCoordinate yCoordinate = std::round(floatToPixel(Axis::Vertical, yPosition));
  KDPoint position = positionLabel(xCoordinate, yCoordinate, labelSize, horizontalPosition, verticalPosition);
  if (rect.intersects(KDRect(position, labelSize))) {
    // TODO: should we blend?
    ctx->drawString(label, position, k_font, color, KDColorWhite);
  }
}

enum class FloatingPosition : uint8_t {
  None,
  Min,
  Max
};

void CurveView::drawLabelsAndGraduations(KDContext * ctx, KDRect rect, Axis axis, bool shiftOrigin, bool graduationOnly, bool fixCoordinate, KDCoordinate fixedCoordinate, KDColor backgroundColor) const {
  int numberLabels = numberOfLabels(axis);
  if (numberLabels <= 1) {
    return;
  }

  float verticalCoordinate = fixCoordinate ? fixedCoordinate : std::round(floatToPixel(Axis::Vertical, 0.0f));
  float horizontalCoordinate = fixCoordinate ? fixedCoordinate : std::round(floatToPixel(Axis::Horizontal, 0.0f));

  KDCoordinate viewHeight = bounds().height() - (bannerIsVisible() ? m_bannerView->minimalSizeForOptimalDisplay().height() : 0);

  /* If the axis is not visible, draw floating labels on the edge of the screen.
   * The X axis floating status is needed when drawing both axes labels. */
  FloatingPosition floatingHorizontalLabels = FloatingPosition::None;
  KDCoordinate maximalVerticalPosition = graduationOnly ? viewHeight : viewHeight - k_font->glyphSize().height() - k_labelMargin;
  if (verticalCoordinate > maximalVerticalPosition) {
    floatingHorizontalLabels = FloatingPosition::Max;
  } else if (max(Axis::Vertical) < 0.0f) {
    floatingHorizontalLabels = FloatingPosition::Min;
  }

  FloatingPosition floatingLabels = FloatingPosition::None;
  if (axis == Axis::Horizontal) {
    floatingLabels = floatingHorizontalLabels;
  } else {
    KDCoordinate minimalHorizontalPosition = graduationOnly ? 0 : k_labelMargin + k_font->glyphSize().width() * 3; // We want do display at least 3 characters left of the Y axis
    if (horizontalCoordinate < minimalHorizontalPosition) {
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
    KDPoint position = KDPointZero;
    if (strcmp(labelI, "0") == 0) {
      if (floatingLabels != FloatingPosition::None) {
        // Do not draw the zero, it is symbolized by the other axis
        continue;
      }
      if (shiftOrigin && floatingLabels == FloatingPosition::None) {
        position = positionLabel(horizontalCoordinate, verticalCoordinate, textSize, RelativePosition::Before, RelativePosition::Before);
        goto DrawLabel;
      }
    }
    if (axis == Axis::Horizontal) {
      position = positionLabel(labelPosition, verticalCoordinate, textSize, RelativePosition::None, RelativePosition::Before);
      if (floatingLabels == FloatingPosition::Min) {
        position = KDPoint(position.x(), k_labelMargin);
      } else if (floatingLabels == FloatingPosition::Max) {
        position = KDPoint(position.x(), viewHeight - k_font->glyphSize().height() - k_labelMargin);
      }
    } else {
      position = positionLabel(horizontalCoordinate, labelPosition, textSize, RelativePosition::Before, RelativePosition::None);
      if (floatingLabels == FloatingPosition::Min) {
        position = KDPoint(k_labelMargin, position.y());
      } else if (floatingLabels == FloatingPosition::Max) {
        position = KDPoint(Ion::Display::Width - textSize.width() - k_labelMargin, position.y());
      }
    }

DrawLabel:
    if (rect.intersects(KDRect(position, textSize))) {
      ctx->drawString(labelI, position, k_font, KDColorBlack, backgroundColor);
    }
  }
}

void CurveView::drawHorizontalOrVerticalSegment(KDContext * ctx, KDRect rect, Axis axis, float coordinate, float lowerBound, float upperBound, KDColor color, KDCoordinate thickness, KDCoordinate dashSize) const {
  KDCoordinate min = (axis == Axis::Horizontal) ? rect.x() : rect.y();
  KDCoordinate max = (axis == Axis::Horizontal) ? rect.x() + rect.width() : rect.y() + rect.height();
  KDCoordinate start = std::isinf(lowerBound) ? min : std::round(floatToPixel(axis, lowerBound));
  KDCoordinate end = std::isinf(upperBound) ? max : std::round(floatToPixel(axis, upperBound));
  if (start > end) {
    start = end;
    end = std::round(floatToPixel(axis, lowerBound));
  }
  Axis otherAxis = (axis == Axis::Horizontal) ? Axis::Vertical : Axis::Horizontal;
  KDCoordinate pixelCoordinate = std::round(floatToPixel(otherAxis, coordinate));
  if (dashSize < 0) {
    // Continuous segment is equivalent to one big dash
    dashSize = end - start;
  }
  KDRect lineRect = KDRectZero;
  for (KDCoordinate i = start; i < end; i += 2*dashSize) {
    switch(axis) {
      case Axis::Horizontal:
        lineRect = KDRect(i, pixelCoordinate, dashSize, thickness);
        break;
      case Axis::Vertical:
        lineRect = KDRect(pixelCoordinate, i, thickness, dashSize);
        break;
    }
    if (rect.intersects(lineRect)) {
      ctx->fillRect(lineRect, color);
    }
  }
}

void CurveView::drawSegment(KDContext * ctx, KDRect rect, float x, float y, float u, float v, KDColor color, bool thick) const {
  float pxf = floatToPixel(Axis::Horizontal, x);
  float pyf = floatToPixel(Axis::Vertical, y);
  float puf = floatToPixel(Axis::Horizontal, u);
  float pvf = floatToPixel(Axis::Vertical, v);
  straightJoinDots(ctx, rect, pxf, pyf, puf, pvf, color, thick);
}

void CurveView::drawDot(KDContext * ctx, KDRect rect, float x, float y, KDColor color, Size size) const {
  KDCoordinate diameter = 0;
  const uint8_t * mask = nullptr;
  switch (size) {
    case Size::Small:
      diameter = Dots::SmallDotDiameter;
      mask = (const uint8_t *)Dots::SmallDotMask;
      break;
    case Size::Medium:
      diameter = Dots::MediumDotDiameter;
      mask = (const uint8_t *)Dots::MediumDotMask;
      break;
    default:
      assert(size == Size::Large);
      diameter = Dots::LargeDotDiameter;
      mask = (const uint8_t *)Dots::LargeDotMask;
  }
  KDCoordinate px = std::round(floatToPixel(Axis::Horizontal, x));
  KDCoordinate py = std::round(floatToPixel(Axis::Vertical, y));
  KDRect dotRect(px - diameter/2, py - diameter/2, diameter, diameter);
  if (!rect.intersects(dotRect)) {
    return;
  }
  KDColor workingBuffer[Dots::LargeDotDiameter*Dots::LargeDotDiameter];
  ctx->blendRectWithMask(dotRect, color, mask, workingBuffer);
}


void CurveView::drawArrow(KDContext * ctx, KDRect rect, float x, float y, float dx, float dy, KDColor color, KDCoordinate pixelArrowLength, float angle) const {
  /* Let's call the following variables L and l:
   *
   *            /                  |
   *          /                    |
   *        /                      l
   *      /                        |
   *    /                          |
   *  <--------------------------------------------------
   *    \
   *      \
   *        \
   *          \
   *            \
   *
   * ----- L -----
   *
   **/
  assert(angle >= 0.0f);
  if (std::fabs(dx) < FLT_EPSILON && std::fabs(dy) < FLT_EPSILON) {
    // We can't draw an arrow without any orientation
    return;
  }
  /* We compute the arrow segments in pixels in order to correctly size the
   * arrow without depending on the displayed range.
   * Warning: the computed values are relative so we need to add/subtract the
   * pixel position of 0s. */
  float x0Pixel = floatToPixel(Axis::Horizontal, 0.0f);
  float y0Pixel = floatToPixel(Axis::Vertical, 0.0f);
  float dxPixel = floatToPixel(Axis::Horizontal, dx) - x0Pixel;
  float dyPixel = y0Pixel - floatToPixel(Axis::Vertical, dy);
  float dx2dy2 = std::sqrt(dxPixel*dxPixel+dyPixel*dyPixel);
  float L = pixelArrowLength;
  float l = angle*L;

  float arrow1dx = pixelToFloat(Axis::Horizontal, x0Pixel + L*dxPixel/dx2dy2 + l*dyPixel/dx2dy2);
  float arrow1dy = pixelToFloat(Axis::Vertical, y0Pixel - (L*dyPixel/dx2dy2 - l*dxPixel/dx2dy2));
  drawSegment(ctx, rect, x, y, x - arrow1dx, y - arrow1dy, color, false);

  float arrow2dx =  pixelToFloat(Axis::Horizontal, x0Pixel + L*dxPixel/dx2dy2 - l*dyPixel/dx2dy2);
  float arrow2dy = pixelToFloat(Axis::Vertical, y0Pixel - (L*dyPixel/dx2dy2 + l*dxPixel/dx2dy2));
  drawSegment(ctx, rect, x, y, x - arrow2dx, y - arrow2dy, color, false);
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

constexpr KDCoordinate thinCircleDiameter = 1;
constexpr KDCoordinate thinStampSize = thinCircleDiameter+1;
const uint8_t thinStampMask[(thinStampSize+1)*(thinStampSize+1)] = {
  0xFF, 0xE1, 0xFF,
  0xE1, 0x00, 0xE1,
  0xFF, 0xE1, 0xFF,
};

#define LINE_THICKNESS 2

#if LINE_THICKNESS == 2

constexpr KDCoordinate thickCircleDiameter = 2;
constexpr KDCoordinate thickStampSize = thickCircleDiameter+1;
const uint8_t thickStampMask[(thickStampSize+1)*(thickStampSize+1)] = {
  0xFF, 0xE6, 0xE6, 0xFF,
  0xE6, 0x33, 0x33, 0xE6,
  0xE6, 0x33, 0x33, 0xE6,
  0xFF, 0xE6, 0xE6, 0xFF,
};

#elif LINE_THICKNESS == 3

constexpr KDCoordinate thickCircleDiameter = 3;
constexpr KDCoordinate thickStampSize = thickCircleDiameter+1;
const uint8_t thickStampMask[(thickStampSize+1)*(thickStampSize+1)] = {
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0x7A, 0x0C, 0x7A, 0xFF,
  0xFF, 0x0C, 0x00, 0x0C, 0xFF,
  0xFF, 0x7A, 0x0C, 0x7A, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

#elif LINE_THICKNESS == 5

constexpr KDCoordinate thickCircleDiameter = 5;
constexpr KDCoordinate thickStampSize = thickCircleDiameter+1;
const uint8_t thickStampMask[(thickStampSize+1)*(thickStampSize+1)] = {
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xE1, 0x45, 0x0C, 0x45, 0xE1, 0xFF,
  0xFF, 0x45, 0x00, 0x00, 0x00, 0x45, 0xFF,
  0xFF, 0x0C, 0x00, 0x00, 0x00, 0x0C, 0xFF,
  0xFF, 0x45, 0x00, 0x00, 0x00, 0x45, 0xFF,
  0xFF, 0xE1, 0x45, 0x0C, 0x45, 0xE1, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};

#endif

constexpr static int k_maxNumberOfIterations = 10;

void CurveView::drawCurve(KDContext * ctx, KDRect rect, float tStart, float tEnd, float tStep, EvaluateXYForParameter xyEvaluation, void * model, void * context, bool drawStraightLinesEarly, KDColor color, bool thick, bool colorUnderCurve, float colorLowerBound, float colorUpperBound) const {
  float previousT = NAN;
  float t = NAN;
  float previousX = NAN;
  float x = NAN;
  float previousY = NAN;
  float y = NAN;
  int i = 0;
  do {
    previousT = t;
    t = tStart + (i++) * tStep;
    if (t <= tStart) {
      t = tStart + FLT_EPSILON;
    }
    if (t >= tEnd) {
      t = tEnd - FLT_EPSILON;
    }
    if (previousT == t) {
      break;
    }
    previousX = x;
    previousY = y;
    Coordinate2D<float> xy = xyEvaluation(t, model, context);
    x = xy.x1();
    y = xy.x2();
    if (colorUnderCurve && !std::isnan(x) && colorLowerBound < x && x < colorUpperBound && !(std::isnan(y) || std::isinf(y))) {
      drawHorizontalOrVerticalSegment(ctx, rect, Axis::Vertical, x, std::min(0.0f, y), std::max(0.0f, y), color, 1);
    }
    joinDots(ctx, rect, xyEvaluation, model, context, drawStraightLinesEarly, previousT, previousX, previousY, t, x, y, color, thick, k_maxNumberOfIterations);
  } while (true);
}

void CurveView::drawCartesianCurve(KDContext * ctx, KDRect rect, float xMin, float xMax, EvaluateXYForParameter xyEvaluation, void * model, void * context, KDColor color, bool thick, bool colorUnderCurve, float colorLowerBound, float colorUpperBound) const {
  float rectLeft = pixelToFloat(Axis::Horizontal, rect.left() - k_externRectMargin);
  float rectRight = pixelToFloat(Axis::Horizontal, rect.right() + k_externRectMargin);
  float tStart = std::isnan(rectLeft) ? xMin : std::max(xMin, rectLeft);
  float tEnd = std::isnan(rectRight) ? xMax : std::min(xMax, rectRight);
  assert(!std::isnan(tStart) && !std::isnan(tEnd));
  if (std::isinf(tStart) || std::isinf(tEnd) || tStart > tEnd) {
    return;
  }
  float tStep = pixelWidth();
  drawCurve(ctx, rect, tStart, tEnd, tStep, xyEvaluation, model, context, true, color, thick, colorUnderCurve, colorLowerBound, colorUpperBound);
}

void CurveView::drawHistogram(KDContext * ctx, KDRect rect, EvaluateYForX yEvaluation, void * model, void * context, float firstBarAbscissa, float barWidth,
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
    float y = yEvaluation(centerX, model, context);
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

void CurveView::joinDots(KDContext * ctx, KDRect rect, EvaluateXYForParameter xyEvaluation , void * model, void * context, bool drawStraightLinesEarly, float t, float x, float y, float s, float u, float v, KDColor color, bool thick, int maxNumberOfRecursion) const {
  const bool isFirstDot = std::isnan(t);
  const bool isLeftDotValid = !(
      std::isnan(x) || std::isinf(x) ||
      std::isnan(y) || std::isinf(y));
  const bool isRightDotValid = !(
      std::isnan(u) || std::isinf(u) ||
      std::isnan(v) || std::isinf(v));
  float pxf = floatToPixel(Axis::Horizontal, x);
  float pyf = floatToPixel(Axis::Vertical, y);
  float puf = floatToPixel(Axis::Horizontal, u);
  float pvf = floatToPixel(Axis::Vertical, v);
  if (!isRightDotValid && !isLeftDotValid) {
    return;
  }
  KDCoordinate circleDiameter = thick ? thickCircleDiameter : thinCircleDiameter;
  if (isRightDotValid) {
    const float deltaX = pxf - puf;
    const float deltaY = pyf - pvf;
    if (isFirstDot // First dot has to be stamped
       || (!isLeftDotValid && maxNumberOfRecursion == 0) // Last step of the recursion with an undefined left dot: we stamp the last right dot
       || (isLeftDotValid && deltaX*deltaX + deltaY*deltaY < circleDiameter * circleDiameter / 4.0f)) { // the dots are already close enough
      // the dots are already joined
      stampAtLocation(ctx, rect, puf, pvf, color, thick);
      return;
    }
  }
  // Middle point
  float ct = (t + s)/2.0f;
  Coordinate2D<float> cxy = xyEvaluation(ct, model, context);
  float cx = cxy.x1();
  float cy = cxy.x2();
  if ((drawStraightLinesEarly || maxNumberOfRecursion == 0) && isRightDotValid && isLeftDotValid &&
      ((x <= cx && cx <= u) || (u <= cx && cx <= x)) && ((y <= cy && cy <= v) || (v <= cy && cy <= y))) {
    /* As the middle dot is between the two dots, we assume that we
     * can draw a 'straight' line between the two */
    straightJoinDots(ctx, rect, pxf, pyf, puf, pvf, color, thick);
    return;
  }
  if (maxNumberOfRecursion > 0) {
    joinDots(ctx, rect, xyEvaluation, model, context, drawStraightLinesEarly, t, x, y, ct, cx, cy, color, thick, maxNumberOfRecursion-1);
    joinDots(ctx, rect, xyEvaluation, model, context, drawStraightLinesEarly, ct, cx, cy, s, u, v, color, thick, maxNumberOfRecursion-1);
  }
}

static void clipBarycentricCoordinatesBetweenBounds(float & start, float & end, const KDCoordinate * bounds, const float p1f, const float p2f) {
  static constexpr int lower = 0;
  static constexpr int upper = 1;
  if (p1f == p2f) {
    if (p1f < bounds[lower] || bounds[upper] < p1f) {
      start = 1;
      end = 0;
    }
  } else {
    start = std::max(start, (bounds[(p1f > p2f) ? lower : upper] - p2f)/(p1f-p2f));
    end   = std::min( end , (bounds[(p1f > p2f) ? upper : lower] - p2f)/(p1f-p2f));
  }
}

void CurveView::straightJoinDots(KDContext * ctx, KDRect rect, float pxf, float pyf, float puf, float pvf, KDColor color, bool thick) const {
  {
    /* Before drawing the line segment, clip it to rect:
     * start and end are the barycentric coordinates on the line segment (0
     * corresponding to (u, v) and 1 to (x, y)), of the drawing start and end
     * points. */
    float start = 0;
    float end   = 1;
    KDCoordinate stampSize = thick ? thickStampSize : thinStampSize;
    const KDCoordinate xBounds[2] = {
      static_cast<KDCoordinate>(rect.left() - stampSize),
      static_cast<KDCoordinate>(rect.right() + stampSize)
    };
    const KDCoordinate yBounds[2] = {
      static_cast<KDCoordinate>(rect.top() - stampSize),
      static_cast<KDCoordinate>(rect.bottom() + stampSize)
    };
    clipBarycentricCoordinatesBetweenBounds(start, end, xBounds, pxf, puf);
    clipBarycentricCoordinatesBetweenBounds(start, end, yBounds, pyf, pvf);
    if (start > end) {
      return;
    }
    puf = start * pxf + (1-start) * puf;
    pvf = start * pyf + (1-start) * pvf;
    pxf =  end  * pxf + (1- end ) * puf;
    pyf =  end  * pyf + (1- end ) * pvf;
  }
  const float deltaX = pxf - puf;
  const float deltaY = pyf - pvf;
  KDCoordinate circleDiameter = thick ? thickCircleDiameter : thinCircleDiameter;
  const float normsRatio = std::sqrt(deltaX*deltaX + deltaY*deltaY) / (circleDiameter / 2.0f);
  const float stepX = deltaX / normsRatio ;
  const float stepY = deltaY / normsRatio;
  const int numberOfStamps = std::floor(normsRatio);
  for (int i = 0; i < numberOfStamps; i++) {
    stampAtLocation(ctx, rect, puf, pvf, color, thick);
    puf += stepX;
    pvf += stepY;
  }
}

void CurveView::stampAtLocation(KDContext * ctx, KDRect rect, float pxf, float pyf, KDColor color, bool thick) const {
  /* The (pxf, pyf) coordinates are not generally locating the center of a
   * pixel. We use stampMask, which is one pixel wider and higher than
   * stampSize, in order to cover stampRect without aligning the pixels. Then
   * shiftedMask is computed so that each pixel is the average of the values of
   * the four pixels of stampMask by which it is covered, proportionally to the
   * area of the intersection with each of those.
   *
   * In order to compute the coordinates (px, py) of the top-left pixel of
   * stampRect, we consider that stampMask is centered at the provided point
   * (pxf,pyf) which is then translated to the center of the top-left pixel of
   * stampMask.
   */
  KDCoordinate stampSize = thick ? thickStampSize : thinStampSize;
  const uint8_t * stampMask = thick ? thickStampMask : thinStampMask;
  pxf -= (stampSize + 1 - 1)/2.0f;
  pyf -= (stampSize + 1 - 1)/2.0f;
  const KDCoordinate px = std::ceil(pxf);
  const KDCoordinate py = std::ceil(pyf);
  KDRect stampRect(px, py, stampSize, stampSize);
  if (!rect.intersects(stampRect)) {
    return;
  }
  uint8_t shiftedMask[stampSize][stampSize];
  KDColor workingBuffer[stampSize*stampSize];
  const float dx = px - pxf;
  const float dy = py - pyf;
  /* TODO: this could be optimized by precomputing 10 or 100 shifted masks. The
   * dx and dy would be rounded to one tenth or one hundredth to choose the
   * right shifted mask. */
  const KDCoordinate stampMaskSize = stampSize + 1;
  for (int i=0; i<stampSize; i++) {
    for (int j=0; j<stampSize; j++) {
      shiftedMask[j][i] = (1.0f - dx) * (stampMask[j*stampMaskSize+i]*(1.0-dy)+stampMask[(j+1)*stampMaskSize+i]*dy)
        + dx * (stampMask[j*stampMaskSize+(i+1)]*(1.0f-dy) + stampMask[(j+1)*stampMaskSize+(i+1)]*dy);
    }
  }
  ctx->blendRectWithMask(stampRect, color, (const uint8_t *)shiftedMask, workingBuffer);
}

void CurveView::layoutSubviews(bool force) {
  if (m_curveViewCursor != nullptr && m_cursorView != nullptr) {
    m_cursorView->setCursorFrame(cursorFrame(), force);
  }
  if (m_bannerView != nullptr) {
    m_bannerView->setFrame(bannerFrame(), force);
  }
  if (m_okView != nullptr) {
    m_okView->setFrame(okFrame(), force);
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
    assert(bounds().width() == Ion::Display::Width); // Else the bannerHeight will not be properly computed
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
    PrintFloat::ConvertFloatToText<float>(
        labelValueAtIndex(axis, i),
        label(axis, i),
        k_labelBufferMaxSize,
        labelMaxGlyphLengthSize(),
        increaseNumberOfSignificantDigits ? k_bigNumberSignificantDigits : k_numberSignificantDigits,
        Preferences::PrintFloatMode::Decimal);
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
