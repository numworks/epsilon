#ifndef UTILS_LIST_H
#define UTILS_LIST_H

namespace List {

typedef void (*Swap) (int i, int j, void * context, int numberOfElements);
typedef bool (*Compare) (int i, int j, void * context, int numberOfElements);

void Sort(Swap swap, Compare compare, void * context, int numberOfElements);
int ExtremumIndex(Compare compare, void * context, int numberOfElements, bool minimum);

}

#endif
