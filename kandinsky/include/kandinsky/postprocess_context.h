#ifndef KANDINSKY_POSTPROCESS_CONTEXT_H
#define KANDINSKY_POSTPROCESS_CONTEXT_H

#include <kandinsky/context.h>

class KDPostProcessContext : public KDContext {
public:
  virtual void setOrigin(KDPoint origin) override;
  virtual void setClippingRect(KDRect clippingRect) override;
  void setTarget(KDContext * context);
protected:
  KDPostProcessContext();
  virtual void pushRect(KDRect rect, const KDColor * pixels) override;
  virtual void pushRectUniform(KDRect rect, KDColor color) override;
  virtual void pullRect(KDRect rect, KDColor * pixels) override;
private:
  KDContext * m_target;
};

#endif
