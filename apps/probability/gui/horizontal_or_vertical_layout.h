#ifndef PROBABILITY_GUI_HORIZONTAL_OR_VERTICAL_LAYOUT_H
#define PROBABILITY_GUI_HORIZONTAL_OR_VERTICAL_LAYOUT_H

#include <escher/palette.h>
#include <escher/solid_color_view.h>
#include <kandinsky/color.h>
#include <kandinsky/rect.h>

namespace Probability {

class OrientedLayout : public Escher::View {
public:
  OrientedLayout(KDColor color = Escher::Palette::WallScreen) :
        m_backgroundColor(color), m_secondaryDirectionMargin(0), m_mainDirectionMargin(0) {}
  KDSize minimalSizeForOptimalDisplay() const override;
  void layoutSubviews(bool force = false) override;
  void setMargins(KDCoordinate secondaryDirectionMargin, KDCoordinate mainDirectionMargin) {
    m_secondaryDirectionMargin = secondaryDirectionMargin;
    m_mainDirectionMargin = mainDirectionMargin;
  }

  virtual KDCoordinate secondaryDirectionCoordinate(KDPoint p) const = 0;
  virtual KDCoordinate mainDirectionCoordinate(KDPoint p) const = 0;
  virtual KDCoordinate secondaryDirectionLength(KDSize p) const = 0;
  virtual KDCoordinate mainDirectionLength(KDSize p) const = 0;

  void drawRect(KDContext * ctx, KDRect rect) const override;

protected:
  KDColor m_backgroundColor;
  KDCoordinate m_secondaryDirectionMargin;
  KDCoordinate m_mainDirectionMargin;

private:
  virtual KDSize sizeFromMainAndSecondaryDirection(KDCoordinate mainDirection,
                                                   KDCoordinate secondaryDirection) const = 0;
  KDCoordinate secondaryDirectionMargin() const;
  KDCoordinate mainDirectionMargin() const;
  KDPoint pointFromMainAndSecondaryDirection(KDCoordinate mainDirection,
                                             KDCoordinate secondaryDirection) const;
  KDRect rectFromMainAndSecondaryDirection(KDRect rect) const;
};

/* View that lays out its subviews vertically.*/
class VerticalLayout : public OrientedLayout {
public:
  VerticalLayout(KDColor color = Escher::Palette::WallScreen) : OrientedLayout(color) {}

  KDCoordinate secondaryDirectionCoordinate(KDPoint p) const override { return p.x(); }
  KDCoordinate mainDirectionCoordinate(KDPoint p) const override { return p.y(); }
  KDCoordinate secondaryDirectionLength(KDSize p) const override { return p.width(); }
  KDCoordinate mainDirectionLength(KDSize p) const override { return p.height(); }
  KDSize sizeFromMainAndSecondaryDirection(KDCoordinate mainDirection,
                                           KDCoordinate secondaryDirection) const override {
    return KDSize(secondaryDirection, mainDirection);
  }
};

/* View that lays out its subviews horizontally.*/
class HorizontalLayout : public OrientedLayout {
public:
  HorizontalLayout(KDColor color = Escher::Palette::WallScreen) : OrientedLayout(color) {}

  KDCoordinate secondaryDirectionCoordinate(KDPoint p) const override { return p.y(); }
  KDCoordinate mainDirectionCoordinate(KDPoint p) const override { return p.x(); }
  KDCoordinate secondaryDirectionLength(KDSize p) const override { return p.height(); }
  KDCoordinate mainDirectionLength(KDSize p) const override { return p.width(); }
  KDSize sizeFromMainAndSecondaryDirection(KDCoordinate mainDirection,
                                           KDCoordinate secondaryDirection) const override {
    return KDSize(mainDirection, secondaryDirection);
  }
};

}  // namespace Probability

#endif /* PROBABILITY_GUI_HORIZONTAL_OR_VERTICAL_LAYOUT_H */
