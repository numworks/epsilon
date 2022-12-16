#ifndef APPS_GRAPH_TEST_HELPER_H
#define APPS_GRAPH_TEST_HELPER_H

#include "../app.h"
#include "../../poincare/test/helper.h"

using namespace Poincare;
using namespace Shared;

namespace Graph {

ContinuousFunction * addFunction(const char * definition, Shared::ContinuousFunctionStore * store, Context * context);

}

#endif
