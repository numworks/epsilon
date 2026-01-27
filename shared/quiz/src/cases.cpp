#include "cases.h"

#include <omg/list.h>
#include <string.h>

#include <algorithm>

namespace Quiz {

int Cases::s_number = 0;
const char* Cases::s_names[Cases::k_maxNumber];
TestCase Cases::s_bodies[Cases::k_maxNumber];

void Cases::Sort() {
  OMG::List::Sort(
      [](int i, int j, void* context, int nbElements) {
        std::swap(s_names[i], s_names[j]);
        std::swap(s_bodies[i], s_bodies[j]);
      },
      [](int i, int j, void* context, int nbElements) {
        return strcmp(s_names[i], s_names[j]) > 0;
      },
      nullptr, s_number);
}

RegisterCase::RegisterCase(const char* name, TestCase function) {
  assert(Cases::s_number < Cases::k_maxNumber);
  Cases::s_names[Cases::s_number] = name;
  Cases::s_bodies[Cases::s_number++] = function;
}

}  // namespace Quiz
