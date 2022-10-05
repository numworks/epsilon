#include "plot_view.h"
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
    View * banner = bannerView();
    KDCoordinate bannerHeight = banner ? banner->bounds().height() : 0;
    markRectAsDirty(KDRect(0, 0, bounds().width(), bounds().height() - bannerHeight));
    reloadAxes();
  }
  layoutSubviews();
}

void AbstractPlotView::setCursorView(CursorView * cursorView) {
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

void AbstractPlotView::drawRect(KDContext * ctx, KDRect rect) const {
  drawBackground(ctx, rect);
  drawAxes(ctx, rect);
  drawPlot(ctx, rect);
}

static float clippedFloat(float f) {
  if (f < KDCOORDINATE_MIN) {
    return KDCOORDINATE_MIN;
  } else if (f > KDCOORDINATE_MAX) {
    return KDCOORDINATE_MAX;
  } else {
    return f;
  }
}

float AbstractPlotView::floatToPixel(Axis axis, float f) const {
  float res = axis == Axis::Horizontal ? (f - m_range->xMin()) / pixelWidth() : (m_range->yMax() - f) / pixelHeight();
  return clippedFloat(res);
}

float AbstractPlotView::pixelToFloat(Axis axis, KDCoordinate p) const {
  return (axis == Axis::Horizontal) ? m_range->xMin() + p * pixelWidth() : m_range->yMax() - p * pixelHeight();
}

void AbstractPlotView::drawStraightSegment(KDContext * ctx, KDRect rect, Axis parallel, float position, float min, float max, KDColor color, KDCoordinate thickness, KDCoordinate dashSize) const {
  float pLength = pixelLength(parallel);
  float fmin = rangeMin(parallel) - pLength, fmax = rangeMax(parallel) + pLength;
  min = std::clamp(min, fmin, fmax);
  max = std::clamp(max, fmin, fmax);

  KDCoordinate p = std::round(floatToPixel(OtherAxis(parallel), position));
  KDCoordinate a = std::round(floatToPixel(parallel, min));
  KDCoordinate b = std::round(floatToPixel(parallel, max));
  if (a > b) {
    float temp = a;
    a = b;
    b = temp;
  }
  if (dashSize <= 0) {
    dashSize = b - a;
  }
  if (dashSize == 0) {
    return;
  }
  for (KDCoordinate i = a; i < b; i += 2 * dashSize) {
    KDRect rectangle = parallel == Axis::Horizontal ? KDRect(i, p, dashSize, thickness) : KDRect(p, i, thickness, dashSize);
    ctx->fillRect(rectangle.intersectedWith(rect), color);
  }
}

void AbstractPlotView::drawSegment(KDContext * ctx, KDRect rect, Coordinate2D<float> a, Coordinate2D<float> b, KDColor color, bool thick) const {
  setDashed(false);
  Coordinate2D<float> pa = floatToPixel2D(a);
  Coordinate2D<float> pb = floatToPixel2D(b);
  straightJoinDots(ctx, rect, pa, pb, color, thick);
}

static KDCoordinate relativePositionToOffset(AbstractPlotView::RelativePosition position, KDCoordinate size, bool ignoreMargin) {
  switch (position) {
  case AbstractPlotView::RelativePosition::Before:
    return -size - (ignoreMargin ? 0.f : AbstractPlotView::k_labelMargin);
  case AbstractPlotView::RelativePosition::There:
    return - size / 2;
  default:
    assert(position == AbstractPlotView::RelativePosition::After);
    return ignoreMargin ? 0.f : AbstractPlotView::k_labelMargin;
  }
}

void AbstractPlotView::drawLabel(KDContext * ctx, KDRect rect, const char * label, Coordinate2D<float> xy, RelativePosition xPosition, RelativePosition yPosition, KDColor color, bool ignoreMargin) const {
  KDSize labelSize = KDFont::Font(k_font)->stringSize(label);

  Coordinate2D<float> p = floatToPixel2D(xy);
  KDCoordinate x = std::round(p.x1()) + relativePositionToOffset(xPosition, labelSize.width(), ignoreMargin);
  KDCoordinate y = std::round(p.x2()) + relativePositionToOffset(yPosition, labelSize.height(), ignoreMargin);
  KDPoint labelOrigin(x, y);

  if (KDRect(labelOrigin, labelSize).intersects(rect)) {
    ctx->drawString(label, labelOrigin, k_font, color, backgroundColor());
  }
}

void AbstractPlotView::drawLayout(KDContext * ctx, KDRect rect, Layout layout, Coordinate2D<float> xy, RelativePosition xPosition, RelativePosition yPosition, KDColor color, bool ignoreMargin) const {
  KDSize layoutSize = layout.layoutSize(k_font);

  Coordinate2D<float> p = floatToPixel2D(xy);
  KDCoordinate x = std::round(p.x1() + relativePositionToOffset(xPosition, layoutSize.width(), ignoreMargin));
  KDCoordinate y = std::round(p.x2() + relativePositionToOffset(yPosition, layoutSize.height(), ignoreMargin));
  KDPoint layoutOrigin(x, y);

  if (KDRect(layoutOrigin, layoutSize).intersects(rect)) {
    layout.draw(ctx, layoutOrigin, k_font, color, backgroundColor());
  }
}

void AbstractPlotView::drawDot(KDContext * ctx, KDRect rect, Dots::Size size, Poincare::Coordinate2D<float> xy, KDColor color) const {
  KDCoordinate diameter = 0;
  const uint8_t * mask = nullptr;
  switch (size) {
  case Dots::Size::Tiny:
    diameter = Dots::TinyDotDiameter;
    mask = (const uint8_t *)Dots::TinyDotMask;
    break;
  case Dots::Size::Small:
    diameter = Dots::SmallDotDiameter;
    mask = (const uint8_t *)Dots::SmallDotMask;
    break;
  case Dots::Size::Medium:
    diameter = Dots::MediumDotDiameter;
    mask = (const uint8_t *)Dots::MediumDotMask;
    break;
  default:
    assert(size == Dots::Size::Large);
    diameter = Dots::LargeDotDiameter;
    mask = (const uint8_t *)Dots::LargeDotMask;
  }
  assert(diameter <= Dots::LargeDotDiameter);
  KDColor workingBuffer[Dots::LargeDotDiameter * Dots::LargeDotDiameter];

  /* If circle has an even diameter, out of the four center pixels, the bottom
   * left one will be placed at (x, y) */
  Coordinate2D<float> pF = floatToPixel2D(xy);
  KDPoint p(std::round(pF.x1()) - (diameter - 1) / 2, std::round(pF.x2()) - diameter / 2);
  KDRect dotRect(p.x(), p.y(), diameter, diameter);
  if (rect.intersects(dotRect)) {
    ctx->blendRectWithMask(dotRect, color, mask, workingBuffer);
  }
}

void AbstractPlotView::drawTick(KDContext * ctx, KDRect rect, Axis perpendicular, float position, KDColor color) const {
  Axis parallel = OtherAxis(perpendicular);
  KDCoordinate p = std::round(floatToPixel(perpendicular, position));
  KDCoordinate tickStart = std::round(floatToPixel(parallel, 0.f)) - k_tickHalfLength;
  KDCoordinate tickLength = 2 * k_tickHalfLength + 1;
  KDRect tickRect = perpendicular == Axis::Horizontal ? KDRect(p, tickStart, 1, tickLength) : KDRect(tickStart, p, tickLength, 1);
  ctx->fillRect(tickRect, color);
}

void AbstractPlotView::drawArrowhead(KDContext * ctx, KDRect rect, Coordinate2D<float> xy, Coordinate2D<float> dxy, float pixelArrowWidth, KDColor color, bool thick, float tanAngle) const {
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

  Coordinate2D<float> xy2(xy.x1() - dxy.x1(), xy.x2() - dxy.x2());
  Coordinate2D<float> pA = floatToPixel2D(xy);
  Coordinate2D<float> pB = floatToPixel2D(xy2);
  Coordinate2D<float> pAB(pB.x1() - pA.x1(), pB.x2() - pA.x2());
  float bodyLength = std::sqrt(std::pow(pAB.x1(), 2.f) + std::pow(pAB.x2(), 2.f));
  Coordinate2D<float> u(pAB.x1() / bodyLength, pAB.x2() / bodyLength);
  Coordinate2D<float> v(-u.x2(), u.x1());

  if (pixelArrowWidth == 0.f) {
    constexpr float defaultArrowWidth = 8.f;
    pixelArrowWidth = defaultArrowWidth;
  }
  float h = pixelArrowWidth * 0.5f;
  float l = h / tanAngle;

  Coordinate2D<float> pC(pA.x1() + l * u.x1() + h * v.x1(), pA.x2() + l * u.x2() + h * v.x2());
  Coordinate2D<float> pD(pA.x1() + l * u.x1() - h * v.x1(), pA.x2() + l * u.x2() - h * v.x2());

  straightJoinDots(ctx, rect, pA, pC, color, thick);
  straightJoinDots(ctx, rect, pA, pD, color, thick);
}

View * AbstractPlotView::subviewAtIndex(int i) {
  View * subviews[] = { bannerView(), cursorView() };
  if (!subviews[0]) {
    i++;
  }
  assert(i < sizeof(subviews) / sizeof(subviews[0]));
  return subviews[i];
}

void AbstractPlotView::layoutSubviews(bool force) {
  View * banner = bannerView();
  if (banner) {
    KDRect oldFrame = banner->bounds();
    banner->setFrame(bannerFrame(), force);
    /* If the banner frame changes, dirty the area right above it, since the X
     * axis labels may have moved. */
    if (!(oldFrame == banner->bounds())) {
      KDCoordinate dirtyHeight = std::max(oldFrame.height(), banner->bounds().height()) + KDFont::GlyphSize(k_font).height() + 2 * k_labelMargin;
      markRectAsDirty(KDRect(0, m_frame.height() - dirtyHeight, m_frame.width(), dirtyHeight));
    }
  }
  CursorView * cursor = cursorView();
  if (cursor) {
    cursor->setCursorFrame(cursorFrame(), force);
  }
}

/* The following code is dedicated to stamping the screen.
 * FIXME Move it into a helper to clean up this file ? */

template<unsigned T>
struct Mask {
  uint8_t m_mask[T*T];
};

constexpr KDCoordinate thinCircleDiameter = 1;
constexpr KDCoordinate thinStampSize = thinCircleDiameter+1;
constexpr const Mask<thinStampSize + 1> thinStampMask {
  0xFF, 0xE1, 0xFF,
  0xE1, 0x00, 0xE1,
  0xFF, 0xE1, 0xFF,
};

#define LINE_THICKNESS 2

#if LINE_THICKNESS == 2

constexpr KDCoordinate thickCircleDiameter = 2;
constexpr KDCoordinate thickStampSize = thickCircleDiameter+1;
constexpr const Mask<thickStampSize + 1> thickStampMask {
  0xFF, 0xE6, 0xE6, 0xFF,
  0xE6, 0x33, 0x33, 0xE6,
  0xE6, 0x33, 0x33, 0xE6,
  0xFF, 0xE6, 0xE6, 0xFF,
};

#elif LINE_THICKNESS == 3

constexpr KDCoordinate thickCircleDiameter = 3;
constexpr KDCoordinate thickStampSize = thickCircleDiameter+1;
constexpr const Mask<thickStampSize + 1> thickStampMask {
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0x7A, 0x0C, 0x7A, 0xFF,
  0xFF, 0x0C, 0x00, 0x0C, 0xFF,
  0xFF, 0x7A, 0x0C, 0x7A, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

#elif LINE_THICKNESS == 5

constexpr KDCoordinate thickCircleDiameter = 5;
constexpr KDCoordinate thickStampSize = thickCircleDiameter+1;
constexpr const Mask<thickStampSize + 1> thickStampMask {
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xE1, 0x45, 0x0C, 0x45, 0xE1, 0xFF,
  0xFF, 0x45, 0x00, 0x00, 0x00, 0x45, 0xFF,
  0xFF, 0x0C, 0x00, 0x00, 0x00, 0x0C, 0xFF,
  0xFF, 0x45, 0x00, 0x00, 0x00, 0x45, 0xFF,
  0xFF, 0xE1, 0x45, 0x0C, 0x45, 0xE1, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};

#endif

template<unsigned T>
constexpr Mask<T-1> shiftedMask(const Mask<T> stampMask, float dx, float dy) {
  const KDCoordinate stampSize = T - 1;
  const KDCoordinate stampMaskSize = T;
  Mask<T-1> shiftedMask = {};
  for (int i=0; i<stampSize; i++) {
    for (int j=0; j<stampSize; j++) {
      shiftedMask.m_mask[j*stampSize+i] = (1.0f - dx) * (stampMask.m_mask[j*stampMaskSize+i]*(1.0-dy)+stampMask.m_mask[(j+1)*stampMaskSize+i]*dy)
        + dx * (stampMask.m_mask[j*stampMaskSize+(i+1)]*(1.0f-dy) + stampMask.m_mask[(j+1)*stampMaskSize+(i+1)]*dy);
    }
  }
  return shiftedMask;
}

/* [0,1[ is divided in 4 but we precompute 0. and 1. so that we don't have to
 * add logic to move to the next pixel if dx rounds up to 1. */
constexpr size_t k_shiftedSteps = 4;

constexpr Mask<thinStampSize> thinShiftedMasks[k_shiftedSteps + 1][k_shiftedSteps + 1] = {
  {shiftedMask(thinStampMask, .00, .00), shiftedMask(thinStampMask, .00, .25), shiftedMask(thinStampMask, .00, .50), shiftedMask(thinStampMask, .00, .75), shiftedMask(thinStampMask, .00, 1.0)},
  {shiftedMask(thinStampMask, .25, .00), shiftedMask(thinStampMask, .25, .25), shiftedMask(thinStampMask, .25, .50), shiftedMask(thinStampMask, .25, .75), shiftedMask(thinStampMask, .25, 1.0)},
  {shiftedMask(thinStampMask, .50, .00), shiftedMask(thinStampMask, .50, .25), shiftedMask(thinStampMask, .50, .50), shiftedMask(thinStampMask, .50, .75), shiftedMask(thinStampMask, .50, 1.0)},
  {shiftedMask(thinStampMask, .75, .00), shiftedMask(thinStampMask, .75, .25), shiftedMask(thinStampMask, .75, .50), shiftedMask(thinStampMask, .75, .75), shiftedMask(thinStampMask, .75, 1.0)},
  {shiftedMask(thinStampMask, 1.0, .00), shiftedMask(thinStampMask, 1.0, .25), shiftedMask(thinStampMask, 1.0, .50), shiftedMask(thinStampMask, 1.0, .75), shiftedMask(thinStampMask, 1.0, 1.0)},
};

constexpr Mask<thickStampSize> thickShiftedMasks[k_shiftedSteps + 1][k_shiftedSteps + 1] = {
  {shiftedMask(thickStampMask, .00, .00), shiftedMask(thickStampMask, .00, .25), shiftedMask(thickStampMask, .00, .50), shiftedMask(thickStampMask, .00, .75), shiftedMask(thickStampMask, .00, 1.0)},
  {shiftedMask(thickStampMask, .25, .00), shiftedMask(thickStampMask, .25, .25), shiftedMask(thickStampMask, .25, .50), shiftedMask(thickStampMask, .25, .75), shiftedMask(thickStampMask, .25, 1.0)},
  {shiftedMask(thickStampMask, .50, .00), shiftedMask(thickStampMask, .50, .25), shiftedMask(thickStampMask, .50, .50), shiftedMask(thickStampMask, .50, .75), shiftedMask(thickStampMask, .50, 1.0)},
  {shiftedMask(thickStampMask, .75, .00), shiftedMask(thickStampMask, .75, .25), shiftedMask(thickStampMask, .75, .50), shiftedMask(thickStampMask, .75, .75), shiftedMask(thickStampMask, .75, 1.0)},
  {shiftedMask(thickStampMask, 1.0, .00), shiftedMask(thickStampMask, 1.0, .25), shiftedMask(thickStampMask, 1.0, .50), shiftedMask(thickStampMask, 1.0, .75), shiftedMask(thickStampMask, 1.0, 1.0)},
};

static void clipBarycentricCoordinatesBetweenBounds(float * start, float * end, KDCoordinate lower, KDCoordinate upper, float p1, float p2) {
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

void AbstractPlotView::straightJoinDots(KDContext * ctx, KDRect rect, Coordinate2D<float> pixelA, Coordinate2D<float> pixelB, KDColor color, bool thick) const {
  /* Before drawing the line segment, clip it to rect:
   * start and end are the barycentric coordinates on the line segment (0
   * corresponding to (u, v) and 1 to (x, y)), of the drawing start and end
   * points. */
  float start = 0;
  float end = 1;
  KDCoordinate stampSize = thick ? thickStampSize : thinStampSize;
  clipBarycentricCoordinatesBetweenBounds(&start, &end, rect.left() - stampSize, rect.right() + stampSize, pixelA.x1(), pixelB.x1());
  clipBarycentricCoordinatesBetweenBounds(&start, &end, rect.top() - stampSize, rect.bottom() + stampSize, pixelA.x2(), pixelB.x2());
  float puf = start * pixelB.x1() + (1 - start) * pixelA.x1();
  float pvf = start * pixelB.x2() + (1 - start) * pixelA.x2();
  float pxf = end * pixelB.x1() + (1 - end) * pixelA.x1();
  float pyf = end * pixelB.x2() + (1 - end) * pixelA.x2();

  float deltaX = pxf - puf;
  float deltaY = pyf - pvf;
  KDCoordinate circleDiameter = thick ? thickCircleDiameter : thinCircleDiameter;
  float normsRatio = std::sqrt(deltaX*deltaX + deltaY*deltaY) / (circleDiameter / 2.0f);
  float stepX = deltaX / normsRatio ;
  float stepY = deltaY / normsRatio;
  int numberOfStamps = std::floor(normsRatio);
  for (int i = 0; i < numberOfStamps; i++) {
    stamp(ctx, rect, Coordinate2D<float>(puf, pvf), color, thick);
    puf += stepX;
    pvf += stepY;
  }
}

void AbstractPlotView::stamp(KDContext * ctx, KDRect rect, Coordinate2D<float> p, KDColor color, bool thick) const {
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

  KDCoordinate stampSize = thick ? thickStampSize : thinStampSize;
  float pxf = p.x1() - 0.5f * stampSize;
  float pyf = p.x2() - 0.5f * stampSize;
  KDCoordinate px = std::ceil(pxf);
  KDCoordinate py = std::ceil(pyf);
  KDRect stampRect(px, py, stampSize, stampSize);
  if (!rect.intersects(stampRect)) {
    return;
  }

  KDColor workingBuffer[stampSize*stampSize];
  float dx = px - pxf;
  float dy = py - pyf;
  size_t ix = std::round(dx * k_shiftedSteps);
  size_t iy = std::round(dy * k_shiftedSteps);
  const uint8_t * shiftedMask;
  if (thick) {
    shiftedMask = &thickShiftedMasks[ix][iy].m_mask[0];
  } else {
    shiftedMask = &thinShiftedMasks[ix][iy].m_mask[0];
  }
  ctx->blendRectWithMask(stampRect, color, shiftedMask, workingBuffer);
}

bool AbstractPlotView::pointsInSameStamp(Coordinate2D<float> p1, Coordinate2D<float> p2, bool thick) const {
  KDCoordinate diameter = thick ? thickCircleDiameter : thinCircleDiameter;
  const float deltaX = p1.x1() - p2.x1();
  const float deltaY = p1.x2() - p2.x2();
  return deltaX * deltaX + deltaY * deltaY < 0.25f * diameter * diameter;
}

}
