#pragma once

#include <escher/view.h>

namespace Escher {

/* A simple view filled with a single color */
class SolidColorView : public View {
 public:
  SolidColorView(KDColor color);
  void reload();
  virtual void setColor(KDColor color);
  void drawRect(KDContext* ctx, KDRect rect) const override;

 protected:
#if ESCHER_VIEW_LOGGING
  const char* className() const override;
  void logAttributes(std::ostream& os) const override;
#endif
 private:
  KDColor m_color;
};

/* A simple view filled with a single color, with a border of customizable width
 * and color */
class SolidColorWithBorderView : public View {
 public:
  SolidColorWithBorderView(KDColor color, KDColor border,
                           KDCoordinate borderWidth);

  void setColors(KDColor inside, KDColor border);
  void setBorderWidth(KDCoordinate width) {
    assert(width >= 0);
    m_borderWidth = width;
  }
  void drawRect(KDContext* ctx, KDRect rect) const override;

 protected:
#if ESCHER_VIEW_LOGGING
  const char* className() const override;
  void logAttributes(std::ostream& os) const override;
#endif

 private:
  KDColor m_color;
  KDColor m_border;
  KDCoordinate m_borderWidth;
};

}  // namespace Escher
