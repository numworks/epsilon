#ifndef KANDINSKY_POSTPROCESS_ZOOM_CONTEXT_H
#define KANDINSKY_POSTPROCESS_ZOOM_CONTEXT_H

#include <kandinsky/postprocess_context.h>

class KDPostProcessZoomContext : public KDPostProcessContext {
public:
  KDPostProcessZoomContext();
  KDRect viewingArea() const { return m_viewingArea; }
  void setViewingArea(KDRect viewingArea) { m_viewingArea = viewingArea; }
  KDRect targetArea() const { return m_targetArea; }
  void setTargetArea(KDRect targetArea) { m_targetArea = targetArea; }
private:
  void pushRect(KDRect rect, const KDColor * pixels) override;
  void pushRectUniform(KDRect rect, KDColor color) override;
  void pullRect(KDRect rect, KDColor * pixels) override;
  KDRect m_viewingArea, m_targetArea;
};

#endif
