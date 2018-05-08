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
  /* cos: R  ->  R (oscillator)
   *      Ri ->  R (even)
   */
  // On R
  assert_parsed_expression_evaluates_to<double>("cos(2)", "-4.1614683654714E-1", Radian);
  assert_parsed_expression_evaluates_to<double>("cos(2)", "0.9993908270191", Degree);
  // Oscillator
  assert_parsed_expression_evaluates_to<float>("cos(P/2)", "0", Radian);
  assert_parsed_expression_evaluates_to<double>("cos(3*P/2)", "0", Radian);
  assert_parsed_expression_evaluates_to<float>("cos(3*P)", "-1", Radian);
  assert_parsed_expression_evaluates_to<float>("cos(-540)", "-1", Degree);
  // On R*i
  assert_parsed_expression_evaluates_to<double>("cos(-2*I)", "3.7621956910836", Radian);
  assert_parsed_expression_evaluates_to<double>("cos(-2*I)", "1.0006092967033", Degree);
  // Symmetry: even
  assert_parsed_expression_evaluates_to<double>("cos(2*I)", "3.7621956910836", Radian);
  assert_parsed_expression_evaluates_to<double>("cos(2*I)", "1.0006092967033", Degree);
  // On C
  assert_parsed_expression_evaluates_to<float>("cos(I-4)", "(-1.008625)-0.889395*I", Radian);
  assert_parsed_expression_evaluates_to<float>("cos(I-4)", "0.997716+0.001218*I", Degree, Cartesian, 6);

  /* sin: R  ->  R (oscillator)
   *      Ri ->  Ri (odd)
   */
  // On R
  assert_parsed_expression_evaluates_to<double>("sin(2)", "9.0929742682568E-1", Radian);
  assert_parsed_expression_evaluates_to<double>("sin(2)", "0.0348994967025", Degree);
  // Oscillator
  assert_parsed_expression_evaluates_to<float>("sin(P/2)", "1", Radian);
  assert_parsed_expression_evaluates_to<double>("sin(3*P/2)", "-1", Radian);
  assert_parsed_expression_evaluates_to<float>("sin(3*P)", "0", Radian);
  assert_parsed_expression_evaluates_to<float>("sin(-540)", "0", Degree);
  // On R*i
  assert_parsed_expression_evaluates_to<double>("sin(3*I)", "10.01787492741*I", Radian);
  assert_parsed_expression_evaluates_to<float>("sin(3*I)", "0.052384*I", Degree);
  // Symmetry: odd
  assert_parsed_expression_evaluates_to<double>("sin(-3*I)", "-10.01787492741*I", Radian);
  assert_parsed_expression_evaluates_to<float>("sin(-3*I)", "-0.052384*I", Degree);
  // On: C
  assert_parsed_expression_evaluates_to<float>("sin(I-4)", "1.16781-0.768163*I", Radian, Cartesian, 6);
  assert_parsed_expression_evaluates_to<float>("sin(I-4)", "(-0.069767)+0.017412*I", Degree, Cartesian, 6);

  /* tan: R  ->  R (tangent-style)
   *      Ri ->  Ri (odd)
   */
  // On R
  assert_parsed_expression_evaluates_to<double>("tan(2)", "-2.1850398632615", Radian);
  assert_parsed_expression_evaluates_to<double>("tan(2)", "3.492076949175E-2", Degree);
  // Tangent-style
  assert_parsed_expression_evaluates_to<float>("tan(P/2)", "undef", Radian);
  assert_parsed_expression_evaluates_to<double>("tan(3*P/2)", "undef", Radian);
  assert_parsed_expression_evaluates_to<float>("tan(3*P)", "0", Radian);
  assert_parsed_expression_evaluates_to<float>("tan(-540)", "0", Degree);
  // On R*i
  assert_parsed_expression_evaluates_to<double>("tan(-2*I)", "-9.6402758007582E-1*I", Radian);
  assert_parsed_expression_evaluates_to<float>("tan(-2*I)", "-0.034892*I", Degree);
  // Symmetry: odd
  assert_parsed_expression_evaluates_to<double>("tan(2*I)", "9.6402758007582E-1*I", Radian);
  assert_parsed_expression_evaluates_to<float>("tan(2*I)", "0.034892*I", Degree);
  // On C
  assert_parsed_expression_evaluates_to<float>("tan(I-4)", "(-0.273553)+1.002811*I", Radian);
  assert_parsed_expression_evaluates_to<float>("tan(I-4)", "(-0.069905)+0.017537*I", Degree, Cartesian);

  /* acos: [-1,1]    ->  R
   *       ]-inf,-1[ -> Pi+R*i (odd imaginary)
   *       ]1, inf[  -> R*i (odd imaginary)
   *       R*i       -> Pi/2+R*i (odd imaginary)
   */
  // On [-1, 1]
  assert_parsed_expression_evaluates_to<double>("acos(0.5)", "1.0471975511966", Radian);
  assert_parsed_expression_evaluates_to<double>("acos(0.5)", "60", Degree);
  // On [1, inf[
  assert_parsed_expression_evaluates_to<double>("acos(2)", "1.3169578969248*I", Radian);
  assert_parsed_expression_evaluates_to<double>("acos(2)", "75.456129290217*I", Degree);
  // Symmetry: odd on imaginary
  assert_parsed_expression_evaluates_to<double>("acos(-2)", "3.1415926535898-1.3169578969248*I", Radian);
  assert_parsed_expression_evaluates_to<double>("acos(-2)", "180-75.456129290217*I", Degree);
  // On ]-inf, -1[
  assert_parsed_expression_evaluates_to<double>("acos(-32)", "3.1415926535898-4.1586388532792*I", Radian);
  assert_parsed_expression_evaluates_to<float>("acos(-32)", "180-238.2725*I", Degree);
  // On R*i
  //assert_parsed_expression_evaluates_to<float>("acos(3*I)", "1.5707963-1.818446*I", Radian);
  //assert_parsed_expression_evaluates_to<float>("acos(3*I)", "90-104.1892*I", Degree);
  // Symmetry: odd on imaginary
  //assert_parsed_expression_evaluates_to<float>("acos(-3*I)", "1.5707963+1.818446*I", Radian);
  //assert_parsed_expression_evaluates_to<float>("acos(-3*I)", "90+104.1892*I", Degree);
  // On C
  assert_parsed_expression_evaluates_to<float>("acos(I-4)", "2.8894-2.0966*I", Radian, Cartesian, 5);
  assert_parsed_expression_evaluates_to<float>("acos(I-4)", "165.551-120.126*I", Degree, Cartesian, 6);
  // Key values
  assert_parsed_expression_evaluates_to<double>("acos(0)", "90", Degree);
  assert_parsed_expression_evaluates_to<float>("acos(-1)", "180", Degree);
  assert_parsed_expression_evaluates_to<double>("acos(1)", "0", Degree);

  /* asin: [-1,1]    ->  R
   *       ]-inf,-1[ -> -Pi/2+R*i (odd)
   *       ]1, inf[  -> Pi/2+R*i (odd)
   *       R*i       -> R*i (odd)
   */
  // On [-1, 1]
  assert_parsed_expression_evaluates_to<double>("asin(0.5)", "0.5235987755983", Radian);
  assert_parsed_expression_evaluates_to<double>("asin(0.5)", "30", Degree);
  // On [1, inf[
  assert_parsed_expression_evaluates_to<double>("asin(2)", "1.5707963267949-1.3169578969248*I", Radian);
  assert_parsed_expression_evaluates_to<double>("asin(2)", "90-75.456129290217*I", Degree);
  // Symmetry: odd
  assert_parsed_expression_evaluates_to<double>("asin(-2)", "(-1.5707963267949)+1.3169578969248*I", Radian);
  assert_parsed_expression_evaluates_to<double>("asin(-2)", "(-90)+75.456129290217*I", Degree);
  // On ]-inf, -1[
  assert_parsed_expression_evaluates_to<float>("asin(-32)", "(-1.571)+4.159*I", Radian, Cartesian, 4);
  assert_parsed_expression_evaluates_to<float>("asin(-32)", "(-90)+238*I", Degree, Cartesian, 3);
  // On R*i
  assert_parsed_expression_evaluates_to<double>("asin(3*I)", "1.8184464592321*I", Radian);
  // Symmetry: odd
  assert_parsed_expression_evaluates_to<double>("asin(-3*I)", "-1.8184464592321*I", Radian);
  // On C
  assert_parsed_expression_evaluates_to<float>("asin(I-4)", "(-1.3186)+2.0966*I", Radian, Cartesian, 5);
  assert_parsed_expression_evaluates_to<float>("asin(I-4)", "(-75.551)+120.13*I", Degree, Cartesian, 5);
  // Key values
  assert_parsed_expression_evaluates_to<double>("asin(0)", "0", Degree);
  assert_parsed_expression_evaluates_to<float>("asin(-1)", "-90", Degree);
  assert_parsed_expression_evaluates_to<double>("asin(1)", "90", Degree);

  /* atan: R         ->  R (odd)
   *       [-i,i]    ->  R*i (odd)
   *       ]-inf*i,-i[ -> -Pi/2+R*i (odd)
   *       ]i, inf*i[  -> Pi/2+R*i (odd)
   */
  // On R
  assert_parsed_expression_evaluates_to<double>("atan(2)", "1.1071487177941", Radian);
  assert_parsed_expression_evaluates_to<double>("atan(2)", "63.434948822922", Degree);
  assert_parsed_expression_evaluates_to<float>("atan(0.5)", "0.463648", Radian);
  // Symmetry: odd
  assert_parsed_expression_evaluates_to<double>("atan(-2)", "-1.1071487177941", Radian);
  // On [-i, i]
  assert_parsed_expression_evaluates_to<float>("atan(0.2*I)", "0.202733*I", Radian, Cartesian, 6);
  // Symmetry: odd
  assert_parsed_expression_evaluates_to<float>("atan(-0.2*I)", "-0.202733*I", Radian, Cartesian, 6);
  // On [i, inf*i[
  assert_parsed_expression_evaluates_to<double>("atan(26*I)", "1.5707963267949+3.848052056806E-2*I", Radian);
  assert_parsed_expression_evaluates_to<double>("atan(26*I)", "90+2.2047714220162*I", Degree);
  // Symmetry: odd
  assert_parsed_expression_evaluates_to<double>("atan(-26*I)", "(-1.5707963267949)-3.848052056806E-2*I", Radian);
  // On ]-inf*i, -i[
  assert_parsed_expression_evaluates_to<float>("atan(-3.4*I)", "(-1.570796)-0.303068*I", Radian);
  assert_parsed_expression_evaluates_to<float>("atan(-3.4*I)", "(-90)-17.3645*I", Degree, Cartesian, 6);
  // On C
  assert_parsed_expression_evaluates_to<float>("atan(I-4)", "(-1.338973)+0.055786*I", Radian);
  assert_parsed_expression_evaluates_to<float>("atan(I-4)", "(-76.7175)+3.1963*I", Degree, Cartesian, 6);
  // Key values
  assert_parsed_expression_evaluates_to<float>("atan(0)", "0", Degree);
  assert_parsed_expression_evaluates_to<double>("atan(-I)", "undef", Radian);
  assert_parsed_expression_evaluates_to<double>("atan(I)", "undef", Radian);

  /* cosh: R         -> R (even)
   *       R*i       -> R (oscillator)
   */
  // On R
  assert_parsed_expression_evaluates_to<double>("cosh(2)", "3.7621956910836", Radian);
  assert_parsed_expression_evaluates_to<double>("cosh(2)", "3.7621956910836", Degree);
  // Symmetry: even
  assert_parsed_expression_evaluates_to<double>("cosh(-2)", "3.7621956910836", Radian);
  assert_parsed_expression_evaluates_to<double>("cosh(-2)", "3.7621956910836", Degree);
  // On R*i
  assert_parsed_expression_evaluates_to<double>("cosh(43*I)", "5.5511330152063E-1", Radian);
  // Oscillator
  assert_parsed_expression_evaluates_to<float>("cosh(P*I/2)", "0", Radian);
  assert_parsed_expression_evaluates_to<float>("cosh(5*P*I/2)", "0", Radian);
  assert_parsed_expression_evaluates_to<float>("cosh(8*P*I/2)", "1", Radian);
  assert_parsed_expression_evaluates_to<float>("cosh(9*P*I/2)", "0", Radian);
  // On C
  assert_parsed_expression_evaluates_to<float>("cosh(I-4)", "14.7547-22.9637*I", Radian, Cartesian, 6);
  assert_parsed_expression_evaluates_to<float>("cosh(I-4)", "14.7547-22.9637*I", Degree, Cartesian, 6);

  /* sinh: R         -> R (odd)
   *       R*i       -> R*i (oscillator)
   */
  // On R
  assert_parsed_expression_evaluates_to<double>("sinh(2)", "3.626860407847", Radian);
  assert_parsed_expression_evaluates_to<double>("sinh(2)", "3.626860407847", Degree);
  // Symmetry: odd
  assert_parsed_expression_evaluates_to<double>("sinh(-2)", "-3.626860407847", Radian);
  // On R*i
  assert_parsed_expression_evaluates_to<double>("sinh(43*I)", "-0.8317747426286*I", Radian);
  // Oscillator
  assert_parsed_expression_evaluates_to<float>("sinh(P*I/2)", "I", Radian);
  assert_parsed_expression_evaluates_to<float>("sinh(5*P*I/2)", "I", Radian);
  assert_parsed_expression_evaluates_to<float>("sinh(7*P*I/2)", "-I", Radian);
  assert_parsed_expression_evaluates_to<float>("sinh(8*P*I/2)", "0", Radian);
  assert_parsed_expression_evaluates_to<float>("sinh(9*P*I/2)", "I", Radian);
  // On C
  assert_parsed_expression_evaluates_to<float>("sinh(I-4)", "(-14.7448)+22.9791*I", Radian, Cartesian, 6);
  assert_parsed_expression_evaluates_to<float>("sinh(I-4)", "(-14.7448)+22.9791*I", Degree, Cartesian, 6);

  /* tanh: R         -> R (odd)
   *       R*i       -> R*i (tangent-style)
   */
  // On R
  assert_parsed_expression_evaluates_to<double>("tanh(2)", "9.6402758007582E-1", Radian);
  // Symmetry: odd
  assert_parsed_expression_evaluates_to<double>("tanh(-2)", "-9.6402758007582E-1", Degree);
  // On R*i
  assert_parsed_expression_evaluates_to<double>("tanh(43*I)", "-1.4983873388552*I", Radian);
  // Tangent-style
  assert_parsed_expression_evaluates_to<float>("tanh(P*I/2)", "undef", Radian);
  assert_parsed_expression_evaluates_to<float>("tanh(5*P*I/2)", "undef", Radian);
  assert_parsed_expression_evaluates_to<float>("tanh(7*P*I/2)", "undef", Radian);
  assert_parsed_expression_evaluates_to<float>("tanh(8*P*I/2)", "0", Radian);
  assert_parsed_expression_evaluates_to<float>("tanh(9*P*I/2)", "undef", Radian);
  // On C
  assert_parsed_expression_evaluates_to<float>("tanh(I-4)", "(-1.000279)+0.00061*I", Radian);
  assert_parsed_expression_evaluates_to<float>("tanh(I-4)", "(-1.000279)+0.00061*I", Degree);

  /* acosh: [-1,1]       ->  R*i
   *        ]-inf,-1[    -> Pi*i+R (even on real)
   *        ]1, inf[     -> R (even on real)
   *        ]-inf*i, 0[  -> -Pi/2*i+R (even on real)
   *        ]0, inf*i[   -> Pi/2*i+R (even on real)
   */
  // On [-1,1]
  assert_parsed_expression_evaluates_to<double>("acosh(2)", "1.3169578969248", Radian);
  assert_parsed_expression_evaluates_to<double>("acosh(2)", "1.3169578969248", Degree);
  // On ]-inf, -1[
  assert_parsed_expression_evaluates_to<double>("acosh(-4)", "2.0634370688956+3.1415926535898*I", Radian);
  assert_parsed_expression_evaluates_to<float>("acosh(-4)", "2.06344+3.14159*I", Radian, Cartesian, 6);
  // On ]1,inf[: Symmetry: even on real
  assert_parsed_expression_evaluates_to<double>("acosh(4)", "2.0634370688956", Radian);
  assert_parsed_expression_evaluates_to<float>("acosh(4)", "2.063437", Radian);
  // On ]-inf*i, 0[
  assert_parsed_expression_evaluates_to<double>("acosh(-42*I)", "4.4309584920805-1.5707963267949*I", Radian);
  assert_parsed_expression_evaluates_to<float>("acosh(-42*I)", "4.431-1.571*I", Radian, Cartesian, 4);
  // On ]0, i*inf[: Symmetry: even on real
  assert_parsed_expression_evaluates_to<double>("acosh(42*I)", "4.4309584920805+1.5707963267949*I", Radian);
  assert_parsed_expression_evaluates_to<float>("acosh(42*I)", "4.431+1.571*I", Radian, Cartesian, 4);
  // On C
  assert_parsed_expression_evaluates_to<float>("acosh(I-4)", "2.0966+2.8894*I", Radian, Cartesian, 5);
  assert_parsed_expression_evaluates_to<float>("acosh(I-4)", "2.0966+2.8894*I", Degree, Cartesian, 5);
  // Key values
  //assert_parsed_expression_evaluates_to<double>("acosh(-1)", "3.1415926535898*I", Radian);
  assert_parsed_expression_evaluates_to<double>("acosh(1)", "0", Radian);
  assert_parsed_expression_evaluates_to<float>("acosh(0)", "1.570796*I", Radian);

  /* acosh: R            -> R (odd)
   *        [-i,i]       ->  R*i (odd)
   *        ]-inf*i,-i[    -> -Pi/2*i+R (odd)
   *        ]i, inf*I[     -> Pi/2*I+R (odd)
   */
  // On R
  assert_parsed_expression_evaluates_to<double>("asinh(2)", "1.4436354751788", Radian);
  assert_parsed_expression_evaluates_to<double>("asinh(2)", "1.4436354751788", Degree);
  // Symmetry: odd
  assert_parsed_expression_evaluates_to<double>("asinh(-2)", "-1.4436354751788", Radian);
  assert_parsed_expression_evaluates_to<double>("asinh(-2)", "-1.4436354751788", Degree);
  // On [-i,i]
  assert_parsed_expression_evaluates_to<double>("asinh(0.2*I)", "2.0135792079033E-1*I", Radian);
  assert_parsed_expression_evaluates_to<float>("asinh(0.2*I)", "0.201358*I", Degree);
  // Symmetry: odd
  assert_parsed_expression_evaluates_to<double>("asinh(-0.2*I)", "-2.0135792079033E-1*I", Radian);
  assert_parsed_expression_evaluates_to<float>("asinh(-0.2*I)", "-0.201358*I", Degree);
  // On ]-inf*i, -i[
  assert_parsed_expression_evaluates_to<double>("asinh(-22*I)", "(-3.7836727043295)-1.5707963267949*I", Radian);
  assert_parsed_expression_evaluates_to<float>("asinh(-22*I)", "(-3.784)-1.571*I", Degree, Cartesian, 4);
  // On ]i, inf*i[, Symmetry: odd
  assert_parsed_expression_evaluates_to<double>("asinh(22*I)", "3.7836727043295+1.5707963267949*I", Radian);
  assert_parsed_expression_evaluates_to<float>("asinh(22*I)", "3.784+1.571*I", Degree, Cartesian, 4);
  // On C
  assert_parsed_expression_evaluates_to<float>("asinh(I-4)", "(-2.123)+0.2383*I", Radian, Cartesian, 4);
  assert_parsed_expression_evaluates_to<float>("asinh(I-4)", "(-2.123)+0.2383*I", Degree, Cartesian, 4);

  /* acosh: [-1,1]       -> R (odd)
   *        ]-inf,-1[    -> Pi/2*i+R (odd)
   *        ]1, inf[     -> -Pi/2*i+R (odd)
   *        R*i          -> R*i (odd)
   */
  // On [-1,1]
  assert_parsed_expression_evaluates_to<double>("atanh(0.4)", "0.4236489301936", Radian);
  assert_parsed_expression_evaluates_to<double>("atanh(0.4)", "0.4236489301936", Degree);
  // Symmetry: odd
  assert_parsed_expression_evaluates_to<double>("atanh(-0.4)", "-0.4236489301936", Radian);
  assert_parsed_expression_evaluates_to<double>("atanh(-0.4)", "-0.4236489301936", Degree);
  // On ]1, inf[
  assert_parsed_expression_evaluates_to<double>("atanh(4)", "0.255412811883-1.5707963267949*I", Radian);
  assert_parsed_expression_evaluates_to<float>("atanh(4)", "0.255413-1.570796*I", Degree);
  // On ]-inf,-1[, Symmetry: odd
  assert_parsed_expression_evaluates_to<double>("atanh(-4)", "(-0.255412811883)+1.5707963267949*I", Radian);
  assert_parsed_expression_evaluates_to<float>("atanh(-4)", "(-0.255413)+1.570796*I", Degree);
  // On R*i
  assert_parsed_expression_evaluates_to<double>("atanh(4*I)", "1.325817663668*I", Radian);
  assert_parsed_expression_evaluates_to<float>("atanh(4*I)", "1.325818*I", Radian);
  // Symmetry: odd
  assert_parsed_expression_evaluates_to<double>("atanh(-4*I)", "-1.325817663668*I", Radian);
  assert_parsed_expression_evaluates_to<float>("atanh(-4*I)", "-1.325818*I", Radian);
  // On C
  assert_parsed_expression_evaluates_to<float>("atanh(I-4)", "(-0.238878)+1.50862*I", Radian, Cartesian, 6);
  assert_parsed_expression_evaluates_to<float>("atanh(I-4)", "(-0.238878)+1.50862*I", Degree, Cartesian, 6);

  // WARNING: evaluate on branch cut can be multivalued
  assert_parsed_expression_evaluates_to<double>("acos(2)", "1.3169578969248*I", Radian);
  assert_parsed_expression_evaluates_to<double>("acos(2)", "75.456129290217*I", Degree);
  assert_parsed_expression_evaluates_to<double>("asin(2)", "1.5707963267949-1.3169578969248*I", Radian);
  assert_parsed_expression_evaluates_to<double>("asin(2)", "90-75.456129290217*I", Degree);
  assert_parsed_expression_evaluates_to<double>("atanh(2)", "5.4930614433405E-1-1.5707963267949*I", Radian);
  assert_parsed_expression_evaluates_to<double>("atan(2I)", "1.5707963267949+5.4930614433405E-1*I", Radian);
  assert_parsed_expression_evaluates_to<double>("atan(2I)", "90+31.472923730945*I", Degree);
  assert_parsed_expression_evaluates_to<double>("asinh(2I)", "1.3169578969248+1.5707963267949*I", Radian);
  assert_parsed_expression_evaluates_to<double>("acosh(-2)", "1.3169578969248+3.1415926535898*I", Radian);
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
  assert_parsed_expression_simplify_to("acos(-1.2)", "(-acos(6/5))+P");
  assert_parsed_expression_simplify_to("acos(cos(2/3))", "2/3");
  assert_parsed_expression_simplify_to("acos(cos(3/2))", "3/2");
  assert_parsed_expression_simplify_to("cos(acos(3/2))", "3/2");
  assert_parsed_expression_simplify_to("cos(acos(2/3))", "2/3");
  assert_parsed_expression_simplify_to("acos(cos(12))", "acos(cos(12))");
  assert_parsed_expression_simplify_to("acos(cos(4P/7))", "(4*P)/7");
  assert_parsed_expression_simplify_to("acos(-cos(2))", "(-2)+P");
  assert_parsed_expression_simplify_to("acos(-1/2)", "120", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("acos(-1.2)", "180-acos(6/5)", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("acos(cos(2/3))", "2/3", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("acos(cos(190))", "170", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("acos(cos(75))", "75", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("cos(acos(190))", "190", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("cos(acos(75))", "75", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("acos(cos(12))", "12", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("acos(cos(720/7))", "720/7", Expression::AngleUnit::Degree);
  // -- asin
  assert_parsed_expression_simplify_to("asin(-1/2)", "-P/6");
  assert_parsed_expression_simplify_to("asin(-1.2)", "-asin(6/5)");
  assert_parsed_expression_simplify_to("asin(sin(2/3))", "2/3");
  assert_parsed_expression_simplify_to("sin(asin(2/3))", "2/3");
  assert_parsed_expression_simplify_to("sin(asin(3/2))", "3/2");
  assert_parsed_expression_simplify_to("asin(sin(3/2))", "3/2");
  assert_parsed_expression_simplify_to("asin(sin(12))", "asin(sin(12))");
  assert_parsed_expression_simplify_to("asin(sin(-P/7))", "-P/7");
  assert_parsed_expression_simplify_to("asin(sin(-R(2)))", "-R(2)");
  assert_parsed_expression_simplify_to("asin(-1/2)", "-30", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("asin(-1.2)", "-asin(6/5)", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("asin(sin(75))", "75", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("sin(asin(75))", "75", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("sin(asin(190))", "190", Expression::AngleUnit::Degree);
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
