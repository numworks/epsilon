#ifndef APPS_GRAPH_TEST_HELPER_H
#define APPS_GRAPH_TEST_HELPER_H

#include "../app.h"

using namespace Poincare;
using namespace Shared;

namespace Graph {

constexpr ContinuousFunction::PlotType Cartesian = ContinuousFunction::PlotType::Cartesian;
constexpr ContinuousFunction::PlotType Polar = ContinuousFunction::PlotType::Polar;
constexpr ContinuousFunction::PlotType Parametric = ContinuousFunction::PlotType::Parametric;

constexpr Preferences::AngleUnit Radian = Preferences::AngleUnit::Radian;
constexpr Preferences::AngleUnit Degree = Preferences::AngleUnit::Degree;
constexpr Preferences::AngleUnit Gradian = Preferences::AngleUnit::Gradian;

ContinuousFunction * addFunction(const char * definition, ContinuousFunction::PlotType type, ContinuousFunctionStore * store, Context * context);

}

#endif
