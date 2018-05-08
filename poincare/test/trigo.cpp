#include <quiz.h>
#include <poincare.h>
#include <ion.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_parse_trigo) {
  assert_parsed_expression_type("sin(0)", Expression::Type::Sine);
  assert_parsed_expression_type("cos(0)", Expression::Type::Cosine);
  assert_parsed_expression_type("tan(0)", Expression::Type::Tangent);
  assert_parsed_expression_type("cosh(0)", Expression::Type::HyperbolicCosine);
  assert_parsed_expression_type("sinh(0)", Expression::Type::HyperbolicSine);
  assert_parsed_expression_type("tanh(0)", Expression::Type::HyperbolicTangent);
  assert_parsed_expression_type("acos(0)", Expression::Type::ArcCosine);
  assert_parsed_expression_type("asin(0)", Expression::Type::ArcSine);
  assert_parsed_expression_type("atan(0)", Expression::Type::ArcTangent);
  assert_parsed_expression_type("acosh(0)", Expression::Type::HyperbolicArcCosine);
  assert_parsed_expression_type("asinh(0)", Expression::Type::HyperbolicArcSine);
  assert_parsed_expression_type("atanh(0)", Expression::Type::HyperbolicArcTangent);
}

QUIZ_CASE(poincare_trigo_evaluate) {
  Complex<double> a[1] = {Complex<double>::Float(-0.4161468365)};
  assert_parsed_expression_evaluates_to("cos(2)", a, Radian);
  Complex<float> a1[1] = {Complex<float>::Cartesian(-1.0086248134f, -0.8893951958f)};
  assert_parsed_expression_evaluates_to("cos(I-4)", a1, Radian);
  Complex<double> b[1] = {Complex<double>::Float(0.9092974268)};
  assert_parsed_expression_evaluates_to("sin(2)", b, Radian);
  Complex<float> b1[1] = {Complex<float>::Cartesian( 1.16780727488f, -0.768162763456f)};
  assert_parsed_expression_evaluates_to("sin(I-4)", b1, Radian);
  Complex<double> c[1] = {Complex<double>::Float(-2.18503986326151899)};
  assert_parsed_expression_evaluates_to("tan(2)", c, Radian);
  Complex<float> c1[1] = {Complex<float>::Cartesian(-0.27355308280730f, 1.002810507583504f)};
  assert_parsed_expression_evaluates_to("tan(I-4)", c1, Radian);
  Complex<double> a2[1] = {Complex<double>::Float(3.762195691)};
  assert_parsed_expression_evaluates_to("cosh(2)", a2, Radian);
  Complex<float> a3[1] = {Complex<float>::Cartesian(14.754701170483756280f,-22.96367349919304059f)};
  assert_parsed_expression_evaluates_to("cosh(I-4)", a3, Radian);
  Complex<double> b2[1] = {Complex<double>::Float(3.62686040784701876)};
  assert_parsed_expression_evaluates_to("sinh(2)", b2, Radian);
  Complex<float> b3[1] = {Complex<float>::Cartesian(-14.744805188558725031023f, 22.979085577886129555168f)};
  assert_parsed_expression_evaluates_to("sinh(I-4)", b3, Radian);
  Complex<double> c2[1] = {Complex<double>::Float(0.9640275800758168839464)};
  assert_parsed_expression_evaluates_to("tanh(2)", c2, Radian);
  Complex<float> c3[1] = {Complex<float>::Cartesian(-1.00027905623446556836909f, 0.000610240921376259f)};
  assert_parsed_expression_evaluates_to("tanh(I-4)", c3, Radian);
}

QUIZ_CASE(poincare_trigo_simplify) {
  // -- sin/cos -> tan
  assert_parsed_expression_simplify_to("sin(x)/cos(x)", "tan(x)");
  assert_parsed_expression_simplify_to("cos(x)/sin(x)", "1/tan(x)");
  assert_parsed_expression_simplify_to("sin(x)*P/cos(x)", "tan(x)*P");
  assert_parsed_expression_simplify_to("sin(x)/(P*cos(x))", "tan(x)/P");
  assert_parsed_expression_simplify_to("1*tan(2)*tan(5)", "tan(2)*tan(5)");
  assert_parsed_expression_simplify_to("tan(62P/21)", "-tan(P/21)");
  assert_parsed_expression_simplify_to("cos(26P/21)/sin(25P/17)", "cos((5*P)/21)/sin((8*P)/17)");
  assert_parsed_expression_simplify_to("cos(62P/21)*P*3/sin(62P/21)", "-(3*P)/tan(P/21)");
  assert_parsed_expression_simplify_to("cos(62P/21)/(P*3*sin(62P/21))", "-1/(3*tan(P/21)*P)");
  assert_parsed_expression_simplify_to("sin(62P/21)*P*3/cos(62P/21)", "-3*tan(P/21)*P");
  assert_parsed_expression_simplify_to("sin(62P/21)/(P*3cos(62P/21))", "-tan(P/21)/(3*P)");
  assert_parsed_expression_simplify_to("-cos(P/62)ln(3)/(sin(P/62)P)", "-ln(3)/(tan(P/62)*P)");
  assert_parsed_expression_simplify_to("-2cos(P/62)ln(3)/(sin(P/62)P)", "-(2*ln(3))/(tan(P/62)*P)");
  // -- cos
  assert_parsed_expression_simplify_to("cos(0)", "1");
  assert_parsed_expression_simplify_to("cos(P)", "-1");
  assert_parsed_expression_simplify_to("cos(P*4/7)", "-cos((3*P)/7)");
  assert_parsed_expression_simplify_to("cos(P*35/29)", "-cos((6*P)/29)");
  assert_parsed_expression_simplify_to("cos(-P*35/29)", "-cos((6*P)/29)");
  assert_parsed_expression_simplify_to("cos(P*340000)", "1");
  assert_parsed_expression_simplify_to("cos(-P*340001)", "-1");
  assert_parsed_expression_simplify_to("cos(-P*R(2))", "cos(R(2)*P)");
  assert_parsed_expression_simplify_to("cos(1311P/6)", "0");
  assert_parsed_expression_simplify_to("cos(P/12)", "(R(2)+R(6))/4");
  assert_parsed_expression_simplify_to("cos(-P/12)", "(R(2)+R(6))/4");
  assert_parsed_expression_simplify_to("cos(-P17/8)", "R(2+R(2))/2");
  assert_parsed_expression_simplify_to("cos(41P/6)", "-R(3)/2");
  assert_parsed_expression_simplify_to("cos(P/4+1000P)", "R(2)/2");
  assert_parsed_expression_simplify_to("cos(-P/3)", "1/2");
  assert_parsed_expression_simplify_to("cos(41P/5)", "(1+R(5))/4");
  assert_parsed_expression_simplify_to("cos(7P/10)", "-(R(2)*R(5-R(5)))/4");
    assert_parsed_expression_simplify_to("cos(0)", "1", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("cos(180)", "-1", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("cos(720/7)", "-cos(540/7)", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("cos(6300/29)", "-cos(1080/29)", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("cos(-6300/29)", "-cos(1080/29)", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("cos(61200000)", "1", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("cos(-61200180)", "-1", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("cos(-180*R(2))", "cos(180*R(2))", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("cos(39330)", "0", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("cos(15)", "(R(2)+R(6))/4", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("cos(-15)", "(R(2)+R(6))/4", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("cos(-765/2)", "R(2+R(2))/2", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("cos(7380/6)", "-R(3)/2", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("cos(180045)", "R(2)/2", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("cos(-60)", "1/2", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("cos(7380/5)", "(1+R(5))/4", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("cos(112.5)", "-R(2-R(2))/2", Expression::AngleUnit::Degree);
  // -- sin
  assert_parsed_expression_simplify_to("sin(0)", "0");
  assert_parsed_expression_simplify_to("sin(P)", "0");
  assert_parsed_expression_simplify_to("sin(P*35/29)", "-sin((6*P)/29)");
  assert_parsed_expression_simplify_to("sin(-P*35/29)", "sin((6*P)/29)");
  assert_parsed_expression_simplify_to("sin(P*340000)", "0");
  assert_parsed_expression_simplify_to("sin(P*340001)", "0");
  assert_parsed_expression_simplify_to("sin(-P*340001)", "0");
  assert_parsed_expression_simplify_to("sin(P/12)", "((-R(2))+R(6))/4");
  assert_parsed_expression_simplify_to("sin(-P/12)", "(R(2)-R(6))/4");
  assert_parsed_expression_simplify_to("sin(-P*R(2))", "-sin(R(2)*P)");
  assert_parsed_expression_simplify_to("sin(1311P/6)", "1");
  assert_parsed_expression_simplify_to("sin(-P17/8)", "-R(2-R(2))/2");
  assert_parsed_expression_simplify_to("sin(41P/6)", "1/2");
  assert_parsed_expression_simplify_to("sin(-3P/10)", "((-1)-R(5))/4");
  assert_parsed_expression_simplify_to("sin(P/4+1000P)", "R(2)/2");
  assert_parsed_expression_simplify_to("sin(-P/3)", "-R(3)/2");
  assert_parsed_expression_simplify_to("sin(17P/5)", "-(R(2)*R(5+R(5)))/4");
  assert_parsed_expression_simplify_to("sin(P/5)", "(R(2)*R(5-R(5)))/4");
  assert_parsed_expression_simplify_to("sin(0)", "0", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("sin(180)", "0", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("sin(6300/29)", "-sin(1080/29)", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("sin(-6300/29)", "sin(1080/29)", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("sin(61200000)", "0", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("sin(61200180)", "0", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("sin(-61200180)", "0", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("sin(15)", "((-R(2))+R(6))/4", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("sin(-15)", "(R(2)-R(6))/4", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("sin(-180*R(2))", "-sin(180*R(2))", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("sin(39330)", "1", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("sin(-765/2)", "-R(2-R(2))/2", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("sin(1230)", "1/2", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("sin(180045)", "R(2)/2", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("sin(-60)", "-R(3)/2", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("sin(612)", "-(R(2)*R(5+R(5)))/4", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("sin(36)", "(R(2)*R(5-R(5)))/4", Expression::AngleUnit::Degree);
  // -- tan
  assert_parsed_expression_simplify_to("tan(0)", "0");
  assert_parsed_expression_simplify_to("tan(P)", "0");
  assert_parsed_expression_simplify_to("tan(P*35/29)", "tan((6*P)/29)");
  assert_parsed_expression_simplify_to("tan(-P*35/29)", "-tan((6*P)/29)");
  assert_parsed_expression_simplify_to("tan(P*340000)", "0");
  assert_parsed_expression_simplify_to("tan(P*340001)", "0");
  assert_parsed_expression_simplify_to("tan(-P*340001)", "0");
  assert_parsed_expression_simplify_to("tan(P/12)", "2-R(3)");
  assert_parsed_expression_simplify_to("tan(-P/12)", "(-2)+R(3)");
  assert_parsed_expression_simplify_to("tan(-P*R(2))", "-tan(R(2)*P)");
  assert_parsed_expression_simplify_to("tan(1311P/6)", "undef");
  assert_parsed_expression_simplify_to("tan(-P17/8)", "1-R(2)");
  assert_parsed_expression_simplify_to("tan(41P/6)", "-R(3)/3");
  assert_parsed_expression_simplify_to("tan(P/4+1000P)", "1");
  assert_parsed_expression_simplify_to("tan(-P/3)", "-R(3)");
  assert_parsed_expression_simplify_to("tan(-P/10)", "-(R(5)*R(5-2*R(5)))/5");
  assert_parsed_expression_simplify_to("tan(0)", "0", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("tan(180)", "0", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("tan(6300/29)", "tan(1080/29)", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("tan(-6300/29)", "-tan(1080/29)", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("tan(61200000)", "0", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("tan(61200180)", "0", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("tan(-61200180)", "0", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("tan(15)", "2-R(3)", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("tan(-15)", "(-2)+R(3)", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("tan(-180*R(2))", "-tan(180*R(2))", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("tan(39330)", "undef", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("tan(-382.5)", "1-R(2)", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("tan(1230)", "-R(3)/3", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("tan(180045)", "1", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("tan(-60)", "-R(3)", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("tan(tan(tan(tan(9))))", "tan(tan(tan(tan(9))))");
  // -- acos
  assert_parsed_expression_simplify_to("acos(-1/2)", "(2*P)/3");
  assert_parsed_expression_simplify_to("acos(-1.2)", "undef");
  assert_parsed_expression_simplify_to("acos(cos(2/3))", "2/3");
  assert_parsed_expression_simplify_to("acos(cos(3/2))", "3/2");
  assert_parsed_expression_simplify_to("cos(acos(3/2))", "undef");
  assert_parsed_expression_simplify_to("cos(acos(2/3))", "2/3");
  assert_parsed_expression_simplify_to("acos(cos(12))", "acos(cos(12))");
  assert_parsed_expression_simplify_to("acos(cos(4P/7))", "(4*P)/7");
  assert_parsed_expression_simplify_to("acos(-cos(2))", "(-2)+P");
  assert_parsed_expression_simplify_to("acos(-1/2)", "120", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("acos(-1.2)", "undef", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("acos(cos(2/3))", "2/3", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("acos(cos(190))", "170", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("acos(cos(75))", "75", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("cos(acos(190))", "undef", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("cos(acos(75))", "undef", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("acos(cos(12))", "12", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("acos(cos(720/7))", "720/7", Expression::AngleUnit::Degree);
  // -- asin
  assert_parsed_expression_simplify_to("asin(-1/2)", "-P/6");
  assert_parsed_expression_simplify_to("asin(-1.2)", "undef");
  assert_parsed_expression_simplify_to("asin(sin(2/3))", "2/3");
  assert_parsed_expression_simplify_to("sin(asin(2/3))", "2/3");
  assert_parsed_expression_simplify_to("sin(asin(3/2))", "undef");
  assert_parsed_expression_simplify_to("asin(sin(3/2))", "3/2");
  assert_parsed_expression_simplify_to("asin(sin(12))", "asin(sin(12))");
  assert_parsed_expression_simplify_to("asin(sin(-P/7))", "-P/7");
  assert_parsed_expression_simplify_to("asin(sin(-R(2)))", "-R(2)");
  assert_parsed_expression_simplify_to("asin(-1/2)", "-30", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("asin(-1.2)", "undef", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("asin(sin(75))", "75", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("sin(asin(75))", "undef", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("sin(asin(190))", "undef", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("asin(sin(32))", "32", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("asin(sin(400))", "40", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("asin(sin(-180/7))", "-180/7", Expression::AngleUnit::Degree);
  // -- atan
  assert_parsed_expression_simplify_to("atan(-1)", "-P/4");
  assert_parsed_expression_simplify_to("atan(-1.2)", "-atan(6/5)");
  assert_parsed_expression_simplify_to("atan(tan(2/3))", "2/3");
  assert_parsed_expression_simplify_to("tan(atan(2/3))", "2/3");
  assert_parsed_expression_simplify_to("tan(atan(5/2))", "5/2");
  assert_parsed_expression_simplify_to("atan(tan(5/2))", "atan(tan(5/2))");
  assert_parsed_expression_simplify_to("atan(tan(5/2))", "atan(tan(5/2))");
  assert_parsed_expression_simplify_to("atan(tan(-P/7))", "-P/7");
  assert_parsed_expression_simplify_to("atan(R(3))", "P/3");
  assert_parsed_expression_simplify_to("atan(tan(-R(2)))", "-R(2)");
  assert_parsed_expression_simplify_to("atan(-1)", "-45", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("atan(-1.2)", "-atan(6/5)", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("atan(tan(-45))", "-45", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("tan(atan(120))", "120", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("tan(atan(2293))", "2293", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("atan(tan(2293))", "-47", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("atan(tan(1808))", "8", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("atan(tan(-180/7))", "-180/7", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("atan(R(3))", "60", Expression::AngleUnit::Degree);
}
