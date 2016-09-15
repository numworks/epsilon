#ifndef KANDINSKY_CONTEXT_H
#define KANDINSKY_CONTEXT_H

#include <kandinsky/color.h>
#include <kandinsky/rect.h>

class KDContext {
public:
  void setOrigin(KDPoint origin);
  void setClippingRect(KDRect clippingRect);

  // Pixel manipulation
  void setPixel(KDPoint p, KDColor c);
  KDColor getPixel(KDPoint p);

  // Text
  void drawChar(char character, KDPoint p, KDColor textColor = KDColorBlack, KDColor backgroundColor = KDColorWhite);
  void drawString(const char * text, KDPoint p, KDColor textColor = KDColorBlack, KDColor backgroundColor = KDColorWhite);

  // Line. Not anti-aliased.
  void drawLine(KDPoint p1, KDPoint p2, KDColor c);

  // Rect
  void fillRect(KDRect rect, KDColor color);
  void fillRectWithPixels(KDRect rect, const KDColor * pixels, KDColor * workingBuffer);
  void fillRectWithMask(KDRect rect, KDColor color, const uint8_t * mask, KDColor * workingBuffer);
  void drawRect(KDRect rect, KDColor color);
protected:
  KDContext(KDPoint origin, KDRect clippingRect);
  virtual void pushRect(KDRect, const KDColor * pixels) = 0;
  virtual void pushRectUniform(KDRect rect, KDColor color) = 0;
  virtual void pullRect(KDRect rect, KDColor * pixels) = 0;
private:
  KDRect absoluteFillRect(KDRect rect);
  KDPoint m_origin;
  KDRect m_clippingRect;
};

#endif
