#ifndef APPS_GRAPH_TEST_HELPER_H
#define APPS_GRAPH_TEST_HELPER_H

#include "../app.h"

using namespace Poincare;
using namespace Shared;

namespace Graph {

constexpr ContinuousFunction::PlotType Cartesian = ContinuousFunction::PlotType::Cartesian;
constexpr ContinuousFunction::PlotType Polar = ContinuousFunction::PlotType::Polar;
constexpr ContinuousFunction::PlotType Parametric = ContinuousFunction::PlotType::Parametric;

ContinuousFunction * addFunction(const char * definition, ContinuousFunction::PlotType type, ContinuousFunctionStore * store, Context * context);

}

#endif
