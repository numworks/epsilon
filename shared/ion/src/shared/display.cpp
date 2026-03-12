#include <ion/display.h>
#include <omg/global_box.h>

namespace Ion {
namespace Display {

KDPoint s_origin = KDPointZero;
uint8_t s_ratio = 1;

/* Focus setters */

void setFocusOrigin(KDPoint origin) { s_origin = origin; }

void setFocusRatio(uint8_t r) { s_ratio = r; }

/* Focus helpers */

// The focused rect is the rect displayed on the screen
KDRect FocusedRect() {
  return KDRect(s_origin, KDSize(Width / s_ratio, Height / s_ratio));
}

// From a software KDPoint to the one displayed on the screen
KDPoint ToFocusedPoint(KDPoint p) {
  assert(FocusedRect().contains(p));
  return KDPoint((p.x() - s_origin.x()) * s_ratio,
                 (p.y() - s_origin.y()) * s_ratio);
}

// From a software KDRect to the one displayed on the screen
KDRect ToFocusedRect(KDRect r) {
  if (r == KDRectZero) {
    return r;
  }
  assert(FocusedRect().containsRect(r));
  return KDRect(ToFocusedPoint(r.origin()), r.width() * s_ratio,
                r.height() * s_ratio);
}

// View is focused if the ratio is superior to 1
bool IsFocused() {
  // s_origin should always fit the screen.
  assert(s_ratio > 0);
  assert(Rect.containsRect(FocusedRect()));
  return s_ratio > 1;
}

/* Display methods */

void pushRect(KDRect r, const KDColor* pixels) {
  if (!IsFocused()) {
    return pushRectNoFocus(r, pixels);
  }
  KDRect visibleRect = r.intersectedWith(FocusedRect());
  KDCoordinate iStart = visibleRect.x() - r.x();
  KDCoordinate jStart = visibleRect.y() - r.y();
  // Only push the pixels that are in focus.
  for (int j = jStart; j < jStart + visibleRect.height(); j++) {
    for (int i = iStart; i < iStart + visibleRect.width(); i++) {
      KDColor color = pixels[j * r.width() + i];
      KDRect focusedPixelRect =
          ToFocusedRect(KDRect(i + r.x(), j + r.y(), 1, 1));
      pushRectUniformNoFocus(focusedPixelRect, color);
    }
  }
}

void pushRectUniform(KDRect r, KDColor c) {
  if (!IsFocused()) {
    return pushRectUniformNoFocus(r, c);
  }
  r = r.intersectedWith(FocusedRect());
  pushRectUniformNoFocus(ToFocusedRect(r), c);
}

void pullRect(KDRect r, KDColor* pixels) {
  if (!IsFocused()) {
    return pullRectNoFocus(r, pixels);
  }
  for (int j = 0; j < r.height(); j++) {
    for (int i = 0; i < r.width(); i++) {
      // If pixel isn't on screen because of focus, pull a black pixel.
      KDColor color = KDBlack;
      KDPoint pixelPoint = KDPoint(i + r.x(), j + r.y());
      if (FocusedRect().contains(pixelPoint)) {
        pullRectNoFocus(KDRect(ToFocusedPoint(pixelPoint), 1, 1), &color);
      }
      *pixels++ = color;
    }
  }
}

}  // namespace Display
}  // namespace Ion
