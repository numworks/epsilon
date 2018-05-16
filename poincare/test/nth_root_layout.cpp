#include <quiz.h>
#include <ion.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_nth_root_layout_serialize) {
  assert_parsed_expression_layout_serialize_to_self("root(7,3)");
}
