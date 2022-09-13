#ifndef POINCARE_TEST_UTILS_H
#define POINCARE_TEST_UTILS_H

#include <poincare_junior/cached_tree.h>

// Dummy parse
inline CachedTree Parse(const char * textInput) {
  // textInput == (1-2)/3/4
  std::cout << "\n---------------- Input " << textInput << "----------------" << std::endl;
  return CachedTree([]{
      Division::PushNode();
      Division::PushNode();
      Subtraction::PushNode();
      Integer::PushNode(1);
      Integer::PushNode(2);
      Integer::PushNode(3);
      Integer::PushNode(4);
      return true;
    });
}

#endif
