#include <kandinsky/postprocess_invert_context.h>
#include <ion.h>

void KDPostProcessInvertContext::pushRect(KDRect rect, const KDColor * pixels) {
  KDColor workingBuffer[rect.width()];

  for (KDCoordinate y = 0; y < rect.height(); y++) {
    KDRect workingRect(rect.x(), rect.y()+y, rect.width(), 1);
  
    for (KDCoordinate x = 0; x < rect.width(); x++) {
      workingBuffer[x] = pixels[y*rect.width()+x].invert();
    }
    KDPostProcessContext::pushRect(workingRect, workingBuffer);
  }
}

void KDPostProcessInvertContext::pushRectUniform(KDRect rect, KDColor color) {
  KDPostProcessContext::pushRectUniform(rect, color.invert());
}

void KDPostProcessInvertContext::pullRect(KDRect rect, KDColor * pixels) {
  KDPostProcessContext::pullRect(rect, pixels);
  for (KDCoordinate y = 0; y < rect.height(); y++) {
    for (KDCoordinate x = 0; x < rect.width(); x++) {
      pixels[y*rect.width()+x] = pixels[y*rect.width()+x].invert();
    }
  }
}
