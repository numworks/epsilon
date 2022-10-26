#include <utils/list.h>

void List::Sort(Swap swap, Compare compare, void * context, int numberOfElements) {
  /* Using an insertion-sort algorithm, which has the advantage of being
   * in-place and efficient when already sorted. It is optimal if Compare is
   * more lenient with equalities ( >= instead of > ) */
  for (int i = 1; i < numberOfElements; i++) {
    for (int j = i - 1; j >= 0; j--) {
      if (compare(j+1, j, context, numberOfElements)) {
        break;
      }
      swap(j, j+1, context, numberOfElements);
    }
  }
}

int List::ExtremumIndex(Compare compare, void * context, int numberOfElements, bool minimum) {
  int returnIndex = 0;
  for (int i = 0; i < numberOfElements; i++) {
    bool newIsGreater = compare(i, returnIndex, context, numberOfElements);
    if ((minimum && !newIsGreater) || (!minimum && newIsGreater)) {
      returnIndex = i;
    }
  }
  return returnIndex;
}
