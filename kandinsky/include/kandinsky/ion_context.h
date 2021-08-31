#ifndef KANDINSKY_ION_CONTEXT_H
#define KANDINSKY_ION_CONTEXT_H

#include <kandinsky/context.h>
#include <kandinsky/postprocess_gamma_context.h>
#include <kandinsky/postprocess_invert_context.h>
#include <kandinsky/postprocess_zoom_context.h>

class KDRealIonContext : public KDContext {
public:
  KDRealIonContext();
  void pushRect(KDRect rect, const KDColor * pixels) override;
  void pushRectUniform(KDRect rect, KDColor color) override;
  void pullRect(KDRect rect, KDColor * pixels) override;
};

class KDIonContext : public KDContext {
public:
  static KDIonContext * sharedContext();
  void updatePostProcessingEffects();

  KDPostProcessInvertContext invert;
  KDPostProcessZoomContext zoom;
  KDPostProcessGammaContext gamma;
  bool invertEnabled;
  bool zoomEnabled;
  bool zoomInhibit;
  bool gammaEnabled;
  int zoomPosition;
  static void putchar(char c);
private:
  KDIonContext();
  void pushRect(KDRect rect, const KDColor * pixels) override;
  void pushRectUniform(KDRect rect, KDColor color) override;
  void pullRect(KDRect rect, KDColor * pixels) override;
  KDContext *rootContext;
  KDRealIonContext m_realContext;
};

#endif
