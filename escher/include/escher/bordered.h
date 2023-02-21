#ifndef ESCHER_BORDERED_H
#define ESCHER_BORDERED_H

#include <escher/metric.h>
#include <kandinsky/context.h>

namespace Escher {

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

#endif
