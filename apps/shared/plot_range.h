#ifndef SHARED_PLOT_RANGE_H
#define SHARED_PLOT_RANGE_H

namespace Shared {

class PlotRange {
public:
  virtual float xMin() const = 0;
  virtual float xMax() const = 0;
  virtual float yMin() const = 0;
  virtual float yMax() const = 0;
  virtual float xGridUnit() const = 0;
  virtual float yGridUnit() const = 0;
};

}

#endif
