#ifndef KANDINSKY_CONTEXT_H
#define KANDINSKY_CONTEXT_H

#include <kandinsky/color.h>
#include <kandinsky/rect.h>
#include <kandinsky/text.h>

class KDContext {
public:
  void setOrigin(KDPoint origin);
  void setClippingRect(KDRect clippingRect);

  // Pixel manipulation
  void setPixel(KDPoint p, KDColor c);
  KDColor getPixel(KDPoint p);

  // Text
  KDPoint drawString(const char * text, KDPoint p, KDText::FontSize size = KDText::FontSize::Large, KDColor textColor = KDColorBlack, KDColor backgroundColor = KDColorWhite, int maxLength = -1);
  KDPoint blendString(const char * text, KDPoint p, KDText::FontSize size, KDColor textColor = KDColorBlack);

  // Line. Not anti-aliased.
  void drawLine(KDPoint p1, KDPoint p2, KDColor c);

  // Rect
  void fillRect(KDRect rect, KDColor color);
  void fillRectWithPixels(KDRect rect, const KDColor * pixels, KDColor * workingBuffer);
  void blendRectWithMask(KDRect rect, KDColor color, const uint8_t * mask, KDColor * workingBuffer);
  void strokeRect(KDRect rect, KDColor color);
protected:
  KDContext(KDPoint origin, KDRect clippingRect);
  virtual void pushRect(KDRect, const KDColor * pixels) = 0;
  virtual void pushRectUniform(KDRect rect, KDColor color) = 0;
  virtual void pullRect(KDRect rect, KDColor * pixels) = 0;
private:
  KDRect absoluteFillRect(KDRect rect);
  KDPoint writeString(const char * text, KDPoint p, KDText::FontSize size, KDColor textColor, KDColor backgroundColor, int maxLength, bool transparentBackground);
  void writeChar(char character, KDPoint p, KDText::FontSize size, KDColor textColor, KDColor backgroundColor, bool transparentBackground);
  KDPoint m_origin;
  KDRect m_clippingRect;
};

#endif
