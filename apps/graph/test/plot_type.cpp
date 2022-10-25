#include <quiz.h>
#include "helper.h"
#include <apps/shared/global_context.h>

using namespace Shared;

namespace Graph {

void assert_check_function_properties(const char * expression, ContinuousFunction::PlotType plotType, ComparisonNode::OperatorType expectedEquationType = ComparisonNode::OperatorType::Equal, ContinuousFunctionProperties::AreaType expectedAreaType = ContinuousFunctionProperties::AreaType::None) {
  GlobalContext context;
  ContinuousFunctionStore store;
  // AddFunction asserts the function is of the expected plotType.
  Shared::ContinuousFunction * function = addFunction(expression, plotType, &store, &context);
  // Memoize the reduced expression so that numberOfSubCurves() can be asserted
  function->expressionReduced(&context);
  if (!ContinuousFunction::IsPlotTypeInactive(plotType)) {
    ComparisonNode::OperatorType observedEquationType = function->equationType();
    ContinuousFunctionProperties::AreaType observedAreaType = function->areaType();
    // EquationType is accurate on active plot types only.
    quiz_assert(observedEquationType == expectedEquationType);
    quiz_assert(observedAreaType == expectedAreaType);
  }
  store.removeAll();
}

QUIZ_CASE(graph_function_plot_type) {
  // Test the plot type under different Press-to-test parameters :
  Preferences::PressToTestParams pressToTestParams = Preferences::k_inactivePressToTest;
  for (size_t config = 0; config < 4; config++) {
    bool noInequations = (config == 1 || config == 3);
    bool noImplicitPlot = (config == 2 || config == 3);
    // Set the Press-to-test mode
    pressToTestParams.m_inequalityGraphingIsForbidden = noInequations;
    pressToTestParams.m_implicitPlotsAreForbidden = noImplicitPlot;
    Poincare::Preferences::sharedPreferences()->setExamMode((noInequations || noImplicitPlot) ? Poincare::Preferences::ExamMode::PressToTest : Poincare::Preferences::ExamMode::Off, pressToTestParams);

    assert_check_function_properties("r=2", ContinuousFunction::PlotType::Polar);
    assert_check_function_properties("g(t)=[[cos(t)][t]]", ContinuousFunction::PlotType::Parametric);
    assert_check_function_properties("h(x)=log(x)", ContinuousFunction::PlotType::Cartesian);
    assert_check_function_properties("y=log(x)", ContinuousFunction::PlotType::Cartesian);
    assert_check_function_properties("y+x+1=0", noImplicitPlot ? ContinuousFunction::PlotType::Disabled : ContinuousFunction::PlotType::Line);
    assert_check_function_properties("y=2*y+1", noImplicitPlot ? ContinuousFunction::PlotType::Disabled : ContinuousFunction::PlotType::HorizontalLine);
    assert_check_function_properties("2=y", noImplicitPlot ? ContinuousFunction::PlotType::Disabled : ContinuousFunction::PlotType::HorizontalLine);
    assert_check_function_properties("y=0", ContinuousFunction::PlotType::HorizontalLine);
    assert_check_function_properties("x=1", ContinuousFunction::PlotType::VerticalLine);
    assert_check_function_properties("0=x", noImplicitPlot ? ContinuousFunction::PlotType::Disabled : ContinuousFunction::PlotType::VerticalLine);
    assert_check_function_properties("x=1-x", noImplicitPlot ? ContinuousFunction::PlotType::Disabled : ContinuousFunction::PlotType::VerticalLine);
    assert_check_function_properties("0=x+cos(y)+y^2", noImplicitPlot ? ContinuousFunction::PlotType::Disabled : ContinuousFunction::PlotType::CartesianAlongY);
    assert_check_function_properties("y^3=x", noImplicitPlot ? ContinuousFunction::PlotType::Disabled : ContinuousFunction::PlotType::CartesianAlongY);
    assert_check_function_properties("0=x^2", noImplicitPlot ? ContinuousFunction::PlotType::Disabled : ContinuousFunction::PlotType::OtherAlongY);
    assert_check_function_properties("1=x^2+x", noImplicitPlot ? ContinuousFunction::PlotType::Disabled : ContinuousFunction::PlotType::OtherAlongY);
    assert_check_function_properties("1+x^2=0", noImplicitPlot ? ContinuousFunction::PlotType::Disabled : ContinuousFunction::PlotType::OtherAlongY);
    assert_check_function_properties("x+x^2=cos(y)", noImplicitPlot ? ContinuousFunction::PlotType::Disabled : ContinuousFunction::PlotType::OtherAlongY);
    assert_check_function_properties("x^2<0", (noInequations || noImplicitPlot) ? ContinuousFunction::PlotType::Disabled : ContinuousFunction::PlotType::OtherAlongY, ComparisonNode::OperatorType::Inferior, ContinuousFunctionProperties::AreaType::Inside);
    assert_check_function_properties("y>log(x)", noInequations ? ContinuousFunction::PlotType::Disabled : ContinuousFunction::PlotType::Cartesian, ComparisonNode::OperatorType::Superior, ContinuousFunctionProperties::AreaType::Above);
    assert_check_function_properties("2-y>log(x)", (noInequations || noImplicitPlot) ? ContinuousFunction::PlotType::Disabled : ContinuousFunction::PlotType::Cartesian, ComparisonNode::OperatorType::Inferior, ContinuousFunctionProperties::AreaType::Below);
    assert_check_function_properties("2-y^2>x^2+x+y", (noInequations || noImplicitPlot) ? ContinuousFunction::PlotType::Disabled : ContinuousFunction::PlotType::Circle, ComparisonNode::OperatorType::Inferior, ContinuousFunctionProperties::AreaType::Inside);
    assert_check_function_properties("p(x)>log(x)", noInequations ? ContinuousFunction::PlotType::Disabled : ContinuousFunction::PlotType::Cartesian, ComparisonNode::OperatorType::Superior, ContinuousFunctionProperties::AreaType::Above);
    assert_check_function_properties("x^2+y^2=12", noImplicitPlot ? ContinuousFunction::PlotType::Disabled : ContinuousFunction::PlotType::Circle);
    assert_check_function_properties("x^2+2*y^2=12", noImplicitPlot ? ContinuousFunction::PlotType::Disabled : ContinuousFunction::PlotType::Ellipse);
    assert_check_function_properties("x=y^2", noImplicitPlot ? ContinuousFunction::PlotType::Disabled : ContinuousFunction::PlotType::Parabola);
    /* When implicit plots are disabled, these conics are no longer identified
     * to hide details */
    assert_check_function_properties("y=x^2", noImplicitPlot ? ContinuousFunction::PlotType::Cartesian : ContinuousFunction::PlotType::CartesianParabola);

    assert_check_function_properties("x^2-2*y^2=12", noImplicitPlot ? ContinuousFunction::PlotType::Disabled : ContinuousFunction::PlotType::Hyperbola);
    assert_check_function_properties("y*x=1", noImplicitPlot ? ContinuousFunction::PlotType::Disabled : ContinuousFunction::PlotType::CartesianHyperbola);
    assert_check_function_properties("x^2-y^2=0", noImplicitPlot ? ContinuousFunction::PlotType::Disabled : ContinuousFunction::PlotType::Other);
    assert_check_function_properties("x*y^2=1", noImplicitPlot ? ContinuousFunction::PlotType::Disabled : ContinuousFunction::PlotType::Other);
    assert_check_function_properties("x^2-y^2+log(x)=0", noImplicitPlot ? ContinuousFunction::PlotType::Disabled : ContinuousFunction::PlotType::Other);
    assert_check_function_properties("y^2>-1", (noInequations || noImplicitPlot) ? ContinuousFunction::PlotType::Disabled : ContinuousFunction::PlotType::Other, ComparisonNode::OperatorType::Superior, ContinuousFunctionProperties::AreaType::Outside);
    assert_check_function_properties("(y-x+x^2)^2>=0", (noInequations || noImplicitPlot) ? ContinuousFunction::PlotType::Disabled : ContinuousFunction::PlotType::Other, ComparisonNode::OperatorType::SuperiorEqual, ContinuousFunctionProperties::AreaType::Outside);
    assert_check_function_properties("", ContinuousFunction::PlotType::Undefined);
    assert_check_function_properties("y=log(0)", ContinuousFunction::PlotType::Undefined);
    assert_check_function_properties("f(t)=[[cos(t)][t]]*log(0)", ContinuousFunction::PlotType::UndefinedParametric);
    assert_check_function_properties("r=θ+[[0][0]]", ContinuousFunction::PlotType::UndefinedPolar);
    assert_check_function_properties("3=2", ContinuousFunction::PlotType::Unhandled);
    assert_check_function_properties("x*y^2>1", ContinuousFunction::PlotType::Unhandled);
    assert_check_function_properties("2-y^2>log(x)", ContinuousFunction::PlotType::Unhandled);
    assert_check_function_properties("x*y^2=x", ContinuousFunction::PlotType::Unhandled);
    assert_check_function_properties("y=i*x+1", ContinuousFunction::PlotType::Unhandled);

    // Updated complex format
    assert(Poincare::Preferences::sharedPreferences()->complexFormat() == Preferences::ComplexFormat::Cartesian);
    assert_check_function_properties("y=(√(-1))^2", ContinuousFunction::PlotType::HorizontalLine);
    assert_check_function_properties("y=(i)^2", ContinuousFunction::PlotType::HorizontalLine);
    assert_check_function_properties("f(x)=im(i*x+1)", ContinuousFunction::PlotType::Cartesian);
    Poincare::Preferences::sharedPreferences()->setComplexFormat(Preferences::ComplexFormat::Real);
    assert_check_function_properties("y=(√(-1))^2", ContinuousFunction::PlotType::Unhandled);
    assert_check_function_properties("y=(i)^2", ContinuousFunction::PlotType::HorizontalLine);
    assert_check_function_properties("f(x)=im(i*x+1)", ContinuousFunction::PlotType::Cartesian);
    // TODO : Handle this function
    assert_check_function_properties("y=im(i*x+1)", ContinuousFunction::PlotType::Unhandled);
    // Restore preferences
    Poincare::Preferences::sharedPreferences()->setComplexFormat(Preferences::ComplexFormat::Cartesian);
  }
  // Restore an Off exam mode.
  Poincare::Preferences::sharedPreferences()->setExamMode(Poincare::Preferences::ExamMode::Off);
}

QUIZ_CASE(graph_function_plot_type_with_predefined_variables) {

    GlobalContext context;
    ContinuousFunctionStore store;
    // Add a predefined test function
    addFunction("test(x)=1+x", ContinuousFunction::PlotType::Cartesian, &store, &context);

    addFunction("y=x", ContinuousFunction::PlotType::Line, &store, &context);
    addFunction("y=test(x)", ContinuousFunction::PlotType::Line, &store, &context);
    addFunction("y=a*x+1", ContinuousFunction::PlotType::Line, &store, &context);
    addFunction("a*y*y+y=x", ContinuousFunction::PlotType::Other, &store, &context);

    // Add a predefined a symbol
    assert_reduce_and_store("0→a", Preferences::AngleUnit::Radian, Poincare::Preferences::UnitFormat::Metric, Poincare::Preferences::ComplexFormat::Real);
    addFunction("y=a*x+1", ContinuousFunction::PlotType::HorizontalLine, &store, &context);
    addFunction("a*y*y+y=x", ContinuousFunction::PlotType::Line, &store, &context);

    assert_reduce_and_store("1→a", Preferences::AngleUnit::Radian, Poincare::Preferences::UnitFormat::Metric, Poincare::Preferences::ComplexFormat::Real);
    addFunction("y=a*x+1", ContinuousFunction::PlotType::Line, &store, &context);
    addFunction("a*y*y+y=x", ContinuousFunction::PlotType::Parabola, &store, &context);

    // Add a predefined y symbol
    assert_reduce_and_store("1→y", Preferences::AngleUnit::Radian, Poincare::Preferences::UnitFormat::Metric, Poincare::Preferences::ComplexFormat::Real);
    addFunction("y=x", ContinuousFunction::PlotType::Line, &store, &context);

    Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("a.exp").destroy();
    Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("y.exp").destroy();
    store.removeAll();
}

}
