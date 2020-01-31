#ifndef KANDINSKY_CONTEXT_H
#define KANDINSKY_CONTEXT_H

#include <kandinsky/color.h>
#include <kandinsky/rect.h>
#include <kandinsky/font.h>

class KDContext {
public:
  void setOrigin(KDPoint origin) { m_origin = origin; }
  void setClippingRect(KDRect clippingRect) { m_clippingRect = clippingRect; }

  // Pixel manipulation
  void setPixel(KDPoint p, KDColor c);
  void getPixel(KDPoint p, KDColor * pixel);
  void getPixels(KDRect r, KDColor * pixels);

  // Text
  KDPoint drawString(const char * text, KDPoint p, const KDFont * font = KDFont::LargeFont, KDColor textColor = KDColorBlack, KDColor backgroundColor = KDColorWhite, int maxLength = -1);
  // Check that a string is drawn.
  int checkDrawnString(const char * text, KDPoint p, const KDFont * font = KDFont::LargeFont, KDColor textColor = KDColorBlack, KDColor backgroundColor = KDColorWhite, int maxLength = -1);

  // Line. Not anti-aliased.
  void drawLine(KDPoint p1, KDPoint p2, KDColor c);

  // Rect
  void fillRect(KDRect rect, KDColor color);
  void fillRectWithPixels(KDRect rect, const KDColor * pixels, KDColor * workingBuffer);
  void blendRectWithMask(KDRect rect, KDColor color, const uint8_t * mask, KDColor * workingBuffer);
  void strokeRect(KDRect rect, KDColor color);
protected:
  KDContext(KDPoint origin, KDRect clippingRect) :
    m_origin(origin),
    m_clippingRect(clippingRect)
  {}
  virtual void pushRect(KDRect, const KDColor * pixels) = 0;
  virtual void pushRectUniform(KDRect rect, KDColor color) = 0;
  virtual void pullRect(KDRect rect, KDColor * pixels) = 0;
private:
  KDRect absoluteFillRect(KDRect rect);
  KDPoint pushOrPullString(const char * text, KDPoint p, const KDFont * font, KDColor textColor, KDColor backgroundColor, int maxByteLength, bool push, int * result = nullptr);
  KDPoint m_origin;
  KDRect m_clippingRect;
};

#endif
