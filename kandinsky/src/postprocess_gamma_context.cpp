#include <kandinsky/postprocess_gamma_context.h>
#include <ion.h>
#include <math.h>

void KDPostProcessGammaContext::setGamma(float red, float blue, float green) {
  m_redGamma = 1.f/red;
  m_greenGamma = 1.f/green;
  m_blueGamma = 1.f/blue;
}

void KDPostProcessGammaContext::pushRect(KDRect rect, const KDColor * pixels) {
  KDColor workingBuffer[rect.width()];

  for (KDCoordinate y = 0; y < rect.height(); y++) {
    KDRect workingRect(rect.x(), rect.y()+y, rect.width(), 1);
  
    for (KDCoordinate x = 0; x < rect.width(); x++) {
      const KDColor color = pixels[y*rect.width()+x];
      const KDColor result = KDColor::RGB888(
        (uint8_t)(powf(color.red()/255.f, m_redGamma)*255),
        (uint8_t)(powf(color.green()/255.f, m_greenGamma)*255),
        (uint8_t)(powf(color.blue()/255.f, m_blueGamma)*255));
      workingBuffer[x] = result;
    }
    KDPostProcessContext::pushRect(workingRect, workingBuffer);
  }
}

void KDPostProcessGammaContext::pushRectUniform(KDRect rect, KDColor color) {
  const KDColor result = KDColor::RGB888(
      (uint8_t)(powf(color.red()/255.f, m_redGamma)*255),
      (uint8_t)(powf(color.green()/255.f, m_greenGamma)*255),
      (uint8_t)(powf(color.blue()/255.f, m_blueGamma)*255));

  KDPostProcessContext::pushRectUniform(rect, result);
}

void KDPostProcessGammaContext::pullRect(KDRect rect, KDColor * pixels) {
  const float invRedGamma = 1.f/m_redGamma;
  const float invGreenGamma = 1.f/m_greenGamma;
  const float invBlueGamma = 1.f/m_blueGamma;

  KDPostProcessContext::pullRect(rect, pixels);
  for (KDCoordinate y = 0; y < rect.height(); y++) {
    for (KDCoordinate x = 0; x < rect.width(); x++) {
      const KDColor color = pixels[y*rect.width()+x];
      const KDColor result = KDColor::RGB888(
          (uint8_t)(powf(color.red()/255.f, invRedGamma)*255),
          (uint8_t)(powf(color.green()/255.f, invGreenGamma)*255),
          (uint8_t)(powf(color.blue()/255.f, invBlueGamma)*255));
      pixels[y*rect.width()+x] = result;
    }
  }
}
