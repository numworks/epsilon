#ifndef POINCARE_TEST_UTILS_H
#define POINCARE_TEST_UTILS_H

#include <poincare_junior/expression.h>

// Dummy parse
inline Poincare::Expression Parse(const char * textInput) {
  // textInput == (1-2)/3/4
  return Poincare::Expression([]{
      DivisionInterface::PushNode();
      DivisionInterface::PushNode();
      SubtractionInterface::PushNode();
      IntegerInterface::PushNode(1);
      IntegerInterface::PushNode(2);
      IntegerInterface::PushNode(3);
      IntegerInterface::PushNode(4);
    });
}

#endif
