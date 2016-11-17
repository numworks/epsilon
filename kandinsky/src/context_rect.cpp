#include <kandinsky/context.h>
#include <assert.h>

KDRect KDContext::absoluteFillRect(KDRect rect) {
  return rect.translatedBy(m_origin).intersectedWith(m_clippingRect);
}

void KDContext::fillRect(KDRect rect, KDColor color) {
  KDRect absoluteRect = absoluteFillRect(rect);
  if (absoluteRect.isEmpty()) {
    return;
  }
  pushRectUniform(absoluteRect, color);
}

/* Note: we support the case where workingBuffer IS equal to pixels */
void KDContext::fillRectWithPixels(KDRect rect, const KDColor * pixels, KDColor * workingBuffer) {
  KDRect absoluteRect = absoluteFillRect(rect);

  if (absoluteRect.isEmpty()) {
    return;
  }

  /* Caution:
   * The absoluteRect may have a SMALLER size than the original rect because it
   * has been clipped. Therefore we cannot assume that the mask can be read as a
   * continuous area. */

  if (absoluteRect.width() == rect.width() && absoluteRect.height() == rect.height()) {
    pushRect(absoluteRect, pixels);
    return;
  }

  /* At this point we need the working buffer */
  assert(workingBuffer != nullptr);
  for (KDCoordinate j=0; j<absoluteRect.height(); j++) {
    for (KDCoordinate i=0; i<absoluteRect.width(); i++) {
      workingBuffer[i+absoluteRect.width()*j] = pixels[i+rect.width()*j];
    }
  }
  pushRect(absoluteRect, workingBuffer);
}

// Mask's size must be rect.size
// WorkingBuffer, same deal
// TODO: should we avoid pullRect by giving a 'memory' working buffer?
void KDContext::blendRectWithMask(KDRect rect, KDColor color, const uint8_t * mask, KDColor * workingBuffer) {
  KDRect absoluteRect = absoluteFillRect(rect);

  /* Caution:
   * The absoluteRect may have a SMALLER size than the original rect because it
   * has been clipped. Therefore we cannot assume that the mask can be read as a
   * continuous area. */

  pullRect(absoluteRect, workingBuffer);
  for (KDCoordinate j=0; j<absoluteRect.height(); j++) {
    for (KDCoordinate i=0; i<absoluteRect.width(); i++) {
      KDColor * currentPixelAdress = workingBuffer + i + absoluteRect.width()*j;
      const uint8_t * currentMaskAddress = mask + i + rect.width()*j;
      *currentPixelAdress = KDColor::blend(*currentPixelAdress, color, *currentMaskAddress);
      //*currentPixelAdress = KDColorBlend(*currentPixelAdress, color, *currentMaskAddress);
    }
  }
  pushRect(absoluteRect, workingBuffer);
}

void KDContext::drawRect(KDRect rect, KDColor color) {
  KDPoint p1 = rect.origin();
  KDPoint p2 = KDPoint(rect.x(), rect.bottom());
  for (int i = 0; i<rect.width(); i++) {
    setPixel(p1, color);
    setPixel(p2, color);
    p1 = p1.translatedBy(KDPoint(1,0));
    p2 = p2.translatedBy(KDPoint(1,0));
  }
  p1 = rect.origin();
  p2 = KDPoint(rect.right(), rect.y());
  for (int i = 0; i<rect.height(); i++) {
    setPixel(p1, color);
    setPixel(p2, color);
    p1 = p1.translatedBy(KDPoint(0,1));
    p2 = p2.translatedBy(KDPoint(0,1));
  }
}

