#ifndef POINCARE_COMMUTATIVE_OPERATION_H
#define POINCARE_COMMUTATIVE_OPERATION_H

#include <poincare/n_ary_operation.h>

namespace Poincare {

class CommutativeOperation : public NAryOperation {
  using NAryOperation::NAryOperation;
public:
  void sort() override;
};

}

#endif
