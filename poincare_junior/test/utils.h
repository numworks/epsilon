#ifndef POINCARE_TEST_UTILS_H
#define POINCARE_TEST_UTILS_H

#include <poincare_junior/cache_reference.h>

// Dummy parse
inline CachedReference Parse(const char * textInput) {
  // textInput == (1-2)/3/4
  return CacheReference([]{
      DivisionInterface::PushNode();
      DivisionInterface::PushNode();
      SubtractionInterface::PushNode();
      IntegerInterface::PushNode(1);
      IntegerInterface::PushNode(2);
      IntegerInterface::PushNode(3);
      IntegerInterface::PushNode(4);
      return true;
    });
}

#endif
