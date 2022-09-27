#include "plot_view.h"
#include <algorithm>
#include <cmath>

using namespace Escher;
using namespace Poincare;

namespace Shared {

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
  assert(dashSize > 0);
  for (KDCoordinate i = a; i < b; i += 2 * dashSize) {
    KDRect rectangle = parallel == Axis::Horizontal ? KDRect(i, p, dashSize, thickness) : KDRect(p, i, thickness, dashSize);
    ctx->fillRect(rectangle.intersectedWith(rect), color);
  }
}

void AbstractPlotView::drawSegment(KDContext * ctx, KDRect rect, Coordinate2D<float> a, Coordinate2D<float> b, KDColor color, bool thick) const {
  Coordinate2D<float> pa = floatToPixel2D(a);
  Coordinate2D<float> pb = floatToPixel2D(b);
  straightJoinDots(ctx, rect, pa, pb, color, thick);
}

static float relativePositionToOffset(AbstractPlotView::RelativePosition position) {
  switch (position) {
  case AbstractPlotView::RelativePosition::Before:
    return -1.f;
  case AbstractPlotView::RelativePosition::There:
    return -0.5f;
  default:
    assert(position == AbstractPlotView::RelativePosition::After);
    return 0.f;
  }
}

void AbstractPlotView::drawLabel(KDContext * ctx, KDRect rect, const char * label, Poincare::Coordinate2D<float> xy, RelativePosition xPosition, RelativePosition yPosition, KDColor color) const {
  KDSize labelSize = KDFont::Font(k_font)->stringSize(label);

  Coordinate2D<float> p = floatToPixel2D(xy);
  KDCoordinate x = std::round(p.x1() + relativePositionToOffset(xPosition) * labelSize.width());
  KDCoordinate y = std::round(p.x2() + relativePositionToOffset(yPosition) * labelSize.height());
  KDPoint labelOrigin(x, y);

  if (KDRect(labelOrigin, labelSize).intersects(rect)) {
    ctx->drawString(label, labelOrigin, k_font, color, k_backgroundColor);
  }
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
    banner->setFrame(bannerFrame(), force);
  }
  View * cursor = cursorView();
  if (cursor) {
    cursor->setFrame(cursorFrame(), force);
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
  if (p1 == p2) {
    if (p1 < lower || upper < p1) {
      *start = 1;
      *end = 0;
    }
  } else {
    *start = std::max(*start, ((p2 < p1 ? lower : upper) - p2) / (p1 - p2));
    *end = std::max(*end, ((p1 < p2 ? lower : upper) - p2) / (p1 - p2));
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
  float puf = start * pixelA.x1() + (1 - start) * pixelB.x1();
  float pvf = start * pixelA.x2() + (1 - start) * pixelB.x2();
  float pxf = end * pixelA.x1() + (1 - end) * pixelB.x1();
  float pyf = end * pixelA.x2() + (1 - end) * pixelB.x2();

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
