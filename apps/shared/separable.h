#ifndef SHARED_SEPARABLE_H
#define SHARED_SEPARABLE_H

#include <escher/metric.h>
#include <kandinsky/coordinate.h>
#include <kandinsky/rect.h>

namespace Shared {

class Separable {
 public:
  Separable() : m_separatorLeft(false) {}
  void setSeparatorLeft(bool separator);
  KDRect separatorRect(KDRect bounds, KDCoordinate verticalOffset = 0) const;
  KDRect rectWithoutSeparator(KDRect bounds) const;

 protected:
  bool m_separatorLeft;

 private:
  constexpr static int k_thickness = Escher::Metric::TableSeparatorThickness;
  virtual void didSetSeparator() = 0;
};

}  // namespace Shared

#endif
