#include <kandinsky/postprocess_gamma_context.h>
#include <ion.h>
#include <math.h>

constexpr int MaxGammaStates = 7;
constexpr float MaxGammaGamut = 0.75;

constexpr float toGamma(int gamma) {
  return 1.f / (1 + (float(gamma) / MaxGammaStates * MaxGammaGamut));
}

constexpr int clampGamma(int gamma) {
  return gamma < -MaxGammaStates ? -MaxGammaStates : (gamma > MaxGammaStates ? MaxGammaStates : gamma);
}

KDPostProcessGammaContext::KDPostProcessGammaContext() :
  m_redGamma(0), m_greenGamma(0), m_blueGamma(0) {}

void KDPostProcessGammaContext::gamma(int& red, int& green, int& blue) {
  red = m_redGamma;
  green = m_greenGamma;
  blue = m_blueGamma;
}

void KDPostProcessGammaContext::gamma(float& red, float& green, float& blue) {
  red = (m_redGamma + MaxGammaStates) / float(MaxGammaStates*2);
  green = (m_greenGamma + MaxGammaStates) / float(MaxGammaStates*2);
  blue = (m_blueGamma + MaxGammaStates) / float(MaxGammaStates*2);
}

void KDPostProcessGammaContext::setGamma(int red, int green, int blue) {
  m_redGamma = clampGamma(red);
  m_greenGamma = clampGamma(green);
  m_blueGamma = clampGamma(blue);
}

void KDPostProcessGammaContext::pushRect(KDRect rect, const KDColor * pixels) {
  const float redGamma = toGamma(m_redGamma);
  const float greenGamma = toGamma(m_greenGamma);
  const float blueGamma = toGamma(m_blueGamma);
  KDColor workingBuffer[rect.width()];

  for (KDCoordinate y = 0; y < rect.height(); y++) {
    KDRect workingRect(rect.x(), rect.y()+y, rect.width(), 1);
  
    for (KDCoordinate x = 0; x < rect.width(); x++) {
      const KDColor color = pixels[y*rect.width()+x];
      const KDColor result = KDColor::RGB888(
        (uint8_t)(powf(color.red()/255.f, redGamma)*255),
        (uint8_t)(powf(color.green()/255.f, greenGamma)*255),
        (uint8_t)(powf(color.blue()/255.f, blueGamma)*255));
      workingBuffer[x] = result;
    }
    KDPostProcessContext::pushRect(workingRect, workingBuffer);
  }
}

void KDPostProcessGammaContext::pushRectUniform(KDRect rect, KDColor color) {
  const float redGamma = toGamma(m_redGamma);
  const float greenGamma = toGamma(m_greenGamma);
  const float blueGamma = toGamma(m_blueGamma);
  const KDColor result = KDColor::RGB888(
      (uint8_t)(powf(color.red()/255.f, redGamma)*255),
      (uint8_t)(powf(color.green()/255.f, greenGamma)*255),
      (uint8_t)(powf(color.blue()/255.f, blueGamma)*255));

  KDPostProcessContext::pushRectUniform(rect, result);
}

void KDPostProcessGammaContext::pullRect(KDRect rect, KDColor * pixels) {
  const float redGamma = 1.f/toGamma(m_redGamma);
  const float greenGamma = 1.f/toGamma(m_greenGamma);
  const float blueGamma = 1.f/toGamma(m_blueGamma);
  KDPostProcessContext::pullRect(rect, pixels);
  for (KDCoordinate y = 0; y < rect.height(); y++) {
    for (KDCoordinate x = 0; x < rect.width(); x++) {
      const KDColor color = pixels[y*rect.width()+x];
      const KDColor result = KDColor::RGB888(
          (uint8_t)(powf(color.red()/255.f, redGamma)*255),
          (uint8_t)(powf(color.green()/255.f,greenGamma)*255),
          (uint8_t)(powf(color.blue()/255.f, blueGamma)*255));
      pixels[y*rect.width()+x] = result;
    }
  }
}
