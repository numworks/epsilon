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

  KDCoordinate startingI = m_clippingRect.x() - rect.translatedBy(m_origin).x();
  KDCoordinate startingJ = m_clippingRect.y() - rect.translatedBy(m_origin).y();
  startingI = std::max<KDCoordinate>(0, startingI);
  startingJ = std::max<KDCoordinate>(0, startingJ);

  /* If the rect has indeed been clipped, we only want to push the correct
   * discontinuous extract of pixels. We want also to minimize calls to pushRect
   * (time consuming). If a working buffer is available, we can fill it by
   * concatenating extracted rows of 'pixels' to call pushRect only once on the
   * absoluteRect. However, if we do not have a working buffer, we push row by
   * row extracts of 'pixels' calling pushRect multiple times. */

  if (workingBuffer == nullptr) {
    for (KDCoordinate j=0; j<absoluteRect.height(); j++) {
      KDRect absoluteRow = KDRect(absoluteRect.x(), absoluteRect.y()+j, absoluteRect.width(), 1);
      KDColor * rowPixels = (KDColor *)pixels+startingI+rect.width()*(startingJ+j);
      pushRect(absoluteRow, rowPixels);
    }
  } else {
    for (KDCoordinate j=0; j<absoluteRect.height(); j++) {
      for (KDCoordinate i=0; i<absoluteRect.width(); i++) {
        workingBuffer[i+absoluteRect.width()*j] = pixels[startingI+i+rect.width()*(startingJ+j)];
      }
    }
    pushRect(absoluteRect, workingBuffer);
  }
}

void KDContext::fillRectWithMask(KDRect rect, KDColor color, KDColor background, const uint8_t * mask, KDColor * workingBuffer) {
  KDRect absoluteRect = absoluteFillRect(rect);

  /* Caution:
   * The absoluteRect may have a SMALLER size than the original rect because it
   * has been clipped. Therefore we cannot assume that the mask can be read as a
   * continuous area. */

  KDCoordinate startingI = m_clippingRect.x() - rect.translatedBy(m_origin).x();
  KDCoordinate startingJ = m_clippingRect.y() - rect.translatedBy(m_origin).y();
  startingI = std::max<KDCoordinate>(0, startingI);
  startingJ = std::max<KDCoordinate>(0, startingJ);
  for (KDCoordinate j=0; j<absoluteRect.height(); j++) {
    for (KDCoordinate i=0; i<absoluteRect.width(); i++) {
      KDColor * currentPixelAddress = workingBuffer + i + absoluteRect.width()*j;
      const uint8_t * currentMaskAddress = mask + i + startingI + rect.width()*(j + startingJ);
      *currentPixelAddress = KDColor::Blend(background, color, *currentMaskAddress);
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
  KDCoordinate startingI = m_clippingRect.x() - rect.translatedBy(m_origin).x();
  KDCoordinate startingJ = m_clippingRect.y() - rect.translatedBy(m_origin).y();
  startingI = std::max<KDCoordinate>(0, startingI);
  startingJ = std::max<KDCoordinate>(0, startingJ);
  for (KDCoordinate j=0; j<absoluteRect.height(); j++) {
    for (KDCoordinate i=0; i<absoluteRect.width(); i++) {
      KDColor * currentPixelAddress = workingBuffer + i + absoluteRect.width()*j;
      const uint8_t * currentMaskAddress = mask + i + startingI + rect.width()*(j + startingJ);
      *currentPixelAddress = KDColor::Blend(*currentPixelAddress, color, *currentMaskAddress);
      //*currentPixelAddress = KDColorBlend(*currentPixelAddress, color, *currentMaskAddress);
    }
  }
  pushRect(absoluteRect, workingBuffer);
}

void KDContext::strokeRect(KDRect rect, KDColor color) {
  fillRect(KDRect(rect.origin(), rect.width(), 1), color);
  fillRect(KDRect(KDPoint(rect.x(), rect.bottom()), rect.width(), 1), color);
  fillRect(KDRect(rect.origin(), 1, rect.height()), color);
  fillRect(KDRect(KDPoint(rect.right(), rect.y()), 1, rect.height()), color);
}
