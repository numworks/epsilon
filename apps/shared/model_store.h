#ifndef SHARED_MODEL_STORE_H
#define SHARED_MODEL_STORE_H

#include <stdint.h>

namespace Shared {

/* ModelStore is a dumb class.
 * Its only job is to store model */

template<typename T>
class ModelStore {
public:
  ModelStore();
  virtual T * modelAtIndex(int i) = 0;
  T * addEmptyModel();
  void removeModel(T * f);
  virtual void removeAll();
  int numberOfModels() const { return m_numberOfModels; };
  virtual int maxNumberOfModels() const = 0;
  void tidy();
protected:
  virtual T * emptyModel() = 0;
  virtual T * nullModel() = 0;
  virtual void setModelAtIndex(T * f, int i) = 0;
  virtual void tidyModelAtIndex(int i) = 0;
  int m_numberOfModels;
};

}

#endif
