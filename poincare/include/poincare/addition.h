#ifndef POINCARE_ADDITION_H
#define POINCARE_ADDITION_H

#include <poincare/commutative_operation.h>

class Addition : public CommutativeOperation {
  using CommutativeOperation::CommutativeOperation;
  public:
    Type type() override;
    float operateApproximatevelyOn(float a, float b) override;
    Expression * clone() override;
#ifdef DEBUG
    int getPrintableVersion(char* txt) override;
#endif
  protected:
    char operatorChar() override;
};

#endif
