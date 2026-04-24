#pragma once

#include <escher/metric.h>
#include <escher/theme/components.h>
#include <kandinsky/context.h>

namespace Escher {

class Bordered {
 public:
  void drawBorderOfRect(KDContext* ctx, KDRect rect,
                        ThemeComponent borderColor) const;
  void drawInnerRect(KDContext* ctx, KDRect rect,
                     ThemeComponent backgroundColor) const;

 protected:
  constexpr static KDCoordinate k_separatorThickness =
      Metric::CellSeparatorThickness;
};

}  // namespace Escher
