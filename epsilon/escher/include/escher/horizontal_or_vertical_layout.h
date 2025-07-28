#ifndef ESCHER_HORIZONTAL_OR_VERTICAL_LAYOUT_H
#define ESCHER_HORIZONTAL_OR_VERTICAL_LAYOUT_H

#include <escher/palette.h>
#include <escher/solid_color_view.h>
#include <kandinsky/color.h>
#include <kandinsky/rect.h>

namespace Escher {

/* An oriented layout handles multiple views layouted alongside a main axis.
 * Each elements takes all the space available along the secondary axis.
 * A VerticalLayout uses x as it secondary axis and y as the main one.
 * An HorizontalLayout uses y as it secondary axis and x as the main one.
 * To simplify code, OrientedLayout handles coordinates as if it was a vertical
 * oriented layout. Each time an extern methods return or takes as parameter
 * coordinates (point, size or rect), it is adapted to the orientation using the
 * adapt() virtual methods. If it is an horizontal layout, these structures will
 * be transposed. */

class OrientedLayout : public View {
 public:
  OrientedLayout(KDColor color = Palette::WallScreen)
      : m_backgroundColor(color),
        m_secondaryDirectionMargin(0),
        m_mainDirectionMargin(0) {}
  KDSize minimalSizeForOptimalDisplay() const override;
  void layoutSubviews(bool force = false) override;
  void setSecondaryDirectionMargin(KDCoordinate margin) {
    m_secondaryDirectionMargin = margin;
  }
  void setMainDirectionMargin(KDCoordinate margin) {
    m_mainDirectionMargin = margin;
  }
  void drawRect(KDContext* ctx, KDRect rect) const override;

 private:
  virtual KDRect adaptRect(KDRect rect) const = 0;
  virtual KDSize adaptSize(KDSize size) const = 0;
  virtual KDPoint adaptPoint(KDPoint point) const = 0;

  KDColor m_backgroundColor;
  KDCoordinate m_secondaryDirectionMargin;
  KDCoordinate m_mainDirectionMargin;
};

/* View that lays out its subviews vertically.*/
class VerticalLayout : public OrientedLayout {
 public:
  VerticalLayout(KDColor color = Palette::WallScreen) : OrientedLayout(color) {}

 private:
  // Main direction is along y, there is no need to adapt anything.
  KDRect adaptRect(KDRect rect) const override { return rect; }
  KDSize adaptSize(KDSize size) const override { return size; }
  KDPoint adaptPoint(KDPoint point) const override { return point; }
};

/* View that lays out its subviews horizontally.*/
class HorizontalLayout : public OrientedLayout {
 public:
  HorizontalLayout(KDColor color = Palette::WallScreen)
      : OrientedLayout(color) {}

 private:
  // Main direction is along x, coordinates must be transposed.
  KDRect adaptRect(KDRect rect) const override {
    return KDRect(adaptPoint(rect.origin()), adaptSize(rect.size()));
  }
  KDSize adaptSize(KDSize size) const override {
    return KDSize(size.height(), size.width());
  }
  KDPoint adaptPoint(KDPoint point) const override {
    return KDPoint(point.y(), point.x());
  }
};

}  // namespace Escher

#endif
