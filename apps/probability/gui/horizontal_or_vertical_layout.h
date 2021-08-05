#ifndef APPS_PROBABILITY_GUI_HORIZONTAL_OR_VERTICAL_LAYOUT_H
#define APPS_PROBABILITY_GUI_HORIZONTAL_OR_VERTICAL_LAYOUT_H

#include <apps/i18n.h>
#include <escher/palette.h>
#include <escher/responder.h>
#include <escher/solid_color_view.h>
#include <kandinsky/color.h>
#include <kandinsky/context.h>
#include <kandinsky/rect.h>

#include "selectable_cell_list_controller.h"

namespace Probability {

class OrientedLayout : public Escher::SolidColorView {
public:
  OrientedLayout(KDColor color = Palette::WallScreen) :
      Escher::SolidColorView(color), m_marginX(0), m_marginY(0) {}
  KDSize minimalSizeForOptimalDisplay() const override;
  void layoutSubviews(bool force = false) override;
  void setMargins(KDCoordinate marginX, KDCoordinate marginY) {
    m_marginX = marginX;
    m_marginY = marginY;
  }

  virtual KDCoordinate firstCoordinate(KDPoint p) const = 0;
  virtual KDCoordinate secondCoordinate(KDPoint p) const = 0;
  virtual KDCoordinate firstLength(KDSize p) const = 0;
  virtual KDCoordinate secondLength(KDSize p) const = 0;
  virtual KDSize reorderedSize(KDCoordinate first, KDCoordinate second) const = 0;

protected:
  KDCoordinate m_marginX;
  KDCoordinate m_marginY;

private:
  KDCoordinate firstMargin() const;
  KDCoordinate secondMargin() const;
  KDPoint reorderedPoint(KDCoordinate first, KDCoordinate second) const;
  KDRect reorderedRect(KDRect rect) const;
};

/* View that lays out its subviews vertically.*/
class VerticalLayout : public OrientedLayout {
public:
  VerticalLayout(KDColor color = Palette::WallScreen) : OrientedLayout(color) {}

  KDCoordinate firstCoordinate(KDPoint p) const override { return p.x(); }
  KDCoordinate secondCoordinate(KDPoint p) const override { return p.y(); }
  KDCoordinate firstLength(KDSize p) const override { return p.width(); }
  KDCoordinate secondLength(KDSize p) const override { return p.height(); }
  KDSize reorderedSize(KDCoordinate first, KDCoordinate second) const override {
    return KDSize(first, second);
  }
};

/* View that lays out its subviews horizontally.*/
class HorizontalLayout : public OrientedLayout {
public:
  HorizontalLayout(KDColor color = Palette::WallScreen) : OrientedLayout(color) {}

  KDCoordinate firstCoordinate(KDPoint p) const override { return p.y(); }
  KDCoordinate secondCoordinate(KDPoint p) const override { return p.x(); }
  KDCoordinate firstLength(KDSize p) const override { return p.height(); }
  KDCoordinate secondLength(KDSize p) const override { return p.width(); }
  KDSize reorderedSize(KDCoordinate first, KDCoordinate second) const override {
    return KDSize(second, first);
  }
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_GUI_HORIZONTAL_OR_VERTICAL_LAYOUT_H */
