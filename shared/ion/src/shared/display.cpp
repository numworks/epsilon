#include <assert.h>
#include <ion/display.h>

// This file manages two coordinate spaces:
//
// - Software space: the full logical screen (Width x Height). All KDRects and
//   KDPoints passed in from callers live in this space. They are unaware of
//   the current focus state.
//
// - Hardware space: the actual pixel coordinates sent to the physical display.
//   When a focus is active, only a sub-rectangle of software space is rendered
//   onto the hardware screen, magnified by s_zoomFactor.
//
// The helpers below translate between the two spaces. The "NoFocus" platform
// functions (pushRectNoFocus, pushRectUniformNoFocus, pullRectNoFocus) operate
// exclusively in hardware space.

namespace Ion {
namespace Display {

// Top-left corner of the focused region, expressed in software space.
KDPoint s_focusOrigin = KDPointZero;
// Magnification factor: each software pixel maps to s_zoomFactor×s_zoomFactor
// hardware pixels. 1 means no zoom (focus covers the full screen).
uint8_t s_zoomFactor = 1;

/* Focus helpers */

// Returns the sub-rectangle of software space that is currently visible on
// the hardware screen. Its size equals the hardware screen size divided by
// s_zoomFactor, and its origin is s_focusOrigin (all in software space).
static KDRect FocusViewport() {
  return KDRect(s_focusOrigin,
                KDSize(Width / s_zoomFactor, Height / s_zoomFactor));
}

// Converts a point from software space to hardware space.
// The point must lie inside FocusViewport(); points outside the focused region
// are never displayed on the hardware screen.
static KDPoint ToHardwarePoint(KDPoint p) {
  assert(FocusViewport().contains(p));
  return KDPoint((p.x() - s_focusOrigin.x()) * s_zoomFactor,
                 (p.y() - s_focusOrigin.y()) * s_zoomFactor);
}

// Converts a rectangle from software space to hardware space.
// The rectangle must be fully contained within FocusViewport().
// Each software pixel becomes an s_zoomFactor×s_zoomFactor block on the
// hardware screen.
static KDRect ToHardwareRect(KDRect r) {
  assert(FocusViewport().containsRect(r));
  return KDRect(ToHardwarePoint(r.origin()), r.width() * s_zoomFactor,
                r.height() * s_zoomFactor);
}

// Returns true when a focus is active, i.e. only a sub-region of software
// space is magnified onto the hardware screen.
static bool IsZoomed() { return s_zoomFactor > 1; }

/* Focus setters */

// Activates focus: the hardware screen will display only the software-space
// region starting at `origin` with size (Width/zoomFactor × Height/zoomFactor),
// magnified by `zoomFactor`. `origin` and the resulting viewport must lie
// within the full software-space screen rect.
void setFocus(uint8_t zoomFactor, KDPoint origin) {
  assert(zoomFactor > 0);
  s_zoomFactor = zoomFactor;
  s_focusOrigin = origin;
  assert(Rect.containsRect(FocusViewport()));
}

// Deactivates focus: restores a 1:1 mapping between software and hardware
// space (the full logical screen maps to the full hardware screen).
void resetFocus() { setFocus(1, KDPointZero); }

// Display methods
//
// All public methods below (`pushRect`, `pushRectUniform`, `pullRect`) accept
// rectangles in SOFTWARE space. They clip against the focused region and
// translate to HARDWARE space before calling the platform "NoFocus" functions.

void pushRect(KDRect r, const KDColor* pixels) {
  if (!IsZoomed()) {
    // No focus active: software space == hardware space, forward directly.
    return pushRectNoFocus(r, pixels);
  }
  // Clip the caller's software-space rect to the region visible on the
  // hardware screen. Pixels outside FocusViewport() are never displayed.
  KDRect visibleRect = r.intersectedWith(FocusViewport());
  // Offset within the caller's pixel buffer where the visible region starts.
  KDCoordinate iStart = visibleRect.x() - r.x();
  KDCoordinate jStart = visibleRect.y() - r.y();
  // Each visible software pixel is magnified to an s_zoomFactor×s_zoomFactor
  // block on the hardware screen.
  for (int j = jStart; j < jStart + visibleRect.height(); j++) {
    for (int i = iStart; i < iStart + visibleRect.width(); i++) {
      KDColor color = pixels[j * r.width() + i];
      // Convert the 1×1 software pixel to its hardware-space block.
      KDRect hardwarePixelRect =
          ToHardwareRect(KDRect(i + r.x(), j + r.y(), 1, 1));
      pushRectUniformNoFocus(hardwarePixelRect, color);
    }
  }
}

void pushRectUniform(KDRect r, KDColor c) {
  if (!IsZoomed()) {
    // No focus active: software space == hardware space, forward directly.
    return pushRectUniformNoFocus(r, c);
  }
  // Clip the software-space rect to the visible region on the hardware screen.
  r = r.intersectedWith(FocusViewport());
  if (r.isEmpty()) {
    // ToHardwareRect requires r.origin() to be inside FocusViewport(), but the
    // intersection of two non-overlapping rects is KDRectZero whose origin is
    // outside FocusViewport() - bail out early.
    return;
  }
  // Translate the clipped software-space rect to hardware space and push.
  pushRectUniformNoFocus(ToHardwareRect(r), c);
}

void pullRect(KDRect r, KDColor* pixels) {
  if (!IsZoomed()) {
    // No focus active: software space == hardware space, forward directly.
    return pullRectNoFocus(r, pixels);
  }
  // `r` is in software space; iterate over each software pixel individually.
  for (int j = 0; j < r.height(); j++) {
    for (int i = 0; i < r.width(); i++) {
      // Software pixels outside FocusViewport() have no corresponding hardware
      // pixel (they are off-screen due to focus). Return black for those.
      KDColor color = KDBlack;
      KDPoint pixelPoint = KDPoint(i + r.x(), j + r.y());
      if (FocusViewport().contains(pixelPoint)) {
        // Translate the single software pixel to its hardware-space position
        // and pull the actual color from the hardware screen.
        pullRectNoFocus(KDRect(ToHardwarePoint(pixelPoint), 1, 1), &color);
      }
      *pixels++ = color;
    }
  }
}

}  // namespace Display
}  // namespace Ion
