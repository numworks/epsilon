#include "plot_view.h"

#include <float.h>

#include <algorithm>
#include <cmath>

using namespace Escher;
using namespace Poincare;

namespace Shared {

void AbstractPlotView::reload(bool resetInterruption, bool force) {
  uint32_t rangeVersion = m_range->rangeChecksum();
  if (force || m_drawnRangeVersion != rangeVersion) {
    // FIXME: This should also be called if the *curve* changed
    m_drawnRangeVersion = rangeVersion;
    View *banner = bannerView();
    KDCoordinate bannerHeight = banner ? banner->bounds().height() : 0;
    markRectAsDirty(
        KDRect(0, 0, bounds().width(), bounds().height() - bannerHeight));
  }
  reloadAxes();
  layoutSubviews();
}

void AbstractPlotView::setCursorView(CursorView *cursorView) {
  markRectAsDirty(cursorFrame());
  privateSetCursorView(cursorView);
  markRectAsDirty(cursorFrame());
  layoutSubviews();
}

void AbstractPlotView::setFocus(bool focus) {
  if (m_focus != focus) {
    m_focus = focus;
    reload();
  }
}

void AbstractPlotView::drawRect(KDContext *ctx, KDRect rect) const {
  drawBackground(ctx, rect);
  drawAxesAndGrid(ctx, rect);
  drawPlot(ctx, rect);
}

float AbstractPlotView::floatToFloatPixel(Axis axis, float f) const {
  float res = axis == Axis::Horizontal ? (f - m_range->xMin()) / pixelWidth()
                                       : (m_range->yMax() - f) / pixelHeight();
  return std::clamp(res, static_cast<float>(KDCOORDINATE_MIN),
                    static_cast<float>(KDCOORDINATE_MAX));
}

KDCoordinate AbstractPlotView::floatToKDCoordinatePixel(Axis axis,
                                                        float f) const {
  return std::round(floatToFloatPixel(axis, f));
}

float AbstractPlotView::pixelToFloat(Axis axis, KDCoordinate p) const {
  return (axis == Axis::Horizontal) ? m_range->xMin() + p * pixelWidth()
                                    : m_range->yMax() - p * pixelHeight();
}

void AbstractPlotView::drawStraightSegment(KDContext *ctx, KDRect rect,
                                           Axis parallel, float position,
                                           float min, float max, KDColor color,
                                           KDCoordinate thickness,
                                           KDCoordinate dashSize) const {
  float pLength = pixelLength(parallel);
  float fmin = rangeMin(parallel) - pLength,
        fmax = rangeMax(parallel) + pLength;
  min = std::clamp(min, fmin, fmax);
  max = std::clamp(max, fmin, fmax);

  KDCoordinate p = floatToKDCoordinatePixel(OtherAxis(parallel), position);
  KDCoordinate a = floatToKDCoordinatePixel(parallel, min);
  KDCoordinate b = floatToKDCoordinatePixel(parallel, max);
  if (a > b) {
    std::swap(a, b);
  }
#if 0
  /* Uncomment this if you need a thick broken line.
   * See the history of CobwebPlotPolicy::drawPlot for a use case. */
  if (inBottomRightAngle) {
    /* Thick lines are always drawn below or to the right of the given
     * coordinates. A bottom/right corner made from a thick horizontal line
     * ending in (x,y) and a thick vertical line going up from (x,y) will
     * therefore look like this :
     *       (y) …
     *        O  O
     * (x) O  O  O
     *  …  O  O
     * We need to add a pixel in (x+1, y+1) by adding the thickness to the end. */
    b += thickness - 1;
  }
#endif
  if (dashSize <= 0 || 2 * dashSize > b - a) {
    dashSize = b - a;
    KDRect lineRect = (parallel == Axis::Horizontal)
                          ? KDRect(a, p, dashSize, thickness)
                          : KDRect(p, a, thickness, dashSize);
    ctx->fillRect(lineRect, color);
    return;
  }
  if (dashSize == 0) {
    return;
  }
  int numberOfDashes = (b - a + 1) / (2 * dashSize);
  float interval = static_cast<float>(b - a + 1 - dashSize) /
                   static_cast<float>(numberOfDashes);
  for (int d = 0; d <= numberOfDashes; d++) {
    int i = std::round(interval * d) + a;
    KDRect rectangle = parallel == Axis::Horizontal
                           ? KDRect(i, p, dashSize, thickness)
                           : KDRect(p, i, thickness, dashSize);
    ctx->fillRect(rectangle.intersectedWith(rect), color);
  }
}

void AbstractPlotView::drawSegment(KDContext *ctx, KDRect rect,
                                   Coordinate2D<float> a, Coordinate2D<float> b,
                                   KDColor color, bool thick) const {
  setDashed(false);
  Coordinate2D<float> pa = floatToPixel2D(a);
  Coordinate2D<float> pb = floatToPixel2D(b);
  straightJoinDots(ctx, rect, pa, pb, color, thick);
}

static KDCoordinate relativePositionToOffset(
    AbstractPlotView::RelativePosition position, KDCoordinate size,
    bool ignoreMargin) {
  switch (position) {
    case AbstractPlotView::RelativePosition::Before:
      return -size - (ignoreMargin ? 0.f : AbstractPlotView::k_labelMargin);
    case AbstractPlotView::RelativePosition::There:
      return -size / 2;
    default:
      assert(position == AbstractPlotView::RelativePosition::After);
      return ignoreMargin ? 0.f : AbstractPlotView::k_labelMargin;
  }
}

static KDPoint computeOrigin(Coordinate2D<float> xy, KDSize size,
                             AbstractPlotView::RelativePosition xPosition,
                             AbstractPlotView::RelativePosition yPosition,
                             bool ignoreMargin) {
  KDCoordinate x =
      std::round(xy.x()) +
      relativePositionToOffset(xPosition, size.width(), ignoreMargin);
  KDCoordinate y =
      std::round(xy.y()) +
      relativePositionToOffset(yPosition, size.height(), ignoreMargin);
  return KDPoint(x, y);
}

KDRect AbstractPlotView::labelRect(const char *label, Coordinate2D<float> xy,
                                   RelativePosition xPosition,
                                   RelativePosition yPosition,
                                   bool ignoreMargin) const {
  KDSize labelSize = KDFont::Font(k_font)->stringSize(label);
  KDPoint labelOrigin = computeOrigin(floatToPixel2D(xy), labelSize, xPosition,
                                      yPosition, ignoreMargin);
  return KDRect(labelOrigin, labelSize);
}

void AbstractPlotView::drawLabel(KDContext *ctx, KDRect rect, const char *label,
                                 const KDRect labelRect, KDColor color) const {
  if (labelRect.intersects(rect)) {
    ctx->drawString(label, labelRect.origin(), k_font, color,
                    backgroundColor());
  }
}

void AbstractPlotView::drawLabel(KDContext *ctx, KDRect rect, const char *label,
                                 Coordinate2D<float> xy,
                                 RelativePosition xPosition,
                                 RelativePosition yPosition, KDColor color,
                                 bool ignoreMargin) const {
  KDRect thisLabelRect =
      labelRect(label, xy, xPosition, yPosition, ignoreMargin);
  drawLabel(ctx, rect, label, thisLabelRect, color);
}

void AbstractPlotView::drawLayout(KDContext *ctx, KDRect rect, Layout layout,
                                  Coordinate2D<float> xy,
                                  RelativePosition xPosition,
                                  RelativePosition yPosition, KDColor color,
                                  bool ignoreMargin) const {
  KDSize layoutSize = layout.layoutSize(k_font);
  KDPoint layoutOrigin = computeOrigin(floatToPixel2D(xy), layoutSize,
                                       xPosition, yPosition, ignoreMargin);
  if (KDRect(layoutOrigin, layoutSize).intersects(rect)) {
    layout.draw(ctx, layoutOrigin, k_font, color, backgroundColor());
  }
}

KDRect AbstractPlotView::dotRect(Dots::Size size,
                                 Poincare::Coordinate2D<float> xy) const {
  KDCoordinate diameter = 0;
  switch (size) {
    case Dots::Size::Tiny:
      diameter = Dots::TinyDotDiameter;
      break;
    case Dots::Size::Small:
      diameter = Dots::SmallDotDiameter;
      break;
    case Dots::Size::Medium:
      diameter = Dots::MediumDotDiameter;
      break;
    default:
      assert(size == Dots::Size::Large);
      diameter = Dots::LargeDotDiameter;
  }
  assert(diameter <= Dots::LargeDotDiameter);
  /* If circle has an even diameter, out of the four center pixels, the bottom
   * left one will be placed at (x, y) */
  Coordinate2D<float> pF = floatToPixel2D(xy);
  KDPoint p(std::round(pF.x()) - (diameter - 1) / 2,
            std::round(pF.y()) - diameter / 2);
  return KDRect(p.x(), p.y(), diameter, diameter);
}

void AbstractPlotView::drawDot(KDContext *ctx, KDRect rect, Dots::Size size,
                               Poincare::Coordinate2D<float> xy,
                               KDColor color) const {
  const uint8_t *mask = nullptr;
  switch (size) {
    case Dots::Size::Tiny:
      mask = (const uint8_t *)Dots::TinyDotMask;
      break;
    case Dots::Size::Small:
      mask = (const uint8_t *)Dots::SmallDotMask;
      break;
    case Dots::Size::Medium:
      mask = (const uint8_t *)Dots::MediumDotMask;
      break;
    default:
      assert(size == Dots::Size::Large);
      mask = (const uint8_t *)Dots::LargeDotMask;
  }

  KDRect rectForDot = dotRect(size, xy);
  if (rect.intersects(rectForDot)) {
    KDColor workingBuffer[Dots::LargeDotDiameter * Dots::LargeDotDiameter];
    ctx->blendRectWithMask(rectForDot, color, mask, workingBuffer);
  }
}

double AbstractPlotView::angleFromPoint(KDPoint point) const {
  double x = pixelToFloat(Axis::Horizontal, point.x());
  double y = pixelToFloat(Axis::Vertical, point.y());
  double angle = std::atan2(y, x);
  return angle < 0 ? angle + 2 * M_PI : angle;
}

void AbstractPlotView::drawArc(KDContext *ctx, KDRect rect,
                               Poincare::Coordinate2D<float> center,
                               float radius, float angleStart, float angleEnd,
                               KDColor color) const {
  assert(radius >= 0.f);
  double previousT = NAN;
  double t = NAN;
  double previousX = NAN;
  double x = NAN;
  double previousY = NAN;
  double y = NAN;
  int i = 0;
  bool isLastSegment = false;
  double tMin = 0;
  double tMax = 2 * M_PI;
  if (!rect.contains(KDPoint(floatToKDCoordinatePixel(Axis::Horizontal, 0.f),
                             floatToKDCoordinatePixel(Axis::Vertical, 0.f)))) {
    // TODO: factorise with Graph::GraphView::drawPolar
    double t1 = angleFromPoint(rect.bottomRight());
    double t2 = angleFromPoint(rect.topRight());
    double t3 = angleFromPoint(rect.bottomLeft());
    double t4 = angleFromPoint(rect.topLeft());
    tMin = std::min({t1, t2, t3, t4});
    tMax = std::max({t1, t2, t3, t4});
    // Does the angle cross the positive x axis ?
    if (tMax - tMin > M_PI) {
      /* When we are in this situation
       *
       *
       *     t4             t2 = tMin
       *      +-------------+
       *      |             |     angleStart
       *  0- -|- - - - - - -|- - -
       *      |             |     angleEnd
       *      |             |
       *      +-------------+
       *     t3             t1 = tMax
       *
       *
       * We split the drawing in two parts :
       * - from t3 to angleEnd with a recursive call
       * - from angleStart to t4 with the body of this function
       *
       * TODO: make sure it works with all angleStart and angleEnd
       */
      if (angleStart < tMin) {
        // Are we not in the recursive call already ?
        drawArc(ctx, rect, center, radius, t3, angleEnd, color);
        angleEnd = t4;
      }
    } else {
      angleStart = std::max(angleStart, (float)tMin);
      angleEnd = std::min(angleEnd, (float)tMax);
    }
  }
  // Choose tStep to match the expected length of a single segment in pixels
  const float segmentLengthInPixels = 8.f;  // Ad hoc
  const float radiusInPixel =
      std::max(radius / pixelWidth(), radius / pixelHeight());
  // 2π * length / perimeter where perimeter = 2π * radius in pixels
  const float tStep =
      std::min((float)M_PI / 30.f, segmentLengthInPixels / radiusInPixel);
  do {
    previousT = t;
    t = angleStart + (i++) * tStep;
    if (t <= angleStart) {
      t = angleStart + FLT_EPSILON;
    }
    if (t >= angleEnd) {
      t = angleEnd - FLT_EPSILON;
      isLastSegment = true;
    }
    if (previousT == t) {
      // No need to draw segment. Happens when tStep << tStart .
      continue;
    }
    previousX = x;
    previousY = y;
    x = floatToFloatPixel(Axis::Horizontal, std::cos(t) * radius);
    y = floatToFloatPixel(Axis::Vertical, std::sin(t) * radius);
    if (std::isnan(previousX)) {
      continue;
    }
    KDPoint from = KDPoint(previousX, previousY);
    KDPoint to = KDPoint(x, y);
    if (rect.contains(from) || rect.contains(to)) {
      ctx->drawAntialiasedLine(previousX, previousY, x, y, color, KDColorWhite);
    }
    // straightJoinDots(ctx, rect, x, y, previousX, previousY, color, thick);
  } while (!isLastSegment);
}

void AbstractPlotView::drawTick(KDContext *ctx, KDRect rect, Axis perpendicular,
                                float position, KDColor color) const {
  Axis parallel = OtherAxis(perpendicular);
  KDCoordinate p = floatToKDCoordinatePixel(perpendicular, position);
  KDCoordinate tickStart =
      floatToKDCoordinatePixel(parallel, 0.f) - k_tickHalfLength;
  KDCoordinate tickLength = 2 * k_tickHalfLength + 1;
  KDRect tickRect = perpendicular == Axis::Horizontal
                        ? KDRect(p, tickStart, 1, tickLength)
                        : KDRect(tickStart, p, tickLength, 1);
  ctx->fillRect(tickRect, color);
}

void AbstractPlotView::drawArrowhead(KDContext *ctx, KDRect rect,
                                     Coordinate2D<float> xy,
                                     Coordinate2D<float> dxy,
                                     float pixelArrowWidth, KDColor color,
                                     bool thick, float tanAngle) const {
  /*
   * In the screen plane:
   *
   *            /C     |                ^
   *          /        h                |v
   *        / \ angle  |             u  |
   *    A <------------------ B    <----+
   *        \
   *          \
   *            \D
   *
   *      -- l --
   */

  Coordinate2D<float> xy2(xy.x() - dxy.x(), xy.y() - dxy.y());
  Coordinate2D<float> pA = floatToPixel2D(xy);
  Coordinate2D<float> pB = floatToPixel2D(xy2);
  Coordinate2D<float> pAB(pB.x() - pA.x(), pB.y() - pA.y());
  float bodyLength = std::sqrt(std::pow(pAB.x(), 2.f) + std::pow(pAB.y(), 2.f));
  Coordinate2D<float> u(pAB.x() / bodyLength, pAB.y() / bodyLength);
  Coordinate2D<float> v(-u.y(), u.x());

  if (pixelArrowWidth == 0.f) {
    constexpr float defaultArrowWidth = 8.f;
    pixelArrowWidth = defaultArrowWidth;
  }
  float h = pixelArrowWidth * 0.5f;
  float l = h / tanAngle;

  Coordinate2D<float> pC(pA.x() + l * u.x() + h * v.x(),
                         pA.y() + l * u.y() + h * v.y());
  Coordinate2D<float> pD(pA.x() + l * u.x() - h * v.x(),
                         pA.y() + l * u.y() - h * v.y());

  straightJoinDots(ctx, rect, pA, pC, color, thick);
  straightJoinDots(ctx, rect, pA, pD, color, thick);
}

View *AbstractPlotView::subviewAtIndex(int i) {
  constexpr int maxNumberOfSubviews = 3;
  View *subviews[maxNumberOfSubviews] = {bannerView(), cursorView(),
                                         ornamentView()};
  assert(numberOfSubviews() <= maxNumberOfSubviews);
  for (int k = 0; k < maxNumberOfSubviews; k++) {
    if (subviews[k] == nullptr) {
      continue;
    }
    if (i == 0) {
      return subviews[k];
    }
    i--;
  }
  assert(false);
  return nullptr;
}

void AbstractPlotView::layoutSubviews(bool force) {
  View *banner = bannerView();
  if (banner) {
    KDRect oldFrame = banner->bounds();
    setChildFrame(banner, bannerFrame(), force);
    /* If the banner frame changes, dirty the area right above it, since the X
     * axis labels may have moved. */
    if (!(oldFrame == banner->bounds())) {
      KDCoordinate dirtyHeight =
          std::max(oldFrame.height(), banner->bounds().height()) +
          KDFont::GlyphSize(k_font).height() + 2 * k_labelMargin;
      markRectAsDirty(KDRect(0, bounds().height() - dirtyHeight,
                             bounds().width(), dirtyHeight));
    }
  }
  CursorView *cursor = cursorView();
  if (cursor) {
    cursor->setCursorFrame(this, cursorFrame(), force);
  }
  View *ornament = ornamentView();
  if (ornament) {
    setChildFrame(ornament, bounds().differencedWith(bannerFrame()), force);
  }
}

/* The following code is dedicated to stamping the screen.
 * FIXME Move it into a helper to clean up this file ? */

template <unsigned T>
struct Mask {
  uint8_t m_mask[T * T];
};

constexpr KDCoordinate k_thinCircleDiameter = 1;
constexpr KDCoordinate k_thinStampSize = k_thinCircleDiameter + 1;
constexpr const Mask<k_thinStampSize + 1> thinStampMask{
    0xFF, 0xE1, 0xFF, 0xE1, 0x00, 0xE1, 0xFF, 0xE1, 0xFF,
};

#define LINE_THICKNESS 2

constexpr KDCoordinate k_thickCircleDiameter = LINE_THICKNESS;
constexpr KDCoordinate k_thickStampSize = k_thickCircleDiameter + 1;

#if LINE_THICKNESS == 2

constexpr const Mask<k_thickStampSize + 1> thickStampMask{
    0xFF, 0xE6, 0xE6, 0xFF, 0xE6, 0x33, 0x33, 0xE6,
    0xE6, 0x33, 0x33, 0xE6, 0xFF, 0xE6, 0xE6, 0xFF,
};

#elif LINE_THICKNESS == 3

constexpr const Mask<k_thickStampSize + 1> thickStampMask{
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7A, 0x0C, 0x7A,
    0xFF, 0xFF, 0x0C, 0x00, 0x0C, 0xFF, 0xFF, 0x7A, 0x0C,
    0x7A, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

#elif LINE_THICKNESS == 5

constexpr const Mask<k_thickStampSize + 1> thickStampMask{
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE1, 0x45,
    0x0C, 0x45, 0xE1, 0xFF, 0xFF, 0x45, 0x00, 0x00, 0x00, 0x45,
    0xFF, 0xFF, 0x0C, 0x00, 0x00, 0x00, 0x0C, 0xFF, 0xFF, 0x45,
    0x00, 0x00, 0x00, 0x45, 0xFF, 0xFF, 0xE1, 0x45, 0x0C, 0x45,
    0xE1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};

#endif

template <unsigned T>
constexpr Mask<T - 1> shiftedMask(const Mask<T> stampMask, float dx, float dy) {
  const KDCoordinate stampSize = T - 1;
  const KDCoordinate stampMaskSize = T;
  Mask<T - 1> shiftedMask = {};
  for (int i = 0; i < stampSize; i++) {
    for (int j = 0; j < stampSize; j++) {
      shiftedMask.m_mask[j * stampSize + i] =
          (1.0f - dx) * (stampMask.m_mask[j * stampMaskSize + i] * (1.0 - dy) +
                         stampMask.m_mask[(j + 1) * stampMaskSize + i] * dy) +
          dx * (stampMask.m_mask[j * stampMaskSize + (i + 1)] * (1.0f - dy) +
                stampMask.m_mask[(j + 1) * stampMaskSize + (i + 1)] * dy);
    }
  }
  return shiftedMask;
}

/* [0,1[ is divided in 4 but we precompute 0. and 1. so that we don't have to
 * add logic to move to the next pixel if dx rounds up to 1. */
constexpr size_t k_shiftedSteps = 4;

constexpr Mask<k_thinStampSize> thinShiftedMasks[k_shiftedSteps +
                                                 1][k_shiftedSteps + 1] = {
    {shiftedMask(thinStampMask, .00, .00), shiftedMask(thinStampMask, .00, .25),
     shiftedMask(thinStampMask, .00, .50), shiftedMask(thinStampMask, .00, .75),
     shiftedMask(thinStampMask, .00, 1.0)},
    {shiftedMask(thinStampMask, .25, .00), shiftedMask(thinStampMask, .25, .25),
     shiftedMask(thinStampMask, .25, .50), shiftedMask(thinStampMask, .25, .75),
     shiftedMask(thinStampMask, .25, 1.0)},
    {shiftedMask(thinStampMask, .50, .00), shiftedMask(thinStampMask, .50, .25),
     shiftedMask(thinStampMask, .50, .50), shiftedMask(thinStampMask, .50, .75),
     shiftedMask(thinStampMask, .50, 1.0)},
    {shiftedMask(thinStampMask, .75, .00), shiftedMask(thinStampMask, .75, .25),
     shiftedMask(thinStampMask, .75, .50), shiftedMask(thinStampMask, .75, .75),
     shiftedMask(thinStampMask, .75, 1.0)},
    {shiftedMask(thinStampMask, 1.0, .00), shiftedMask(thinStampMask, 1.0, .25),
     shiftedMask(thinStampMask, 1.0, .50), shiftedMask(thinStampMask, 1.0, .75),
     shiftedMask(thinStampMask, 1.0, 1.0)},
};

constexpr Mask<k_thickStampSize>
    thickShiftedMasks[k_shiftedSteps + 1][k_shiftedSteps + 1] = {
        {shiftedMask(thickStampMask, .00, .00),
         shiftedMask(thickStampMask, .00, .25),
         shiftedMask(thickStampMask, .00, .50),
         shiftedMask(thickStampMask, .00, .75),
         shiftedMask(thickStampMask, .00, 1.0)},
        {shiftedMask(thickStampMask, .25, .00),
         shiftedMask(thickStampMask, .25, .25),
         shiftedMask(thickStampMask, .25, .50),
         shiftedMask(thickStampMask, .25, .75),
         shiftedMask(thickStampMask, .25, 1.0)},
        {shiftedMask(thickStampMask, .50, .00),
         shiftedMask(thickStampMask, .50, .25),
         shiftedMask(thickStampMask, .50, .50),
         shiftedMask(thickStampMask, .50, .75),
         shiftedMask(thickStampMask, .50, 1.0)},
        {shiftedMask(thickStampMask, .75, .00),
         shiftedMask(thickStampMask, .75, .25),
         shiftedMask(thickStampMask, .75, .50),
         shiftedMask(thickStampMask, .75, .75),
         shiftedMask(thickStampMask, .75, 1.0)},
        {shiftedMask(thickStampMask, 1.0, .00),
         shiftedMask(thickStampMask, 1.0, .25),
         shiftedMask(thickStampMask, 1.0, .50),
         shiftedMask(thickStampMask, 1.0, .75),
         shiftedMask(thickStampMask, 1.0, 1.0)},
};

static void clipBarycentricCoordinatesBetweenBounds(float *start, float *end,
                                                    KDCoordinate lower,
                                                    KDCoordinate upper,
                                                    float p1, float p2) {
  if (p1 != p2) {
    float upperPosition = (upper - p1) / (p2 - p1);
    float lowerPosition = (lower - p1) / (p2 - p1);
    if (upperPosition < lowerPosition) {
      std::swap(lowerPosition, upperPosition);
    }
    *start = std::clamp(*start, lowerPosition, upperPosition);
    *end = std::clamp(*end, lowerPosition, upperPosition);
  }
}

void AbstractPlotView::straightJoinDots(KDContext *ctx, KDRect rect,
                                        Coordinate2D<float> pixelA,
                                        Coordinate2D<float> pixelB,
                                        KDColor color, bool thick) const {
  /* Before drawing the line segment, clip it to rect:
   * start and end are the barycentric coordinates on the line segment (0
   * corresponding to A and 1 to B), of the drawing start and end
   * points. */
  float start = 0;
  float end = 1;
  KDCoordinate stampSize = thick ? k_thickStampSize : k_thinStampSize;
  clipBarycentricCoordinatesBetweenBounds(&start, &end, rect.left() - stampSize,
                                          rect.right() + stampSize, pixelA.x(),
                                          pixelB.x());
  clipBarycentricCoordinatesBetweenBounds(&start, &end, rect.top() - stampSize,
                                          rect.bottom() + stampSize, pixelA.y(),
                                          pixelB.y());
  float puf = start * pixelB.x() + (1 - start) * pixelA.x();
  float pvf = start * pixelB.y() + (1 - start) * pixelA.y();
  float pxf = end * pixelB.x() + (1 - end) * pixelA.x();
  float pyf = end * pixelB.y() + (1 - end) * pixelA.y();

  float deltaX = pxf - puf;
  float deltaY = pyf - pvf;
  KDCoordinate circleDiameter =
      thick ? k_thickCircleDiameter : k_thinCircleDiameter;
  float normsRatio =
      std::sqrt(deltaX * deltaX + deltaY * deltaY) / (circleDiameter / 2.0f);
  float stepX = deltaX / normsRatio;
  float stepY = deltaY / normsRatio;
  int numberOfStamps = std::floor(normsRatio);
  for (int i = 1; i <= numberOfStamps; i++) {
    stamp(ctx, rect, Coordinate2D<float>(puf + i * stepX, pvf + i * stepY),
          color, thick);
  }
}

void AbstractPlotView::stamp(KDContext *ctx, KDRect rect, Coordinate2D<float> p,
                             KDColor color, bool thick) const {
  if (m_stampDashIndex != k_stampIndexNoDash) {
    /* The curve or line being drawn is dashed. Skip half the stamps. */
    bool skipThisStamp = m_stampDashIndex >= k_stampDashSize;
    m_stampDashIndex = (m_stampDashIndex + 1) % (2 * k_stampDashSize);
    if (skipThisStamp) {
      return;
    }
  }

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

  KDCoordinate stampSize = thick ? k_thickStampSize : k_thinStampSize;
  float pxf = p.x() - 0.5f * stampSize;
  float pyf = p.y() - 0.5f * stampSize;
  KDCoordinate px = std::ceil(pxf);
  KDCoordinate py = std::ceil(pyf);
  KDRect stampRect(px, py, stampSize, stampSize);
  if (!rect.intersects(stampRect)) {
    return;
  }

  KDColor workingBuffer[stampSize * stampSize];
  float dx = px - pxf;
  float dy = py - pyf;
  size_t ix = std::round(dx * k_shiftedSteps);
  size_t iy = std::round(dy * k_shiftedSteps);
  const uint8_t *shiftedMask;
  if (thick) {
    shiftedMask = &thickShiftedMasks[ix][iy].m_mask[0];
  } else {
    shiftedMask = &thinShiftedMasks[ix][iy].m_mask[0];
  }
  ctx->blendRectWithMask(stampRect, color, shiftedMask, workingBuffer);
}

bool AbstractPlotView::pointsInSameStamp(Coordinate2D<float> p1,
                                         Coordinate2D<float> p2,
                                         bool thick) const {
  KDCoordinate diameter = thick ? k_thickCircleDiameter : k_thinCircleDiameter;
  const float deltaX = p1.x() - p2.x();
  const float deltaY = p1.y() - p2.y();
  return deltaX * deltaX + deltaY * deltaY < 0.25f * diameter * diameter;
}

}  // namespace Shared
