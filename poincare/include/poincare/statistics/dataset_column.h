#pragma once

namespace Poincare {

template <typename T>
class DatasetColumn {
 public:
  virtual T valueAtIndex(int index) const = 0;
  virtual int length() const = 0;
};

}  // namespace Poincare
