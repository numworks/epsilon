#include <assert.h>
#include <kandinsky/framebuffer.h>
#include <string.h>

KDFrameBuffer::KDFrameBuffer(KDColor* pixels, KDSize size)
    : m_pixels(pixels), m_size(size) {}

KDRect KDFrameBuffer::bounds() { return KDRect(KDPointZero, m_size); }

KDColor* KDFrameBuffer::pixelAddress(KDPoint p) const {
  assert(p.x() >= 0 && p.y() >= 0 && m_size.isValid());
  assert(p.x() < m_size.width() && p.y() < m_size.height());
  return m_pixels + p.x() + p.y() * m_size.width();
}

void KDFrameBuffer::pushRect(KDRect rect, const KDColor* pixels) {
  assert(rect.isValid());
  const KDColor* line = pixels;
  for (KDCoordinate j = 0; j < rect.height(); j++) {
    KDPoint lineOffset = KDPoint(0, j);
    memcpy(pixelAddress(rect.origin().translatedBy(lineOffset)), line,
           rect.width() * sizeof(KDColor));
    line += rect.width();
  }
}

void KDFrameBuffer::pushRectUniform(KDRect rect, KDColor color) {
  assert(rect.isValid());
  // Caution: this code is used very frequently. It's worth optimizing!
  KDColor* pixel = pixelAddress(rect.origin());
  int lineDelta = m_size.width() - rect.width();
  for (KDCoordinate j = 0; j < rect.height(); j++) {
    /* KDColor * topLeftPixelAddress =
     * pixelAddress(rect.origin().translatedBy(KDPoint(0,j)));
     * *pixelAddress(rect.origin().translatedBy(KDPoint(i,j))) = color; */
    for (KDCoordinate i = 0; i < rect.width(); i++) {
      *pixel++ = color;
      //*pixelAddress(rect.origin().translatedBy(KDPoint(i,j))) = color;
    }
    pixel += lineDelta;
  }
}

void KDFrameBuffer::pullRect(KDRect rect, KDColor* pixels) {
  assert(rect.isValid());
  KDColor* line = pixels;
  for (KDCoordinate j = 0; j < rect.height(); j++) {
    KDPoint lineOffset = KDPoint(0, j);
    memcpy(line, pixelAddress(rect.origin().translatedBy(lineOffset)),
           rect.width() * sizeof(KDColor));
    line += rect.width();
  }
}
