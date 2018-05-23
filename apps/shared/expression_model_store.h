#ifndef SHARED_EXPRESSION_MODEL_STORE_H
#define SHARED_EXPRESSION_MODEL_STORE_H

#include "expression_model.h"
#include <stdint.h>

namespace Shared {

/* ExpressionModelStore is a dumb class.
 * Its only job is to store model */

class ExpressionModelStore {
public:
  ExpressionModelStore();
  virtual ExpressionModel * modelAtIndex(int i) = 0;
  ExpressionModel * addEmptyModel();
  void removeModel(ExpressionModel * f);
  virtual void removeAll();
  int numberOfModels() const { return m_numberOfModels; };
  virtual int maxNumberOfModels() const = 0;
  void tidy();
protected:
  virtual ExpressionModel * emptyModel() = 0;
  virtual ExpressionModel * nullModel() = 0;
  virtual void setModelAtIndex(ExpressionModel * f, int i) = 0;
  int m_numberOfModels;
};

}

#endif
