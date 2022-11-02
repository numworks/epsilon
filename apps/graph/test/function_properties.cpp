#include <quiz.h>
#include "helper.h"
#include <apps/shared/global_context.h>

using namespace Shared;

namespace Graph {

struct FunctionProperties {
  ContinuousFunctionProperties::Status m_status = ContinuousFunctionProperties::k_defaultStatus;
  I18n::Message m_caption = ContinuousFunctionProperties::k_defaultCaption;
  ContinuousFunctionProperties::SymbolType m_symbolType = ContinuousFunctionProperties::k_defaultSymbolType;
  Poincare::ComparisonNode::OperatorType m_equationType = ContinuousFunctionProperties::k_defaultEquationType;
  ContinuousFunctionProperties::CurveParameterType m_curveParameterType = ContinuousFunctionProperties::k_defaultCurveParameterType;
  Poincare::Conic::Shape m_conicShape = ContinuousFunctionProperties::k_defaultConicShape;
  bool m_isOfDegreeTwo = ContinuousFunctionProperties::k_defaultIsOfDegreeTwo;
  int m_numberOfSubCurves = 1;
  bool m_isAlongY = ContinuousFunctionProperties::k_defaultIsAlongY;
};

void assert_check_function_properties(const char * expression, FunctionProperties expectedProperties, ContinuousFunctionStore * store, Context * context, ContinuousFunctionProperties::AreaType expectedAreaType = ContinuousFunctionProperties::AreaType::None) {
  Shared::ContinuousFunction * function = addFunction(expression, store, context);

  ContinuousFunctionProperties fProperties = function->properties();
  quiz_assert(fProperties.caption() == expectedProperties.m_caption);
  quiz_assert(fProperties.status() == expectedProperties.m_status);
  quiz_assert(fProperties.symbolType() == expectedProperties.m_symbolType);
  if (expectedProperties.m_status == ContinuousFunctionProperties::Status::Enabled) {
    quiz_assert(fProperties.getCurveParameterType() == expectedProperties.m_curveParameterType);
    quiz_assert(fProperties.conicShape() == expectedProperties.m_conicShape);
    quiz_assert(fProperties.isOfDegreeTwo() == expectedProperties.m_isOfDegreeTwo);
    quiz_assert(fProperties.isAlongY() == expectedProperties.m_isAlongY);
    quiz_assert(function->numberOfSubCurves() == expectedProperties.m_numberOfSubCurves);
    quiz_assert(fProperties.equationType() == expectedProperties.m_equationType);
    quiz_assert(fProperties.areaType() == expectedAreaType);
  }
}

void assert_check_function_properties(const char * expression, FunctionProperties expectedProperties, ContinuousFunctionProperties::AreaType expectedAreaType = ContinuousFunctionProperties::AreaType::None) {
  GlobalContext context;
  ContinuousFunctionStore store;
  assert_check_function_properties(expression, expectedProperties, &store, &context, expectedAreaType);
  store.removeAll();
}

QUIZ_CASE(graph_function_plot_type) {
  // Test the plot type under different Press-to-test parameters :
  Preferences::PressToTestParams pressToTestParams = Preferences::k_inactivePressToTest;
  Poincare::Preferences::sharedPreferences()->setComplexFormat(Preferences::ComplexFormat::Cartesian);
  for (size_t config = 0; config < 4; config++) {
    bool noInequations = (config == 1 || config == 3);
    bool noImplicitPlot = (config == 2 || config == 3);
    // Set the Press-to-test mode
    pressToTestParams.m_inequalityGraphingIsForbidden = noInequations;
    pressToTestParams.m_implicitPlotsAreForbidden = noImplicitPlot;
    Poincare::Preferences::sharedPreferences()->setExamMode((noInequations || noImplicitPlot) ? Poincare::Preferences::ExamMode::PressToTest : Poincare::Preferences::ExamMode::Off, pressToTestParams);

    assert_check_function_properties(
      "r=2",
      FunctionProperties{
        .m_caption = I18n::Message::PolarCircleType,
        .m_symbolType = ContinuousFunctionProperties::SymbolType::Theta,
        .m_curveParameterType = ContinuousFunctionProperties::CurveParameterType::Polar,
        .m_conicShape = Poincare::Conic::Shape::Circle}
    );

    assert_check_function_properties(
      "g(t)=[[cos(t)][t]]",
      FunctionProperties{
        .m_caption = I18n::Message::ParametricEquationType,
        .m_symbolType = ContinuousFunctionProperties::SymbolType::T,
        .m_curveParameterType = ContinuousFunctionProperties::CurveParameterType::Parametric}
    );

    assert_check_function_properties(
      "h(x)=log(x)",
      FunctionProperties{
        .m_caption = I18n::Message::LogarithmicType,
        .m_curveParameterType = ContinuousFunctionProperties::CurveParameterType::CartesianFunction}
    );

    assert_check_function_properties(
      "y=log(x)",
      FunctionProperties{
        .m_caption = I18n::Message::EquationType,
        .m_curveParameterType = ContinuousFunctionProperties::CurveParameterType::CartesianFunction}
    );

    constexpr static FunctionProperties k_bannedProperties = FunctionProperties{
      .m_status = ContinuousFunctionProperties::Status::Banned,
      .m_caption = I18n::Message::Disabled
    };

    assert_check_function_properties(
      "y+x+1=0",
      noImplicitPlot ?
        k_bannedProperties :
        FunctionProperties{
          .m_caption = I18n::Message::LineType,
          .m_curveParameterType = ContinuousFunctionProperties::CurveParameterType::Line}
    );

    constexpr static FunctionProperties k_horizontalLineProperties = FunctionProperties{
      .m_caption = I18n::Message::HorizontalLineType,
      .m_curveParameterType = ContinuousFunctionProperties::CurveParameterType::HorizontalLine
    };
    assert_check_function_properties("y=2*y+1", noImplicitPlot ? k_bannedProperties : k_horizontalLineProperties);
    assert_check_function_properties("2=y", noImplicitPlot ? k_bannedProperties : k_horizontalLineProperties);
    assert_check_function_properties("y=0", k_horizontalLineProperties);

    constexpr static FunctionProperties k_verticalLineProperties = FunctionProperties{
      .m_caption = I18n::Message::VerticalLineType,
      .m_curveParameterType = ContinuousFunctionProperties::CurveParameterType::VerticalLine,
      .m_isAlongY = true
    };
    assert_check_function_properties("x=1", k_verticalLineProperties);
    assert_check_function_properties("0=x", noImplicitPlot ? k_bannedProperties : k_verticalLineProperties);
    assert_check_function_properties("x=1-x", noImplicitPlot ? k_bannedProperties : k_verticalLineProperties);

    constexpr static FunctionProperties k_alongY = FunctionProperties{
      .m_caption = I18n::Message::EquationType,
      .m_symbolType = ContinuousFunctionProperties::SymbolType::X,
      .m_isAlongY = true
    };
    assert_check_function_properties("0=x+cos(y)+y^2", noImplicitPlot ? k_bannedProperties : k_alongY);
    assert_check_function_properties("y^3=x", noImplicitPlot ? k_bannedProperties : k_alongY);

    constexpr static FunctionProperties k_alongYOfDegreeTwoWithTwoSubCurves = FunctionProperties{
      .m_caption = I18n::Message::EquationType,
      .m_isOfDegreeTwo = true,
      .m_numberOfSubCurves = 2,
      .m_isAlongY = true
    };
    constexpr static FunctionProperties k_alongYOfDegreeTwoWithOneSubCurve = FunctionProperties{
      .m_caption = I18n::Message::EquationType,
      .m_isOfDegreeTwo = true,
      .m_numberOfSubCurves = 1,
      .m_isAlongY = true
    };
    assert_check_function_properties("0=x^2", noImplicitPlot ? k_bannedProperties : k_alongYOfDegreeTwoWithOneSubCurve);
    assert_check_function_properties("1=x^2+x", noImplicitPlot ? k_bannedProperties : k_alongYOfDegreeTwoWithTwoSubCurves);
    assert_check_function_properties("1+x^2=0", noImplicitPlot ? k_bannedProperties : k_alongYOfDegreeTwoWithTwoSubCurves);
    assert_check_function_properties("x+x^2=cos(y)", noImplicitPlot ? k_bannedProperties : k_alongYOfDegreeTwoWithTwoSubCurves);

    assert_check_function_properties(
      "x^2<0",
      (noInequations || noImplicitPlot) ?
        k_bannedProperties :
        FunctionProperties{
          .m_caption = I18n::Message::InequalityType,
          .m_equationType = Poincare::ComparisonNode::OperatorType::Inferior,
          .m_isOfDegreeTwo = true,
          .m_numberOfSubCurves = 1,
          .m_isAlongY = true
        },
      ContinuousFunctionProperties::AreaType::Inside
    );

    assert_check_function_properties(
      "y>log(x)",
      noInequations ?
        k_bannedProperties :
        FunctionProperties{
          .m_caption = I18n::Message::InequalityType,
          .m_equationType = Poincare::ComparisonNode::OperatorType::Superior,
          .m_curveParameterType = ContinuousFunctionProperties::CurveParameterType::CartesianFunction
        },
      ContinuousFunctionProperties::AreaType::Above
    );

    assert_check_function_properties(
      "2-y>log(x)",
      (noInequations || noImplicitPlot) ?
        k_bannedProperties :
        FunctionProperties{
          .m_caption = I18n::Message::InequalityType,
          .m_equationType = Poincare::ComparisonNode::OperatorType::Inferior,
          .m_curveParameterType = ContinuousFunctionProperties::CurveParameterType::CartesianFunction
        },
      ContinuousFunctionProperties::AreaType::Below
    );

    // Conics
    assert_check_function_properties(
      "2-y^2>x^2+x+y",
      (noInequations || noImplicitPlot) ?
        k_bannedProperties :
        FunctionProperties{
          .m_caption = I18n::Message::InequalityType,
          .m_equationType = Poincare::ComparisonNode::OperatorType::Inferior,
          .m_conicShape = Poincare::Conic::Shape::Circle,
          .m_isOfDegreeTwo = true,
          .m_numberOfSubCurves = 2
        },
      ContinuousFunctionProperties::AreaType::Inside
    );
    assert_check_function_properties(
      "p(x)>log(x)",
      noInequations ?
        k_bannedProperties :
        FunctionProperties{
          .m_caption = I18n::Message::InequalityType,
          .m_equationType = Poincare::ComparisonNode::OperatorType::Superior,
          .m_curveParameterType = ContinuousFunctionProperties::CurveParameterType::CartesianFunction
        },
      ContinuousFunctionProperties::AreaType::Above
    );
    assert_check_function_properties(
      "x^2+y^2=12",
      noImplicitPlot ?
        k_bannedProperties :
        FunctionProperties{
          .m_caption = I18n::Message::CircleType,
          .m_conicShape = Poincare::Conic::Shape::Circle,
          .m_isOfDegreeTwo = true,
          .m_numberOfSubCurves = 2
        }
    );
    assert_check_function_properties(
      "x^2+2*y^2=12",
      noImplicitPlot ?
        k_bannedProperties :
        FunctionProperties{
          .m_caption = I18n::Message::EllipseType,
          .m_conicShape = Poincare::Conic::Shape::Ellipse,
          .m_isOfDegreeTwo = true,
          .m_numberOfSubCurves = 2
        }
    );
    assert_check_function_properties(
      "x=y^2",
      noImplicitPlot ?
        k_bannedProperties :
        FunctionProperties{
          .m_caption = I18n::Message::ParabolaType,
          .m_conicShape = Poincare::Conic::Shape::Parabola,
          .m_isOfDegreeTwo = true,
          .m_numberOfSubCurves = 2
        }
    );
    /* When implicit plots are disabled, these conics are no longer identified
     * to hide details */
    assert_check_function_properties(
      "y=x^2",
      noImplicitPlot ?
        FunctionProperties{
          .m_caption = I18n::Message::EquationType,
          .m_curveParameterType = ContinuousFunctionProperties::CurveParameterType::CartesianFunction
        } :
        FunctionProperties{
          .m_caption = I18n::Message::ParabolaType,
          .m_curveParameterType = ContinuousFunctionProperties::CurveParameterType::CartesianFunction,
          .m_conicShape = Poincare::Conic::Shape::Parabola,
          .m_isOfDegreeTwo = false
        }
    );
    assert_check_function_properties(
      "x^2-2*y^2=12",
      noImplicitPlot ?
        k_bannedProperties :
        FunctionProperties{
          .m_caption = I18n::Message::HyperbolaType,
          .m_conicShape = Poincare::Conic::Shape::Hyperbola,
          .m_isOfDegreeTwo = true,
          .m_numberOfSubCurves = 2
        }
    );
    assert_check_function_properties(
      "y*x=1",
      noImplicitPlot ?
        k_bannedProperties :
        FunctionProperties{
          .m_caption = I18n::Message::HyperbolaType,
          .m_curveParameterType = ContinuousFunctionProperties::CurveParameterType::CartesianFunction,
          .m_conicShape = Poincare::Conic::Shape::Hyperbola,
          .m_isOfDegreeTwo = false
        }
    );

    constexpr static FunctionProperties k_twoSubCurves = FunctionProperties{
      .m_caption = I18n::Message::EquationType,
      .m_isOfDegreeTwo = true,
      .m_numberOfSubCurves = 2
    };
    assert_check_function_properties("x^2-y^2=0", noImplicitPlot ? k_bannedProperties : k_twoSubCurves);
    assert_check_function_properties("x*y^2=1", noImplicitPlot ? k_bannedProperties : k_twoSubCurves);
    assert_check_function_properties("x^2-y^2+log(x)=0", noImplicitPlot ? k_bannedProperties : k_twoSubCurves);

    assert_check_function_properties(
      "y^2>-1",
      (noInequations || noImplicitPlot) ?
        k_bannedProperties :
        FunctionProperties{
          .m_caption = I18n::Message::InequalityType,
          .m_equationType = ComparisonNode::OperatorType::Superior,
          .m_isOfDegreeTwo = true,
          .m_numberOfSubCurves = 2
        },
      ContinuousFunctionProperties::AreaType::Outside
    );
    assert_check_function_properties(
      "(y-x+x^2)^2>=0",

      (noInequations || noImplicitPlot) ?
        k_bannedProperties :
        FunctionProperties{
          .m_caption = I18n::Message::InequalityType,
          .m_equationType = ComparisonNode::OperatorType::SuperiorEqual,
          .m_isOfDegreeTwo = true,
          .m_numberOfSubCurves = 2
        },
      ContinuousFunctionProperties::AreaType::Outside
    );

    // Error status
    assert_check_function_properties(
      "",
      FunctionProperties{
        .m_status = ContinuousFunctionProperties::Status::Undefined,
        .m_caption = I18n::Message::UndefinedType}
    );
    assert_check_function_properties(
      "y=log(0)",
      FunctionProperties{
        .m_status = ContinuousFunctionProperties::Status::Undefined,
        .m_caption = I18n::Message::UndefinedType}
    );
    assert_check_function_properties(
      "f(t)=[[cos(t)][t]]*log(0)",
      FunctionProperties{
        .m_status = ContinuousFunctionProperties::Status::Undefined,
        .m_caption = I18n::Message::UndefinedType,
        .m_symbolType = ContinuousFunctionProperties::SymbolType::T}
    );
    assert_check_function_properties(
      "r=θ+[[0][0]]",
      FunctionProperties{
        .m_status = ContinuousFunctionProperties::Status::Undefined,
        .m_caption = I18n::Message::UndefinedType,
        .m_symbolType = ContinuousFunctionProperties::SymbolType::Theta}
    );
    constexpr static FunctionProperties k_unhandledCartesian = FunctionProperties{.m_status = ContinuousFunctionProperties::Status::Unhandled, .m_caption = I18n::Message::UnhandledType};
    assert_check_function_properties("3=2", k_unhandledCartesian);

    assert_check_function_properties(
      "x*y^2>1",
      noInequations ?
        k_bannedProperties :
        FunctionProperties{
          .m_status = ContinuousFunctionProperties::Status::Unhandled,
          .m_caption = I18n::Message::UnhandledType,
          .m_equationType = Poincare::ComparisonNode::OperatorType::Superior}
    );

    assert_check_function_properties(
      "2-y^2>log(x)",
      noInequations ?
        k_bannedProperties :
        FunctionProperties{
          .m_status = ContinuousFunctionProperties::Status::Unhandled,
          .m_caption = I18n::Message::UnhandledType,
          .m_equationType = Poincare::ComparisonNode::OperatorType::Superior}
    );

    assert_check_function_properties("x*y^2=x", k_unhandledCartesian);

    assert_check_function_properties("y=i*x+1", k_unhandledCartesian);

    // Updated complex format
    assert(Poincare::Preferences::sharedPreferences()->complexFormat() == Preferences::ComplexFormat::Cartesian);
    assert_check_function_properties("y=(√(-1))^2", k_horizontalLineProperties);
    assert_check_function_properties("y=(i)^2", k_horizontalLineProperties);
    assert_check_function_properties(
      "f(x)=im(i*x+1)",
      FunctionProperties{
        .m_caption = I18n::Message::FunctionType,
        .m_curveParameterType = ContinuousFunctionProperties::CurveParameterType::CartesianFunction}
    );

    Poincare::Preferences::sharedPreferences()->setComplexFormat(Preferences::ComplexFormat::Real);
    assert_check_function_properties("y=(√(-1))^2", k_unhandledCartesian);
    assert_check_function_properties("y=(i)^2", k_horizontalLineProperties);
    assert_check_function_properties(
      "f(x)=im(i*x+1)",
      FunctionProperties{
        .m_caption = I18n::Message::FunctionType,
        .m_curveParameterType = ContinuousFunctionProperties::CurveParameterType::CartesianFunction}
    );
    // TODO : Handle this function
    assert_check_function_properties("y=im(i*x+1)", k_unhandledCartesian);
    // Restore preferences
    Poincare::Preferences::sharedPreferences()->setComplexFormat(Preferences::ComplexFormat::Cartesian);
  }
  // Restore an Off exam mode.
  Poincare::Preferences::sharedPreferences()->setExamMode(Poincare::Preferences::ExamMode::Off);
}

QUIZ_CASE(graph_function_plot_type_with_predefined_variables) {

    constexpr static FunctionProperties k_horizontalLineProperties = FunctionProperties{
      .m_caption = I18n::Message::HorizontalLineType,
      .m_curveParameterType = ContinuousFunctionProperties::CurveParameterType::HorizontalLine
    };

    constexpr static FunctionProperties k_lineProperties = FunctionProperties{
      .m_caption = I18n::Message::LineType,
      .m_curveParameterType = ContinuousFunctionProperties::CurveParameterType::Line
    };

    GlobalContext context;
    ContinuousFunctionStore store;
    // Add a predefined test function
    addFunction("test(x)=1+x", &store, &context);

    assert_check_function_properties("y=x", k_lineProperties, &store, &context);
    assert_check_function_properties("y=test(x)", k_lineProperties, &store, &context);
    assert_check_function_properties("y=a*x+1", k_lineProperties, &store, &context);
    assert_check_function_properties(
      "a*y*y+y=x",
      FunctionProperties{.m_caption = I18n::Message::EquationType, .m_isOfDegreeTwo = true, .m_numberOfSubCurves = 2},
      &store,
      &context
    );

    // Add a predefined a symbol
    assert_reduce_and_store("0→a", Preferences::AngleUnit::Radian, Poincare::Preferences::UnitFormat::Metric, Poincare::Preferences::ComplexFormat::Real);
    assert_check_function_properties("y=a*x+1", k_horizontalLineProperties, &store, &context);
    assert_check_function_properties("a*y*y+y=x", k_lineProperties, &store, &context);

    assert_reduce_and_store("1→a", Preferences::AngleUnit::Radian, Poincare::Preferences::UnitFormat::Metric, Poincare::Preferences::ComplexFormat::Real);
    assert_check_function_properties("y=a*x+1", k_lineProperties, &store, &context);
    assert_check_function_properties(
      "a*y*y+y=x",
      FunctionProperties{
        .m_caption = I18n::Message::ParabolaType,
        .m_conicShape = Poincare::Conic::Shape::Parabola,
        .m_isOfDegreeTwo = true,
        .m_numberOfSubCurves = 2
      },
      &store,
      &context
    );

    // Add a predefined y symbol
    assert_reduce_and_store("1→y", Preferences::AngleUnit::Radian, Poincare::Preferences::UnitFormat::Metric, Poincare::Preferences::ComplexFormat::Real);
    assert_check_function_properties("y=x", k_lineProperties, &store, &context);

    Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("a.exp").destroy();
    Ion::Storage::FileSystem::sharedFileSystem()->recordNamed("y.exp").destroy();
    store.removeAll();
}

}
