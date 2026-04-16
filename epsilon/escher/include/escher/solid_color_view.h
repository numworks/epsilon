#pragma once

#include <escher/theme/components.h>
#include <escher/view.h>

namespace Escher {

/* A simple view filled with a single color. ColorType is either ThemeComponent
 * or KDColor */
template <typename ColorType>
class SolidColorViewBase : public View {
 public:
  SolidColorViewBase(ColorType color);
  void reload();
  virtual void setColor(ColorType color);
  void drawRect(KDContext* ctx, KDRect rect) const override;

 protected:
#if ESCHER_VIEW_LOGGING
  const char* className() const override;
  void logAttributes(std::ostream& os) const override;
#endif
 private:
  ColorType m_color;
};

using SolidColorView = SolidColorViewBase<ThemeComponent>;
using RawColorSolidColorView = SolidColorViewBase<KDColor>;

/* A simple view filled with a single color, with a border of customizable width
 * and color. ColorType is either ThemeComponent or KDColor */
template <typename ColorType>
class SolidColorWithBorderViewBase : public View {
 public:
  SolidColorWithBorderViewBase(ColorType color, ColorType border,
                               KDCoordinate borderWidth);

  void setColors(ColorType inside, ColorType border);
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
  ColorType m_color;
  ColorType m_border;
  KDCoordinate m_borderWidth;
};

using SolidColorWithBorderView = SolidColorWithBorderViewBase<ThemeComponent>;
using RawColorSolidColorWithBorderView = SolidColorWithBorderViewBase<KDColor>;

}  // namespace Escher
