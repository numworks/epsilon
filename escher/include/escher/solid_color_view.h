#ifndef ESCHER_SOLID_COLOR_VIEW_H
#define ESCHER_SOLID_COLOR_VIEW_H

#include <escher/view.h>

namespace Escher {

class SolidColorView : public View {
 public:
  SolidColorView(KDColor color);
  void reload();
  virtual void setColor(KDColor color);
  void drawRect(KDContext *ctx, KDRect rect) const override;

 protected:
#if ESCHER_VIEW_LOGGING
  const char *className() const override;
  void logAttributes(std::ostream &os) const override;
#endif
 private:
  KDColor m_color;
};

}  // namespace Escher
#endif
