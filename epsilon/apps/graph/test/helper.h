#ifndef APPS_GRAPH_TEST_HELPER_H
#define APPS_GRAPH_TEST_HELPER_H

#include <poincare/test/old/helper.h>

#include "../app.h"

namespace Graph {

Shared::ContinuousFunction* addFunction(const char* definition,
                                        Shared::ContinuousFunctionStore* store,
                                        Poincare::Context* context);

}

#endif
