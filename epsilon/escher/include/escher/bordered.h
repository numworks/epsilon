#pragma once

#include <escher/metric.h>
#include <kandinsky/context.h>

namespace Escher {

// TODO: Bordered methods should use a ThemeComponent
class Bordered {
 public:
  void drawBorderOfRect(KDContext* ctx, KDRect rect, KDColor borderColor) const;
  void drawInnerRect(KDContext* ctx, KDRect rect,
                     KDColor backgroundColor) const;

 protected:
  constexpr static KDCoordinate k_separatorThickness =
      Metric::CellSeparatorThickness;
};

}  // namespace Escher
