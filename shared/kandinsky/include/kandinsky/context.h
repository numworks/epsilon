#pragma once

#include <kandinsky/glyph.h>
#include <kandinsky/rect.h>

class KDContext {
 public:
  KDPoint origin() const { return m_origin; }
  KDRect clippingRect() const { return m_clippingRect; }
  void setOrigin(KDPoint origin) { m_origin = origin; }
  void setClippingRect(KDRect clippingRect) { m_clippingRect = clippingRect; }

  // Pixel manipulation
  void setPixel(KDPoint p, KDColor c);
  void getPixel(KDPoint p, KDColor* pixel);
  void getPixels(KDRect r, KDColor* pixels);

  // Text
  KDPoint drawString(const char* text, KDPoint p, KDGlyph::Style style = {},
                     int maxLength = -1);
  KDPoint alignAndDrawString(const char* text, KDPoint p, KDSize frame,
                             KDGlyph::Format format = {}, int maxLength = -1,
                             KDCoordinate lineSpacing = 0);

  // Line. Not anti-aliased.
  void drawLine(KDPoint p1, KDPoint p2, KDColor c);
  void drawAntialiasedLine(KDPoint p1, KDPoint p2, KDColor c,
                           KDColor background) {
    privateDrawAntialiasedLine(p1.x(), p1.y(), p2.x(), p2.y(), c, false,
                               background);
  }
  void drawAntialiasedLine(float x1, float y1, float x2, float y2, KDColor c,
                           KDColor background) {
    privateDrawAntialiasedLine(x1, y1, x2, y2, c, false, background);
  }
  /* Draws an anti-aliased line from [(x1,y1)] to [(x2,y2)] of color [c].
   * The anti-aliased effect is achieved by blending into the existing
   * background.
   * NOTE: if the line is drawn on a uniform background, use
   * [drawAntialiasedLine] instead */
  void drawAntialiasedLineAutoBackground(float x1, float y1, float x2, float y2,
                                         KDColor c) {
    privateDrawAntialiasedLine(x1, y1, x2, y2, c, true);
  }

  // Rect
  void fillRect(KDRect rect, KDColor color);
  void fillRectWithPixels(KDRect rect, const KDColor* pixels,
                          KDColor* workingBuffer);
  void fillRectWithMask(KDRect rect, KDColor color, KDColor background,
                        const uint8_t* mask, KDColor* workingBuffer,
                        bool horizontalFlip = false, bool verticalFlip = false);
  void blendRectWithMask(KDRect rect, KDColor color, const uint8_t* mask,
                         KDColor* workingBuffer);
  void strokeRect(KDRect rect, KDColor color);

  // Circle
  void fillAntialiasedCircle(KDPoint topLeft, KDCoordinate radius,
                             KDColor color, KDColor background) {
    fillCircleWithStripes(topLeft, radius, color, background, 0);
  }
  void fillCircleWithStripes(KDPoint topLeft, KDCoordinate radius,
                             KDColor color, KDColor background,
                             KDCoordinate spacing, bool ascending = true);

 protected:
  KDContext(KDPoint origin, KDRect clippingRect)
      : m_origin(origin), m_clippingRect(clippingRect) {}
  virtual void pushRect(KDRect, const KDColor* pixels) = 0;
  virtual void pushRectUniform(KDRect rect, KDColor color) = 0;
  virtual void pullRect(KDRect rect, KDColor* pixels) = 0;
  KDRect relativeRect(KDRect rect);

 private:
  void privateDrawAntialiasedLine(float x1, float y1, float x2, float y2,
                                  KDColor lineColor, bool autoBackground,
                                  KDColor background = KDColorWhite);
  KDPoint alignAndDrawSingleLineString(const char* text, KDPoint p,
                                       KDSize frame, float horizontalAlignment,
                                       KDGlyph::Style style, int maxLength);
  KDRect absoluteFillRect(KDRect rect);
  KDPoint m_origin;
  KDRect m_clippingRect;
};
