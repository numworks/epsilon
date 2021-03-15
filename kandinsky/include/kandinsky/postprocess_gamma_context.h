#ifndef KANDINSKY_POSTPROCESS_GAMMA_CONTEXT_H
#define KANDINSKY_POSTPROCESS_GAMMA_CONTEXT_H

#include <kandinsky/postprocess_context.h>

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
  int m_redGamma, m_greenGamma, m_blueGamma;
};

#endif
