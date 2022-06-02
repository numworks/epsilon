#include "curve_view.h"
#include "../constant.h"
#include "dots.h"
#include <poincare/print_float.h>
#include <assert.h>
#include <string.h>
#include <algorithm>
#include <cmath>
#include <float.h>
#include <complex>
#include <poincare/trigonometry.h>

using namespace Escher;
using namespace Poincare;

namespace Shared {

CurveView::CurveView(CurveViewRange * curveViewRange, CurveViewCursor * curveViewCursor, BannerView * bannerView,
    CursorView * cursorView) :
  View(),
  m_bannerView(bannerView),
  m_curveViewCursor(curveViewCursor),
  m_curveViewRange(curveViewRange),
  m_cursorView(cursorView),
  m_mainViewSelected(false),
  m_drawnRangeVersion(0)
{
}

void CurveView::reload(bool resetInterrupted, bool force) {
  uint32_t rangeVersion = m_curveViewRange->rangeChecksum();
  if (force || m_drawnRangeVersion != rangeVersion) {
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

/* When setting cursor or banner we first dirty the former element
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
  assert(m_curveViewRange->xMax() != m_curveViewRange->xMin());
  return (m_curveViewRange->xMax() - m_curveViewRange->xMin()) / (m_frame.width() - 1);
}

float CurveView::pixelHeight() const {
  assert(m_curveViewRange->yMax() != m_curveViewRange->yMin());
  return (m_curveViewRange->yMax() - m_curveViewRange->yMin()) / (m_frame.height() - 1);
}

float CurveView::pixelLength(Axis axis) const {
  return axis == Axis::Horizontal ? pixelWidth() : pixelHeight();
}

float CurveView::pixelToFloat(Axis axis, KDCoordinate p) const {
  return (axis == Axis::Horizontal) ?
    m_curveViewRange->xMin() + p * pixelWidth() :
    m_curveViewRange->yMax() - p * pixelHeight();
}

static float clippedFloat(float f) {
  /* Make sure that the returned value is between the maximum and minimum
   * possible values of KDCoordinate. */
  if (f == NAN) {
    return NAN;
  } else if (f < KDCOORDINATE_MIN) {
    return KDCOORDINATE_MIN;
  } else if (f > KDCOORDINATE_MAX) {
    return KDCOORDINATE_MAX;
  } else {
    return f;
  }
}

float CurveView::floatToPixel(Axis axis, float f) const {
  float result = (axis == Axis::Horizontal) ?
    (f - m_curveViewRange->xMin()) / pixelWidth() :
    (m_curveViewRange->yMax() - f) / pixelHeight();
  return clippedFloat(result);
}

float CurveView::floatLengthToPixelLength(Axis axis, float f) const {
  float dist = f / pixelLength(axis);
  return clippedFloat(dist);
}

float CurveView::floatLengthToPixelLength(float dx, float dy) const {
  float dxPixel = floatLengthToPixelLength(Axis::Horizontal, dx);
  float dyPixel = floatLengthToPixelLength(Axis::Vertical, dy);
  return std::sqrt(dxPixel*dxPixel+dyPixel*dyPixel);
}

float CurveView::pixelLengthToFloatLength(Axis axis, float f) const {
  return f*pixelLength(axis);
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
  float step = labelStep(axis);
  if (step <= 0.0f) {
    return 0;
  }
  float minLabel = std::ceil(min(axis)/step);
  float maxLabel = std::floor(max(axis)/step);
  int numberOfLabels = maxLabel - minLabel + 1;
  assert(numberOfLabels <= (axis == Axis::Horizontal ? k_maxNumberOfXLabels : k_maxNumberOfYLabels));
  return numberOfLabels;
}

void CurveView::computeLabels(Axis axis) {
  float step = gridUnit(axis);
  int axisLabelsCount = numberOfLabels(axis);
  for (int i = 0; i < axisLabelsCount; i++) {
    float labelValue = labelValueAtIndex(axis, i);
    /* Label cannot hold more than labelMaxGlyphLength characters to prevent
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

    char * labelBuffer = label(axis, i);

    int glyphLength = PrintFloat::ConvertFloatToText<float>(
        labelValue,
        labelBuffer,
        k_labelBufferMaxSize,
        labelMaxGlyphLength,
        k_numberSignificantDigits,
        Preferences::PrintFloatMode::Decimal).GlyphLength;

    // Some implementations may need to append a suffix to the label
    appendLabelSuffix(axis, labelBuffer, k_labelBufferMaxSize, glyphLength, labelMaxGlyphLength);

    if (axis == Axis::Horizontal) {
      if (labelBuffer[0] == 0) {
        /* Some labels are too big and may overlap their neighbors. We write the
         * extrema labels only. */
        computeHorizontalExtremaLabels();
        break;
      }
      if (i > 0 && strcmp(labelBuffer, label(axis, i-1)) == 0) {
        /* We need to increase the number if significant digits, otherwise some
         * labels are rounded to the same value. */
        computeHorizontalExtremaLabels(true);
        break;
      }
    }
  }
  int maxNumberOfLabels = (axis == Axis::Horizontal ? k_maxNumberOfXLabels : k_maxNumberOfYLabels);
  // All remaining labels are empty. They shouldn't be accessed anyway.
  for (int i = axisLabelsCount; i < maxNumberOfLabels; i++) {
    label(axis, i)[0] = 0;
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

  assert(!(bannerIsVisible() && m_bannerView->bounds().isEmpty()));
  KDCoordinate viewHeight = bounds().height() - (bannerIsVisible() ? m_bannerView->bounds().height() : 0);

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

  float step = labelStep(axis);
  int minLabelPixelPosition = std::round(floatToPixel(axis, step * std::ceil(min(axis)/step)));
  int maxLabelPixelPosition = std::round(floatToPixel(axis, step * std::floor(max(axis)/step)));

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
      float labelValue = labelValueAtIndex(axis, i);
      if (shouldDrawLabelAtPosition(labelValue)) {
        KDCoordinate labelPosition = std::round(floatToPixel(axis, labelValue));
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
  }

  if (graduationOnly) {
    return;
  }

  // Labels will be pulled. They must be up to date with current curve view.
  assert(m_drawnRangeVersion == m_curveViewRange->rangeChecksum());

  // Draw the labels
  for (int i = minDrawnLabel; i < maxDrawnLabel; i++) {
    float labelValue = labelValueAtIndex(axis, i);
    if (shouldDrawLabelAtPosition(labelValue)) {
      KDCoordinate labelPosition = std::round(floatToPixel(axis, labelValue));
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
}

void CurveView::drawHorizontalOrVerticalSegment(KDContext * ctx, KDRect rect, Axis axis, float coordinate, float lowerBound, float upperBound, KDColor color, KDCoordinate thickness, KDCoordinate dashSize, int areaPattern) const {
  if (lowerBound > upperBound) {
    // Swap lower and upper bounds
    float tempBound = lowerBound;
    lowerBound = upperBound;
    upperBound = tempBound;
  }
  KDCoordinate min = (axis == Axis::Horizontal) ? rect.x() : rect.y();
  KDCoordinate max = (axis == Axis::Horizontal) ? rect.x() + rect.width() : rect.y() + rect.height();
  // Alongside the y-axis, upperBound correspond to the start
  float startBound = (axis == Axis::Horizontal) ? lowerBound : upperBound;
  float endBound = (axis == Axis::Horizontal) ? upperBound : lowerBound;
  KDCoordinate start = std::isinf(startBound) ? min : std::round(floatToPixel(axis, startBound));
  start = start < min ? min : start;
  KDCoordinate end = std::isinf(endBound) ? max : std::round(floatToPixel(axis, endBound));
  end = end > max ? max : end;
  if (start > end) {
    // Segment was out of rect
    return;
  }
  Axis otherAxis = (axis == Axis::Horizontal) ? Axis::Vertical : Axis::Horizontal;
  KDCoordinate pixelCoordinate = std::round(floatToPixel(otherAxis, coordinate));
  KDCoordinate otherMin = (axis == Axis::Vertical) ? rect.x() : rect.y();
  KDCoordinate otherMax = (axis == Axis::Vertical) ? rect.x() + rect.width() : rect.y() + rect.height();
  if (pixelCoordinate + thickness <= otherMin || pixelCoordinate >= otherMax) {
    // Nothing will intersect
    return;
  }

  if (areaPattern >= 0) {
    assert(pixelCoordinate >= 0 && dashSize == 1);
    for (int area = 0; area < k_numberOfPatternAreas; area++) {
      if (areaPattern & (1 << area)) {
        drawPatternAreaInLine(ctx, axis, pixelCoordinate, start, end, area, color);
      }
    }
    return;
  }

  if (dashSize < 0) {
    // Continuous segment is equivalent to one big dash
    dashSize = end - start;
  }
  for (KDCoordinate i = start; i < end; i += 2*dashSize) {
    KDRect lineRect = (axis == Axis::Horizontal) ? KDRect(i, pixelCoordinate, dashSize, thickness) :  KDRect(pixelCoordinate, i, thickness, dashSize);
    assert(rect.intersects(lineRect));
    ctx->fillRect(lineRect, color);
  }
}

void CurveView::drawPatternAreaInLine(KDContext * ctx, Axis axis, KDCoordinate coordinate, KDCoordinate start, KDCoordinate end, int areaIndex, KDColor color) const {
  /* Draw an inequation area. Up to four areas are handled. Otherwise, they
   * override. */
  /* Inequation Area is tiled over 4x4 blocks of pixels.
   * Depending on the areaIndex, the colorized pixel will be :
   *
   * Segment:    Tile:              Example:
   *    0        0 # 1 #         0 # 1 # 0 # 1 # 0 # 1 # 0 # 1 #
   *    #   ->   # 2 # 3  ->     # 2 # 3 # 2 # 3 # 2 # 3 # 2 # 3
   *    1        1 # 0 #         1 # 0 # 1 # 0 # 1 # 0 # 1 # 0 #
   *    #        # 3 # 2         # 3 # 2 # 3 # 2 # 3 # 2 # 3 # 2
   *
   * Pixels' positions are absolute. Drawn independently, all segments shall
   * align. */
  constexpr int evenOddIndex[4] = {0,0,1,1};
  // Depending on the areaIndex, either even or odd lines are drawn
  if ((coordinate + evenOddIndex[areaIndex])%2 == 0) {
    // Compute the offset (modulo 4) to respect the tile's requirements
    // - Offset due to areaIndex
    constexpr int areaOffset[4] = {0,2,0,2};
    int offset = areaOffset[areaIndex];
    // - Offset due to each subsequent segment shifting by 1
    offset += coordinate%4;
    // - Offset to make the segment alignment absolute (indepandent of start)
    offset += 4 - start%4;
    for (KDCoordinate i = start + offset%4; i < end; i += 4) {
      KDPoint point = (axis == Axis::Horizontal) ? KDPoint(i, coordinate) :  KDPoint(coordinate, i);
      ctx->setPixel(point, color);
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
    case Size::Tiny:
      diameter = Dots::TinyDotDiameter;
      mask = (const uint8_t *)Dots::TinyDotMask;
      break;
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
  /* If circle has an even diameter, out of the four center pixels, the bottom
   * left one will be placed at (x, y) */
  KDCoordinate px = std::round(floatToPixel(Axis::Horizontal, x)) - (diameter-1)/2;
  KDCoordinate py = std::round(floatToPixel(Axis::Vertical, y)) - diameter/2;
  KDRect dotRect(px, py, diameter, diameter);
  if (!rect.intersects(dotRect)) {
    return;
  }
  KDColor workingBuffer[Dots::LargeDotDiameter*Dots::LargeDotDiameter];
  ctx->blendRectWithMask(dotRect, color, mask, workingBuffer);
}


void CurveView::drawArrow(KDContext * ctx, KDRect rect, float x, float y, float dx, float dy, KDColor color, float arrowWidth, float tanAngle) const {
  assert(tanAngle >= 0.0f);
  if (std::fabs(dx) < FLT_EPSILON && std::fabs(dy) < FLT_EPSILON) {
    // We can't draw an arrow without any orientation
    return;
  }

  // Translate arrowWidth in pixel length
  float pixelArrowWidth = 8.0f; // default value in pixels
  if (arrowWidth > 0.0f) {
    float dxdyFloat = std::sqrt(dx * dx + dy * dy);
    float dxArrowFloat = arrowWidth * std::fabs(dy) / dxdyFloat;
    float dyArrowFloat = arrowWidth * std::fabs(dx) / dxdyFloat;
    pixelArrowWidth = floatLengthToPixelLength(dxArrowFloat, dyArrowFloat);
    assert(pixelArrowWidth > 0.0f);
  }

  /* Let's call the following variables L and l:
   *
   *            /arrow2            |
   *          /                    |
   *        /                      l
   *      /                        |
   *    /        B                 |
   *  <---------+----------------------------------------
   *    \
   *      \
   *        \
   *          \
   *            \arrow1
   *
   * ----- L -----
   *
   */

  float lPixel = pixelArrowWidth / 2.0;
  float LPixel = lPixel / tanAngle;

  float xPixel = floatToPixel(Axis::Horizontal, x);
  float yPixel = floatToPixel(Axis::Vertical, y);

  // We compute the arrow segments in pixels
  float dxPixel = floatLengthToPixelLength(Axis::Horizontal, dx);
  float dyPixel = floatLengthToPixelLength(Axis::Vertical, dy);
  float dx2dy2Pixel = floatLengthToPixelLength(dx, dy);

  // Point B is the orthogonal projection of the arrow tips on the arrow body
  float bxPixel = xPixel - LPixel * dxPixel / dx2dy2Pixel;
  float byPixel = yPixel + LPixel * dyPixel / dx2dy2Pixel;

  float dxArrowPixel = - lPixel * dyPixel / dx2dy2Pixel;
  float dyArrowPixel = lPixel * dxPixel / dx2dy2Pixel;

  float arrow1xPixel = bxPixel + dxArrowPixel;
  float arrow1yPixel = byPixel - dyArrowPixel;
  float arrow2xPixel = bxPixel - dxArrowPixel;
  float arrow2yPixel = byPixel + dyArrowPixel;

  straightJoinDots(ctx, rect, xPixel, yPixel, arrow1xPixel, arrow1yPixel, color, true);
  straightJoinDots(ctx, rect, xPixel, yPixel, arrow2xPixel, arrow2yPixel, color, true);
}

void CurveView::drawGrid(KDContext * ctx, KDRect rect) const {
  KDColor boldColor = Palette::GrayMiddle;
  KDColor lightColor = Palette::GrayWhite;
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

void CurveView::drawCurve(KDContext * ctx, KDRect rect, const float tStart, float tEnd, const float tStep, EvaluateXYForFloatParameter xyFloatEvaluation, void * model, void * context, bool drawStraightLinesEarly, KDColor color, bool thick, bool colorUnderCurve, KDColor colorOfFill, float colorLowerBound, float colorUpperBound, EvaluateXYForDoubleParameter xyDoubleEvaluation, bool dashedCurve, EvaluateXYForFloatParameter xyAreaBound, bool shouldColorAreaWhenNan, int areaPattern, Axis axis) const {
  /* ContinuousFunction caching relies on a consistent tStart and tStep. These
   * values shouldn't be altered here. */
  float previousT = NAN;
  float t = NAN;
  float previousX = NAN;
  float x = NAN;
  float previousY = NAN;
  float y = NAN;
  int i = 0;
  bool isLastSegment = false;
  int stampNumber = 0;
  do {
    previousT = t;
    t = tStart + (i++) * tStep;
    if (t <= tStart) {
      t = tStart + FLT_EPSILON;
    }
    if (t >= tEnd) {
      t = tEnd - FLT_EPSILON;
      isLastSegment = true;
    }
    if (previousT == t) {
      // No need to draw segment. Happens when tStep << tStart .
      continue;
    }
    previousX = x;
    previousY = y;
    Coordinate2D<float> xy = xyFloatEvaluation(t, model, context);
    x = xy.x1();
    y = xy.x2();
    float mainCoordinate = axis == Axis::Horizontal ? x : y;
    float secondaryCoordinate = axis == Axis::Horizontal ? y : x;
    Axis secondaryAxis = axis == Axis::Horizontal ? Axis::Vertical : Axis::Horizontal;
    if (colorUnderCurve && !std::isnan(mainCoordinate) && colorLowerBound < mainCoordinate && mainCoordinate < colorUpperBound && !(std::isnan(secondaryCoordinate) || std::isinf(secondaryCoordinate))) {
      drawHorizontalOrVerticalSegment(ctx, rect, secondaryAxis, mainCoordinate, std::min(0.0f, secondaryCoordinate), std::max(0.0f, secondaryCoordinate), colorOfFill, 1, 1, areaPattern);
    }
    if (xyAreaBound && (shouldColorAreaWhenNan || !std::isnan(secondaryCoordinate))) {
      Coordinate2D<float> xyArea = xyAreaBound(t, model, context);
      float areaBound = axis == Axis::Horizontal ? xyArea.x2() : xyArea.x1();
      if (shouldColorAreaWhenNan || !std::isnan(areaBound)) {
        float lowerBound = std::isnan(secondaryCoordinate) || std::isnan(areaBound) ? -INFINITY : std::min(secondaryCoordinate, areaBound);
        float upperBound = std::isnan(secondaryCoordinate) || std::isnan(areaBound) ? INFINITY : std::max(secondaryCoordinate, areaBound);
        drawHorizontalOrVerticalSegment(ctx, rect, secondaryAxis, mainCoordinate, lowerBound, upperBound, colorOfFill, 1, 1, areaPattern);
      }
    }
    stampNumber = joinDots(ctx, rect, xyFloatEvaluation, model, context, drawStraightLinesEarly, previousT, previousX, previousY, t, x, y, color, thick, k_maxNumberOfIterations, xyDoubleEvaluation, dashedCurve, stampNumber);
  } while (!isLastSegment);
}

void CurveView::drawCartesianCurve(KDContext * ctx, KDRect rect, float tMin, float tMax, EvaluateXYForFloatParameter xyFloatEvaluation, void * model, void * context, KDColor color, bool thick, bool colorUnderCurve, KDColor colorOfFill, float colorLowerBound, float colorUpperBound, EvaluateXYForDoubleParameter xyDoubleEvaluation, bool dashedCurve, EvaluateXYForFloatParameter xyAreaBound, bool shouldColorAreaWhenNan, int areaPattern, float cachedTStep, Axis axis) const {
  float tStart = tMin;
  float tStep = cachedTStep;
  KDCoordinate pixelMin = axis == Axis::Horizontal ? rect.left() - k_externRectMargin : rect.bottom() + k_externRectMargin;
  KDCoordinate pixelMax = axis == Axis::Horizontal ? rect.right() + k_externRectMargin : rect.top() - k_externRectMargin;
  if (cachedTStep == 0) {
    /* cachedTStep isn't given, there are no constraints with cache parameters.
     * We can then compute tStep and clip tStart. */
    float rectMin = pixelToFloat(axis, pixelMin);
    tStart = std::isnan(rectMin) ? tMin : std::max(tMin, rectMin);
    tStep = axis == Axis::Horizontal ? pixelWidth() : pixelHeight();
    // Round to pixel perfect position to avoid landing in the middle of pixels
    tStart = roundFloatToPixelPerfect(axis, tStart);
    colorLowerBound = roundFloatToPixelPerfect(axis, colorLowerBound);
  }
  float rectMax = pixelToFloat(axis, pixelMax);
  float tEnd = std::isnan(rectMax) ? tMax : std::min(tMax, rectMax);
  assert(!std::isnan(tStart) && !std::isnan(tEnd));
  if (std::isinf(tStart) || std::isinf(tEnd) || tStart > tEnd) {
    return;
  }
  drawCurve(ctx, rect, tStart, tEnd, tStep, xyFloatEvaluation, model, context, true, color, thick, colorUnderCurve, colorOfFill, colorLowerBound, colorUpperBound, xyDoubleEvaluation, dashedCurve, xyAreaBound, shouldColorAreaWhenNan, areaPattern, axis);
}

float PolarThetaFromCoordinates(float x, float y, Preferences::AngleUnit angleUnit) {
  // Return θ, between -π and π in given angleUnit for a (x,y) position.
  return Trigonometry::ConvertRadianToAngleUnit<float>(std::arg(std::complex<float>(x,y)), angleUnit).real();
}

void CurveView::drawPolarCurve(KDContext * ctx, KDRect rect, float tStart, float tEnd, float tStep, EvaluateXYForFloatParameter xyFloatEvaluation, void * model, void * context, bool drawStraightLinesEarly, KDColor color, bool thick, bool colorUnderCurve, float colorLowerBound, float colorUpperBound, EvaluateXYForDoubleParameter xyDoubleEvaluation) const {
  // Compute rect limits
  float rectLeft = pixelToFloat(Axis::Horizontal, rect.left() - k_externRectMargin);
  float rectRight = pixelToFloat(Axis::Horizontal, rect.right() + k_externRectMargin);
  float rectUp = pixelToFloat(Axis::Vertical, rect.top() - k_externRectMargin);
  float rectDown = pixelToFloat(Axis::Vertical, rect.bottom() + k_externRectMargin);

  const Preferences::AngleUnit angleUnit = Preferences::sharedPreferences()->angleUnit();
  const float piInAngleUnit = Trigonometry::PiInAngleUnit(angleUnit);
  /* Cancel optimization if :
   * - One of rect limits is nan.
   * - Step is too large, see cache optimization comments
   *   ("To optimize cache..."). */
  bool cancelOptimization = std::isnan(rectLeft + rectRight + rectUp + rectDown) || tStep >= piInAngleUnit;

  bool rectOverlapsNegativeAbscissaAxis = false;
  if (cancelOptimization || (rectUp > 0.0f && rectDown < 0.0f && rectLeft < 0.0f)) {
    if (cancelOptimization || rectRight > 0.0f) {
      // Origin is inside rect, tStart and tEnd cannot be optimized
      return drawCurve(ctx, rect, tStart, tEnd, tStep, xyFloatEvaluation, model, context, drawStraightLinesEarly, color, thick, colorUnderCurve, color, colorLowerBound, colorUpperBound, xyDoubleEvaluation);
    }
    // Rect view overlaps the abscissa, on the left of the origin.
    rectOverlapsNegativeAbscissaAxis = true;
  }

  float tMin, tMax;
  /* Compute angular coordinate of each corners of rect.
   * t3 --- t2
   *  |      |
   * t4 --- t1 */
  float t1 = PolarThetaFromCoordinates(rectRight, rectDown, angleUnit);
  float t2 = PolarThetaFromCoordinates(rectRight, rectUp, angleUnit);
  if (!rectOverlapsNegativeAbscissaAxis) {
    float t3 = PolarThetaFromCoordinates(rectLeft, rectUp, angleUnit);
    float t4 = PolarThetaFromCoordinates(rectLeft, rectDown, angleUnit);
    /* The area between tMin and tMax (modulo π) is the only area where
     * something needs to be plotted. */
    tMin = std::min({t1, t2, t3, t4});
    tMax = std::max({t1, t2, t3, t4});
  } else {
    /* PolarThetaFromCoordinates yields coordinates between -π and π. When rect
     * is overlapping the negative abscissa (at this point, the origin cannot be
     * inside rect), t1 and t4 have a negative angle whereas t2 and t3 have a
     * positive angle. We ensure here that tMin is t2 (modulo 2π), tMax is t1,
     * and that tMax-tMin is minimal and positive. */
    tMin = t2 - 2 * piInAngleUnit;
    tMax = t1;
  }

  // Add a thousandth of π as a margin to avoid visible approximation errors.
  tMax += piInAngleUnit / 1000.0f;
  tMin -= piInAngleUnit / 1000.0f;

  /* To optimize cache hits, the area actually drawn will be extended to nearest
   * cached θ. tStep being a multiple of cache steps (see
   * ComputeNonCartesianSteps), we extend segments on both ends to the closest
   * θ = tStart + tStep * i
   * If the drawn segment is extended too much, it might overlap with the next
   * extended segment.
   * For example, with * the segments that must be drawn and piInAngleUnit=7 :
   *                 tStart                                            tEnd
   *              kπ   | (k+1)π  (k+2)π  (k+3)π  (k+4)π  (k+5)π  (k+6)π  |(k+7)π
   *               |-------|-------|-------|-------|-------|-------|-------|--
   * tMax-tMin=3 : |---***-|---***-|---***-|---***-|---***-|---***-|---***-|--
   * A - tStep=3 : |---***-|---***-|---***-|---***-|---***-|---***-|---***-|--
   *               |___^^^_|__     | ^^^^^^|___   _|__^^^^^|^      |___^^^_|__
   *               |       |  ^^^^^|^      |   ^^^ |       | ^^^^^^|       |
   *
   * B - tStep=6 : |---***-|---***-|---***-|---***-|---***-|---***-|---***-|--
   *               |___^^^^|^^     | ^^^^^^|      ^|^^^^^^^|^^^^   |   ^^^^|^^
   *               |       |  ^^^^^|^      |^^^^^^ |     ^^|^^^^^^^|^^^    |
   * In situation A, Step are small enough, not all segments must be drawn.
   * In situation B, The entire range should be drawn, and two extended segments
   * overlap (at tStart+5*tStep). Optimization is useless.
   * If tStep < piInAngleUnit - (tMax - tMin), situation B cannot happen. */
  if (tStep >= piInAngleUnit - tMax + tMin) {
    return drawCurve(ctx, rect, tStart, tEnd, tStep, xyFloatEvaluation, model, context, drawStraightLinesEarly, color, thick, colorUnderCurve, color, colorLowerBound, colorUpperBound, xyDoubleEvaluation);
  }

  /* The number of segments to draw can be reduced by drawing curve on intervals
   * where (tMin%π, tMax%π) intersects (tStart, tEnd).For instance :
   * if tStart=-π, tEnd=3π, tMin=π/4 and tMax=π/3, a curve is drawn between :
   * - [ π/4, π/3 ], [ 2π + π/4, 2π + π/3 ]
   * - [ -π + π/4, -π + π/3 ], [ π + π/4, π + π/3 ] in case f(θ) is negative */

  // 1 - Set offset so that tStart <= tMax+thetaOffset < piInAngleUnit+tStart
  float thetaOffset = std::ceil((tStart - tMax)/piInAngleUnit) * piInAngleUnit;

  // 2 - Increase offset until tMin + thetaOffset > tEnd
  float tCache2 = tStart;
  while (tMin + thetaOffset <= tEnd) {
    float tS = std::max(tMin + thetaOffset, tStart);
    float tE = std::min(tMax + thetaOffset, tEnd);
    // Draw curve if there is an intersection
    if (tS <= tE) {
      /* To maximize cache hits, we floor (and ceil) tS (and tE) to the closest
       * cached value. Step is small enough so that the extra drawn curve cannot
       * overlap as tMax + tStep < piInAngleUnit + tMin) */
      int i = std::floor((tS - tStart) / tStep);
      assert(tStart + tStep * i >= tCache2);
      float tCache1 = tStart + tStep * i;

      int j = std::ceil((tE - tStart) / tStep);
      tCache2 = std::min(tStart + tStep * j, tEnd);

      assert(tCache1 <= tCache2);
      drawCurve(ctx, rect, tCache1, tCache2, tStep, xyFloatEvaluation, model, context, drawStraightLinesEarly, color, thick, colorUnderCurve, color, colorLowerBound, colorUpperBound, xyDoubleEvaluation);
    }
    thetaOffset += piInAngleUnit;
  }
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

static bool pointInBoundingBox(float x1, float y1, float x2, float y2, float xC, float yC) {
  return ((x1 < xC && xC < x2) || (x2 < xC && xC < x1) || (x2 == xC && xC == x1))
      && ((y1 < yC && yC < y2) || (y2 < yC && yC < y1) || (y2 == yC && yC == y1));
}

int CurveView::joinDots(KDContext * ctx, KDRect rect, EvaluateXYForFloatParameter xyFloatEvaluation , void * model, void * context, bool drawStraightLinesEarly, float t, float x, float y, float s, float u, float v, KDColor color, bool thick, int maxNumberOfRecursion, EvaluateXYForDoubleParameter xyDoubleEvaluation, bool dashedCurve, int stampNumber) const {
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
    return stampNumber;
  }
  KDCoordinate circleDiameter = thick ? thickCircleDiameter : thinCircleDiameter;
  if (isRightDotValid) {
    const float deltaX = pxf - puf;
    const float deltaY = pyf - pvf;
    if (isFirstDot // First dot has to be stamped
       || (!isLeftDotValid && maxNumberOfRecursion <= 0) // Last step of the recursion with an undefined left dot: we stamp the last right dot
       || (isLeftDotValid && deltaX*deltaX + deltaY*deltaY < circleDiameter * circleDiameter / 4.0f)) { // the dots are already close enough
      // the dots are already joined
      /* We need to be sure that the point is not an artifact caused by error
       * in float approximation. */
      float pvd = xyDoubleEvaluation ? floatToPixel(Axis::Vertical, static_cast<float>(xyDoubleEvaluation(s, model, context).x2())) : pvf;
      if (std::isnan(pvd)) {
        /* Double evaluation is NAN while float evaluation is not. It can happen
         * with precision sensitive functions such as (-1)^x. We stamp at pvf
         * anyway, instead of setting isRightDotValid to false and continuing
         * because the latter solution significantly slows down the graph. */
        pvd = pvf;
      }
      return stampAtLocation(ctx, rect, puf, pvd, color, thick, dashedCurve, stampNumber);
    }
  }
  // Middle point
  float ct = (t + s)/2.0f;
  Coordinate2D<float> cxy = xyFloatEvaluation(ct, model, context);
  float cx = cxy.x1();
  float cy = cxy.x2();
  if ((drawStraightLinesEarly || maxNumberOfRecursion <= 0) && isRightDotValid && isLeftDotValid &&
      pointInBoundingBox(x, y, u, v, cx, cy)) {
    /* As the middle dot is between the two dots, we assume that we
     * can draw a 'straight' line between the two */

    constexpr float dangerousSlope = 1e6f;
    if (xyDoubleEvaluation && std::fabs((v-y) / (u-x)) > dangerousSlope) {
      /* We need to make sure we're not drawing a vertical asymptote because of
       * rounding errors. */
      Coordinate2D<double> xyD = xyDoubleEvaluation(static_cast<double>(t), model, context);
      Coordinate2D<double> uvD = xyDoubleEvaluation(static_cast<double>(s), model, context);
      Coordinate2D<double> cxyD = xyDoubleEvaluation(static_cast<double>(ct), model, context);
      if (pointInBoundingBox(xyD.x1(), xyD.x2(), uvD.x1(), uvD.x2(), cxyD.x1(), cxyD.x2())) {
        return straightJoinDots(ctx, rect, floatToPixel(Axis::Horizontal, xyD.x1()), floatToPixel(Axis::Vertical, xyD.x2()), floatToPixel(Axis::Horizontal, uvD.x1()), floatToPixel(Axis::Vertical, uvD.x2()), color, thick, dashedCurve, stampNumber);
      }
    } else {
      return straightJoinDots(ctx, rect, pxf, pyf, puf, pvf, color, thick, dashedCurve, stampNumber);
    }
  }
  if (maxNumberOfRecursion > 0) {
    float xmin = min(Axis::Horizontal);
    float xmax = max(Axis::Horizontal);
    float ymax = max(Axis::Vertical);
    float ymin = min(Axis::Vertical);

    int nextMaxNumberOfRecursion = maxNumberOfRecursion - 1;
    // If both dots are out of rect bounds, and on a same side
    if ((xmax < x && xmax < u) || (x < xmin && u < xmin) ||
        (ymax < y && ymax < v) || (y < ymin && v < ymin)) {
      /* Discard a recursion step to save computation time on dots that are
       * likely not to be drawn. It can alter precision with some functions when
       * zooming excessively (compared to plot range) on local minimums
       * For instance, plotting parametric function [t,|t-π|] with t in [0,360],
       * x in [-1,20] and y in [-1,3] will show inaccuracies that would
       * otherwise have been visible at higher zoom only, with x in [2,4] and y
       * in [-0.2,0.2] in this case. */
      nextMaxNumberOfRecursion--;
    }

    stampNumber = joinDots(ctx, rect, xyFloatEvaluation, model, context, drawStraightLinesEarly, t, x, y, ct, cx, cy, color, thick, nextMaxNumberOfRecursion, xyDoubleEvaluation, dashedCurve, stampNumber);
    stampNumber = joinDots(ctx, rect, xyFloatEvaluation, model, context, drawStraightLinesEarly, ct, cx, cy, s, u, v, color, thick, nextMaxNumberOfRecursion, xyDoubleEvaluation, dashedCurve, stampNumber);
  }
  return stampNumber;
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

int CurveView::straightJoinDots(KDContext * ctx, KDRect rect, float pxf, float pyf, float puf, float pvf, KDColor color, bool thick, bool dashedCurve, int stampNumber) const {
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
      return stampNumber;
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
    stampNumber = stampAtLocation(ctx, rect, puf, pvf, color, thick, dashedCurve, stampNumber);
    puf += stepX;
    pvf += stepY;
  }
  return stampNumber;
}

bool ShouldDashedCurveStamp(int stampNumber) {
  // Stamp 5 times out of 10.
  return stampNumber%10 >= 5;
}

int CurveView::stampAtLocation(KDContext * ctx, KDRect rect, float pxf, float pyf, KDColor color, bool thick, bool dashedCurve, int stampNumber) const {
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
  assert(!std::isnan(pxf) && !std::isnan(pyf));
  if (dashedCurve && ShouldDashedCurveStamp(stampNumber)) {
    return stampNumber + 1;
  }
  KDCoordinate stampSize = thick ? thickStampSize : thinStampSize;
  const uint8_t * stampMask = thick ? thickStampMask : thinStampMask;
  pxf -= (stampSize + 1 - 1)/2.0f;
  pyf -= (stampSize + 1 - 1)/2.0f;
  const KDCoordinate px = std::ceil(pxf);
  const KDCoordinate py = std::ceil(pyf);
  KDRect stampRect(px, py, stampSize, stampSize);
  if (!rect.intersects(stampRect)) {
    return stampNumber + 1;
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
  return stampNumber + 1;
}

void CurveView::layoutSubviews(bool force) {
  if (m_curveViewCursor != nullptr && m_cursorView != nullptr) {
    m_cursorView->setCursorFrame(cursorFrame(), force);
  }
  if (m_bannerView != nullptr) {
    /* Changing the banner frame may move the labels of the horizontal axis. We
     * thus need to dirty the band that can contain them. */
    KDRect oldFrame = m_bannerView->bounds();
    m_bannerView->setFrame(bannerFrame(), force);
    if (!(m_bannerView->bounds() == oldFrame)) {
      const KDCoordinate dirtyHeight = std::max(oldFrame.height(), m_bannerView->bounds().height()) + k_font->glyphSize().height() + 2 * k_labelMargin;
      markRectAsDirty(KDRect(0, bounds().height() - dirtyHeight, bounds().width(), dirtyHeight));
    }
  }
}

KDRect CurveView::cursorFrame() {
  KDRect cursorFrame = KDRectZero;
  if (m_cursorView && m_mainViewSelected && std::isfinite(m_curveViewCursor->x()) && std::isfinite(m_curveViewCursor->y())) {
    KDSize cursorSize = m_cursorView->minimalSizeForOptimalDisplay();
    float xCursorPixelPosition = std::round(floatToPixel(Axis::Horizontal, m_curveViewCursor->x()));
    float yCursorPixelPosition = std::round(floatToPixel(Axis::Vertical, m_curveViewCursor->y()));

    /* If the cursor is not visible, put its frame to zero, because it might be
     * very far out of the visible frame and thus later overflow KDCoordinate.
     * The "2" factor is a big safety margin. */
    constexpr int maxCursorPixel = KDCOORDINATE_MAX / 2;
    // Assert we are not removing visible cursors
    static_assert((Ion::Display::Width * 2 < maxCursorPixel)
        && (Ion::Display::Height * 2 < maxCursorPixel),
        "maxCursorPixel is should be bigger");
    if (std::abs(yCursorPixelPosition) > maxCursorPixel
        || std::abs(xCursorPixelPosition) > maxCursorPixel)
    {
      return KDRectZero;
    }

    KDCoordinate xCursor = xCursorPixelPosition;
    KDCoordinate yCursor = yCursorPixelPosition;
    KDCoordinate xCursorFrame = xCursor - (cursorSize.width()-1)/2;
    cursorFrame = KDRect(xCursorFrame, yCursor - (cursorSize.height()-1)/2, cursorSize);
    if (cursorSize.height() == 0) {
      KDCoordinate bannerHeight = (m_bannerView != nullptr) ? m_bannerView->minimalSizeForOptimalDisplay().height() : 0;
      cursorFrame = KDRect(xCursorFrame, 0, cursorSize.width(), bounds().height() - bannerHeight);
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

int CurveView::numberOfSubviews() const {
  return (m_bannerView != nullptr) + (m_cursorView != nullptr);
};

View * CurveView::subviewAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0 && m_cursorView != nullptr) {
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
    int glyphLength = PrintFloat::ConvertFloatToText<float>(
        labelValueAtIndex(axis, i),
        label(axis, i),
        k_labelBufferMaxSize,
        labelMaxGlyphLengthSize(),
        increaseNumberOfSignificantDigits ? k_bigNumberSignificantDigits : k_numberSignificantDigits,
        Preferences::PrintFloatMode::Decimal).GlyphLength;
    // Some implementations may need to append a suffix to the label
    appendLabelSuffix(axis, label(axis, i), k_labelBufferMaxSize, glyphLength, labelMaxGlyphLengthSize());
  }
}

float CurveView::labelValueAtIndex(Axis axis, int i) const {
  assert(i >= 0 && i < numberOfLabels(axis));
  float step = labelStep(axis);
  return step*(std::ceil(min(axis)/step)+i);
}

bool CurveView::bannerIsVisible() const {
  return m_bannerView && m_mainViewSelected;
}

float CurveView::roundFloatToPixelPerfect(Axis axis, float x) const {
  if (!std::isfinite(x)) {
    return x;
  }
  float pixelStart = floatToPixel(axis, x);
  x -= (pixelStart - std::round(pixelStart)) * pixelWidth();
  return x;
}

}
