#include <kandinsky/postprocess_zoom_context.h>
#include <ion.h>

KDPostProcessZoomContext::KDPostProcessZoomContext() : m_viewingArea(KDRectZero), m_targetArea(KDRectZero)
{
}

void KDPostProcessZoomContext::pushRect(KDRect rect, const KDColor * pixels) {
  auto translatedRect = rect.translatedBy(KDPoint(-m_viewingArea.x(),-m_viewingArea.y()));
  auto targetRect = KDRect(translatedRect.x()*2, translatedRect.y()*2, translatedRect.width()*2, translatedRect.height()*2);
  auto clippedTargetRect = m_targetArea.intersectedWith(targetRect);

  KDColor targetBuffer[targetRect.width()];

  for (int y = 0; y < rect.height(); y++) {
    for (int x = 0; x < rect.width(); x++) {
      targetBuffer[2*x+1] = targetBuffer[2*x] = pixels[y*rect.width()+x];
    }

    for (int i = 0; i < 2; i++) {
      auto outputRect = KDRect(targetRect.x(), targetRect.y()+y*2+i, targetRect.width(), 1);
      KDPostProcessContext::pushRect(m_targetArea.intersectedWith(outputRect), targetBuffer+(clippedTargetRect.x()-targetRect.x()));
    }
  }
}

void KDPostProcessZoomContext::pushRectUniform(KDRect rect, KDColor color) {
  auto clippedRect = m_viewingArea.intersectedWith(rect);
  auto targetRect = KDRect(clippedRect.x()*2, clippedRect.y()*2, clippedRect.width()*2, clippedRect.height()*2);
  targetRect = targetRect.translatedBy(KDPoint(-m_viewingArea.x()*2,-m_viewingArea.y()*2));
  targetRect = m_targetArea.intersectedWith(targetRect);

  KDPostProcessContext::pushRectUniform(targetRect, color);
}

void KDPostProcessZoomContext::pullRect(KDRect rect, KDColor * pixels) {
  auto translatedRect = rect.translatedBy(KDPoint(-m_viewingArea.x(),-m_viewingArea.y()));
  auto targetRect = KDRect(translatedRect.x()*2, translatedRect.y()*2, translatedRect.width()*2, translatedRect.height()*2);
  auto clippedTargetRect = m_targetArea.intersectedWith(targetRect);

  KDColor targetBuffer[targetRect.width()];

  for (int y = 0; y < rect.height(); y++) {
    memset(targetBuffer, 0x00, sizeof(targetBuffer));

    auto outputRect = KDRect(targetRect.x(), targetRect.y()+y*2, targetRect.width(), 1).intersectedWith(m_targetArea);
    KDPostProcessContext::pullRect(outputRect, targetBuffer+(clippedTargetRect.x()-targetRect.x()));

    for (int x = 0; x < rect.width(); x++) {
      pixels[y*rect.width()+x] = targetBuffer[x*2];
    }
  }
}
