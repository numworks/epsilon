#ifndef APPS_GRAPH_TEST_HELPER_H
#define APPS_GRAPH_TEST_HELPER_H

#include "../app.h"
#include "../../poincare/test/helper.h"

using namespace Poincare;
using namespace Shared;

namespace Graph {

constexpr NewFunction::PlotType Cartesian = NewFunction::PlotType::Cartesian;
constexpr NewFunction::PlotType Polar = NewFunction::PlotType::Polar;
constexpr NewFunction::PlotType Parametric = NewFunction::PlotType::Parametric;

NewFunction * addFunction(const char * definition, NewFunction::PlotType type, ContinuousFunctionStore * store, Context * context);

}

#endif
