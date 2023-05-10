#ifndef KANDINSKY_POSTPROCESS_GAMMA_CONTEXT_H
#define KANDINSKY_POSTPROCESS_GAMMA_CONTEXT_H

#include <kandinsky/postprocess_context.h>
#include <stdint.h>

class KDPostProcessGammaContext : public KDPostProcessContext {
public:
  KDPostProcessGammaContext();
  void gamma(float& red, float& green, float& blue);
  void gamma(int& red, int& green, int& blue);
  void setGamma(int red, int green, int blue);
private:
  void pushRect(KDRect rect, const KDColor * pixels) override;
  void pushRectUniform(KDRect rect, KDColor color) override;
  void pullRect(KDRect rect, KDColor * pixels) override;
  KDColor correctColor(KDColor color);
  int m_redGamma, m_greenGamma, m_blueGamma;
  // Lookup tables to do gamma correction
  // 5 bits of red
  uint8_t m_redGammaTable[32];
  // 6 bits of green
  uint8_t m_greenGammaTable[64];
  // 5 bits of blue
  uint8_t m_blueGammaTable[32];
  void updateGammaTables();
};

#endif
