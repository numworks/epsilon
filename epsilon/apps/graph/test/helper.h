#pragma once

#include <poincare/test/old/helper.h>

#include "../app.h"

namespace Graph {

Shared::ContinuousFunction* addFunction(const char* definition,
                                        Shared::ContinuousFunctionStore* store,
                                        const Poincare::Context& context);

}
