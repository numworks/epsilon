#include <kandinsky/color.h>
#include <kandinsky/postprocess_gamma_context.h>
#include <ion.h>
#include <math.h>
#include <stdint.h>

constexpr int MaxGammaStates = 7;
constexpr float MaxGammaGamut = 0.75;

constexpr float toGamma(int gamma) {
  return 1.f / (1 + (float(gamma) / MaxGammaStates * MaxGammaGamut));
}

constexpr int clampGamma(int gamma) {
  return gamma < -MaxGammaStates ? -MaxGammaStates : (gamma > MaxGammaStates ? MaxGammaStates : gamma);
}

void KDPostProcessGammaContext::updateGammaTables() {
  const float redGamma = toGamma(m_redGamma);
  const float greenGamma = toGamma(m_greenGamma);
  const float blueGamma = toGamma(m_blueGamma);
  for (int i = 0; i < 32; i++) {
    uint8_t r = (uint8_t)(powf((i << 3) / 255.f, redGamma) * 255);
    m_redGammaTable[i] = r >> 3;
  }
  for (int i = 0; i < 64; i++) {
    uint8_t g = (uint8_t)(powf((i << 2) / 255.f, greenGamma) * 255);
    m_greenGammaTable[i] = g >> 2;
  }
  for (int i = 0; i < 32; i++) {
    uint8_t b = (uint8_t)(powf((i << 3 )/ 255.f, blueGamma) * 255);
    m_blueGammaTable[i] = b >> 3;
  }
}

KDPostProcessGammaContext::KDPostProcessGammaContext() :
  m_redGamma(0), m_greenGamma(0), m_blueGamma(0) {
    updateGammaTables();
  }

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
  updateGammaTables();
}

KDColor KDPostProcessGammaContext::correctColor(KDColor color) {
  uint8_t r5 = (((uint16_t )color)>>11)&0x1F;
  r5 = m_redGammaTable[r5];
  uint8_t g6 = (((uint16_t )color)>>5)&0x3F;
  g6 = m_greenGammaTable[g6];
  uint8_t b5 = ((uint16_t )color)&0x1F;
  b5 = m_blueGammaTable[b5];
  return KDColor::RGB16(r5<<11 | g6<<5 | b5);
}

void KDPostProcessGammaContext::pushRect(KDRect rect, const KDColor * pixels) {
  KDColor workingBuffer[rect.width()];

  for (KDCoordinate y = 0; y < rect.height(); y++) {
    KDRect workingRect(rect.x(), rect.y()+y, rect.width(), 1);
  
    for (KDCoordinate x = 0; x < rect.width(); x++) {
      const KDColor color = pixels[y*rect.width()+x];
      const KDColor result = correctColor(color);
      workingBuffer[x] = result;
    }
    KDPostProcessContext::pushRect(workingRect, workingBuffer);
  }
}

void KDPostProcessGammaContext::pushRectUniform(KDRect rect, KDColor color) {
  KDPostProcessContext::pushRectUniform(rect, correctColor(color));
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
