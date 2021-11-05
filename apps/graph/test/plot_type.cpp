#include <quiz.h>
#include "helper.h"
#include <apps/shared/global_context.h>

using namespace Shared;

namespace Graph {

QUIZ_CASE(graph_function_plot_type) {
  GlobalContext context;
  ContinuousFunctionStore store;
  // AddFunction asserts the function is of the expected plotType.
  addFunction("f(θ)=2", ContinuousFunction::PlotType::Polar, &store, &context);
  addFunction("g(t)=[[cos(t)][t]]", ContinuousFunction::PlotType::Parametric, &store, &context);
  addFunction("h(x)=log(x)", ContinuousFunction::PlotType::Cartesian, &store, &context);
  addFunction("y=log(x)", ContinuousFunction::PlotType::Cartesian, &store, &context);
  addFunction("y+x+1=0", ContinuousFunction::PlotType::Line, &store, &context);
  addFunction("2=y", ContinuousFunction::PlotType::HorizontalLine, &store, &context);
  addFunction("0=x", ContinuousFunction::PlotType::VerticalLine, &store, &context);
  addFunction("2-y>log(x)", ContinuousFunction::PlotType::Inequation, &store, &context);
  addFunction("2-y^2>x^2+x+y", ContinuousFunction::PlotType::Inequation, &store, &context);
  addFunction("p(x)>log(x)", ContinuousFunction::PlotType::Inequation, &store, &context);
  addFunction("x^2+y^2=12", ContinuousFunction::PlotType::Circle, &store, &context);
  addFunction("x^2+2*y^2=12", ContinuousFunction::PlotType::Ellipse, &store, &context);
  addFunction("y=x^2", ContinuousFunction::PlotType::Parabola, &store, &context);
  addFunction("x=y^2", ContinuousFunction::PlotType::Parabola, &store, &context);
  addFunction("x^2-2*y^2=12", ContinuousFunction::PlotType::Hyperbola, &store, &context);
  addFunction("x^2-y^2=0", ContinuousFunction::PlotType::Other, &store, &context);
  addFunction("x*y^2=1", ContinuousFunction::PlotType::Other, &store, &context);
  addFunction("x^2-y^2+log(x)=0", ContinuousFunction::PlotType::Other, &store, &context);
  addFunction("", ContinuousFunction::PlotType::Undefined, &store, &context);
  addFunction("3=2", ContinuousFunction::PlotType::Unhandled, &store, &context);
  addFunction("y^3=x", ContinuousFunction::PlotType::Unhandled, &store, &context);
  addFunction("x*y^2>1", ContinuousFunction::PlotType::Unhandled, &store, &context);
  addFunction("x^2=1", ContinuousFunction::PlotType::Unhandled, &store, &context);
  addFunction("2-y^2>log(x)", ContinuousFunction::PlotType::Unhandled, &store, &context);
  addFunction("x*y^2=x", ContinuousFunction::PlotType::Unhandled, &store, &context);
  store.removeAll();
}

}
