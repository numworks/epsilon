#include <kandinsky/framebuffer.h>
#include <string.h>

KDFrameBuffer::KDFrameBuffer(KDColor * pixels, KDSize size) :
  m_pixels(pixels),
  m_size(size)
{
}

KDRect KDFrameBuffer::bounds() {
  return KDRect(KDPointZero, m_size);
}

KDColor * KDFrameBuffer::pixelAddress(KDPoint p) const {
    return m_pixels + p.x() + p.y()*m_size.width();
}

void KDFrameBuffer::pushRect(KDRect rect, const KDColor * pixels) {
  const KDColor * line = pixels;
  for (KDCoordinate j=0; j<rect.height(); j++) {
    KDPoint lineOffset = KDPoint(0, j);
    memcpy(pixelAddress(rect.origin().translatedBy(lineOffset)),
        line,
        rect.width()*sizeof(KDColor));
    line += rect.width();
  }
}

void KDFrameBuffer::pushRectUniform(KDRect rect, KDColor color) {
  for (KDCoordinate i=0; i<rect.width(); i++) {
    for (KDCoordinate j=0; j<rect.height(); j++) {
      *pixelAddress(rect.origin().translatedBy(KDPoint(i,j))) = color;
    }
  }
}

void KDFrameBuffer::pullRect(KDRect rect, KDColor * pixels) {
  KDColor * line = pixels;
  for (KDCoordinate j=0; j<rect.height(); j++) {
    KDPoint lineOffset = KDPoint(0, j);
    memcpy(line,
        pixelAddress(rect.origin().translatedBy(lineOffset)),
        rect.width()*sizeof(KDColor));
    line += rect.width();
  }
}
