#ifndef POINCARE_DATASET_COLUMN_H
#define POINCARE_DATASET_COLUMN_H

namespace Poincare {

template<typename T>
class DatasetColumn {
public:
  virtual T valueAtIndex(int index) const = 0;
};

}

#endif
