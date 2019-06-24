#include <quiz.h>
#include <poincare/expression.h>
#include <ion.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_parse_trigo) {
  assert_parsed_expression_type("sin(0)", ExpressionNode::Type::Sine);
  assert_parsed_expression_type("cos(0)", ExpressionNode::Type::Cosine);
  assert_parsed_expression_type("tan(0)", ExpressionNode::Type::Tangent);
  assert_parsed_expression_type("cosh(0)", ExpressionNode::Type::HyperbolicCosine);
  assert_parsed_expression_type("sinh(0)", ExpressionNode::Type::HyperbolicSine);
  assert_parsed_expression_type("tanh(0)", ExpressionNode::Type::HyperbolicTangent);
  assert_parsed_expression_type("acos(0)", ExpressionNode::Type::ArcCosine);
  assert_parsed_expression_type("asin(0)", ExpressionNode::Type::ArcSine);
  assert_parsed_expression_type("atan(0)", ExpressionNode::Type::ArcTangent);
  assert_parsed_expression_type("acosh(0)", ExpressionNode::Type::HyperbolicArcCosine);
  assert_parsed_expression_type("asinh(0)", ExpressionNode::Type::HyperbolicArcSine);
  assert_parsed_expression_type("atanh(0)", ExpressionNode::Type::HyperbolicArcTangent);
}

QUIZ_CASE(poincare_trigo_evaluate) {
  /* cos: R  ->  R (oscillator)
   *      Ri ->  R (even)
   */
  // On R
  assert_parsed_expression_evaluates_to<double>("cos(2)", "-4.1614683654714ᴇ-1", System, Radian);
  assert_parsed_expression_evaluates_to<double>("cos(2)", "0.9993908270191", System, Degree);
  assert_parsed_expression_evaluates_to<double>("cos(2)", "9.9950656036573ᴇ-1", System, Gradian);
  // Oscillator
  assert_parsed_expression_evaluates_to<float>("cos(π/2)", "0", System, Radian);
  assert_parsed_expression_evaluates_to<double>("cos(3×π/2)", "0", System, Radian);
  assert_parsed_expression_evaluates_to<float>("cos(3×π)", "-1", System, Radian);
  assert_parsed_expression_evaluates_to<float>("cos(-540)", "-1", System, Degree);
  // On R×i
  assert_parsed_expression_evaluates_to<double>("cos(-2×𝐢)", "3.7621956910836", System, Radian);
  assert_parsed_expression_evaluates_to<double>("cos(-2×𝐢)", "1.0006092967033", System, Degree);
  // Symmetry: even
  assert_parsed_expression_evaluates_to<double>("cos(2×𝐢)", "3.7621956910836", System, Radian);
  assert_parsed_expression_evaluates_to<double>("cos(2×𝐢)", "1.0006092967033", System, Degree);
  // On C
  assert_parsed_expression_evaluates_to<float>("cos(𝐢-4)", "-1.008625-0.8893952×𝐢", System, Radian);
  assert_parsed_expression_evaluates_to<float>("cos(𝐢-4)", "0.997716+0.00121754×𝐢", System, Degree, Cartesian, 6);

  /* sin: R  ->  R (oscillator)
   *      Ri ->  Ri (odd)
   */
  // On R
  assert_parsed_expression_evaluates_to<double>("sin(2)", "9.0929742682568ᴇ-1", System, Radian);
  assert_parsed_expression_evaluates_to<double>("sin(2)", "3.4899496702501ᴇ-2", System, Degree);
  assert_parsed_expression_evaluates_to<double>("sin(2)", "3.1410759078128ᴇ-2", System, Gradian);
  // Oscillator
  assert_parsed_expression_evaluates_to<float>("sin(π/2)", "1", System, Radian);
  assert_parsed_expression_evaluates_to<double>("sin(3×π/2)", "-1", System, Radian);
  assert_parsed_expression_evaluates_to<float>("sin(3×π)", "0", System, Radian);
  assert_parsed_expression_evaluates_to<float>("sin(-540)", "0", System, Degree);
  // On R×i
  assert_parsed_expression_evaluates_to<double>("sin(3×𝐢)", "10.01787492741×𝐢", System, Radian);
  assert_parsed_expression_evaluates_to<float>("sin(3×𝐢)", "0.05238381×𝐢", System, Degree);
  // Symmetry: odd
  assert_parsed_expression_evaluates_to<double>("sin(-3×𝐢)", "-10.01787492741×𝐢", System, Radian);
  assert_parsed_expression_evaluates_to<float>("sin(-3×𝐢)", "-0.05238381×𝐢", System, Degree);
  // On: C
  assert_parsed_expression_evaluates_to<float>("sin(𝐢-4)", "1.16781-0.768163×𝐢", System, Radian, Cartesian, 6);
  assert_parsed_expression_evaluates_to<float>("sin(𝐢-4)", "-0.0697671+0.0174117×𝐢", System, Degree, Cartesian, 6);
  assert_parsed_expression_evaluates_to<float>("sin(1.234567890123456ᴇ-15)", "1.23457ᴇ-15", System, Radian, Cartesian, 6);

  /* tan: R  ->  R (tangent-style)
   *      Ri ->  Ri (odd)
   */
  // On R
  assert_parsed_expression_evaluates_to<double>("tan(2)", "-2.1850398632615", System, Radian);
  assert_parsed_expression_evaluates_to<double>("tan(2)", "3.4920769491748ᴇ-2", System, Degree);
  assert_parsed_expression_evaluates_to<double>("tan(2)", "3.1426266043351ᴇ-2", System, Gradian);
  // Tangent-style
  assert_parsed_expression_evaluates_to<float>("tan(π/2)", Undefined::Name(), System, Radian);
  assert_parsed_expression_evaluates_to<double>("tan(3×π/2)", Undefined::Name(), System, Radian);
  assert_parsed_expression_evaluates_to<float>("tan(3×π)", "0", System, Radian);
  assert_parsed_expression_evaluates_to<float>("tan(-540)", "0", System, Degree);
  // On R×i
  assert_parsed_expression_evaluates_to<double>("tan(-2×𝐢)", "-9.6402758007582ᴇ-1×𝐢", System, Radian);
  assert_parsed_expression_evaluates_to<float>("tan(-2×𝐢)", "-0.03489241×𝐢", System, Degree);
  // Symmetry: odd
  assert_parsed_expression_evaluates_to<double>("tan(2×𝐢)", "9.6402758007582ᴇ-1×𝐢", System, Radian);
  assert_parsed_expression_evaluates_to<float>("tan(2×𝐢)", "0.03489241×𝐢", System, Degree);
  // On C
  assert_parsed_expression_evaluates_to<float>("tan(𝐢-4)", "-0.273553+1.00281×𝐢", System, Radian, Cartesian, 6);
  assert_parsed_expression_evaluates_to<float>("tan(𝐢-4)", "-0.0699054+0.0175368×𝐢", System, Degree, Cartesian, 6);

  /* acos: [-1,1]    -> R
   *       ]-inf,-1[ -> π+R×i (odd imaginary)
   *       ]1, inf[  -> R×i (odd imaginary)
   *       R×i       -> π/2+R×i (odd imaginary)
   */
  // On [-1, 1]
  assert_parsed_expression_evaluates_to<double>("acos(0.5)", "1.0471975511966", System, Radian);
  assert_parsed_expression_evaluates_to<double>("acos(0.03)", "1.5407918249714", System, Radian);
  assert_parsed_expression_evaluates_to<double>("acos(0.5)", "60", System, Degree);
  assert_parsed_expression_evaluates_to<double>("acos(0.5)", "66.666666666667", System, Gradian);
  // On [1, inf[
  assert_parsed_expression_evaluates_to<double>("acos(2)", "1.3169578969248×𝐢", System, Radian);
  assert_parsed_expression_evaluates_to<double>("acos(2)", "75.456129290217×𝐢", System, Degree);
  // Symmetry: odd on imaginary
  assert_parsed_expression_evaluates_to<double>("acos(-2)", "3.1415926535898-1.3169578969248×𝐢", System, Radian);
  assert_parsed_expression_evaluates_to<double>("acos(-2)", "180-75.456129290217×𝐢", System, Degree);
  // On ]-inf, -1[
  assert_parsed_expression_evaluates_to<double>("acos(-32)", "3.1415926535898-4.1586388532792×𝐢", System, Radian);
  assert_parsed_expression_evaluates_to<float>("acos(-32)", "180-238.2725×𝐢", System, Degree);
  // On R×i
  assert_parsed_expression_evaluates_to<float>("acos(3×𝐢)", "1.5708-1.8184×𝐢", System, Radian, Cartesian, 5);
  assert_parsed_expression_evaluates_to<float>("acos(3×𝐢)", "90-104.19×𝐢", System, Degree, Cartesian, 5);
  // Symmetry: odd on imaginary
  assert_parsed_expression_evaluates_to<float>("acos(-3×𝐢)", "1.5708+1.8184×𝐢", System, Radian, Cartesian, 5);
  assert_parsed_expression_evaluates_to<float>("acos(-3×𝐢)", "90+104.19×𝐢", System, Degree, Cartesian, 5);
  // On C
  assert_parsed_expression_evaluates_to<float>("acos(𝐢-4)", "2.8894-2.0966×𝐢", System, Radian, Cartesian, 5);
  assert_parsed_expression_evaluates_to<float>("acos(𝐢-4)", "165.551-120.126×𝐢", System, Degree, Cartesian, 6);
  // Key values
  assert_parsed_expression_evaluates_to<double>("acos(0)", "90", System, Degree);
  assert_parsed_expression_evaluates_to<float>("acos(-1)", "180", System, Degree);
  assert_parsed_expression_evaluates_to<double>("acos(1)", "0", System, Degree);
  assert_parsed_expression_evaluates_to<double>("acos(0)", "100", System, Gradian);
  assert_parsed_expression_evaluates_to<float>("acos(-1)", "200", System, Gradian);
  assert_parsed_expression_evaluates_to<double>("acos(1)", "0", System, Gradian);

  /* asin: [-1,1]    -> R
   *       ]-inf,-1[ -> -π/2+R×i (odd)
   *       ]1, inf[  -> π/2+R×i (odd)
   *       R×i       -> R×i (odd)
   */
  // On [-1, 1]
  assert_parsed_expression_evaluates_to<double>("asin(0.5)", "0.5235987755983", System, Radian);
  assert_parsed_expression_evaluates_to<double>("asin(0.03)", "3.0004501823477ᴇ-2", System, Radian);
  assert_parsed_expression_evaluates_to<double>("asin(0.5)", "30", System, Degree);
  // On [1, inf[
  assert_parsed_expression_evaluates_to<double>("asin(2)", "1.5707963267949-1.3169578969248×𝐢", System, Radian);
  assert_parsed_expression_evaluates_to<double>("asin(2)", "90-75.456129290217×𝐢", System, Degree);
  // Symmetry: odd
  assert_parsed_expression_evaluates_to<double>("asin(-2)", "-1.5707963267949+1.3169578969248×𝐢", System, Radian);
  assert_parsed_expression_evaluates_to<double>("asin(-2)", "-90+75.456129290217×𝐢", System, Degree);
  // On ]-inf, -1[
  assert_parsed_expression_evaluates_to<float>("asin(-32)", "-1.571+4.159×𝐢", System, Radian, Cartesian, 4);
  assert_parsed_expression_evaluates_to<float>("asin(-32)", "-90+238×𝐢", System, Degree, Cartesian, 3);
  // On R×i
  assert_parsed_expression_evaluates_to<double>("asin(3×𝐢)", "1.8184464592321×𝐢", System, Radian);
  // Symmetry: odd
  assert_parsed_expression_evaluates_to<double>("asin(-3×𝐢)", "-1.8184464592321×𝐢", System, Radian);
  // On C
  assert_parsed_expression_evaluates_to<float>("asin(𝐢-4)", "-1.3186+2.0966×𝐢", System, Radian, Cartesian, 5);
  assert_parsed_expression_evaluates_to<float>("asin(𝐢-4)", "-75.551+120.13×𝐢", System, Degree, Cartesian, 5);
  // Key values
  assert_parsed_expression_evaluates_to<double>("asin(0)", "0", System, Degree);
  assert_parsed_expression_evaluates_to<float>("asin(-1)", "-90", System, Degree);
  assert_parsed_expression_evaluates_to<double>("asin(1)", "90", System, Degree);

  /* atan: R         ->  R (odd)
   *       [-𝐢,𝐢]    ->  R×𝐢 (odd)
   *       ]-inf×𝐢,-𝐢[ -> -π/2+R×𝐢 (odd)
   *       ]𝐢, inf×𝐢[  -> π/2+R×𝐢 (odd)
   */
  // On R
  assert_parsed_expression_evaluates_to<double>("atan(2)", "1.1071487177941", System, Radian);
  assert_parsed_expression_evaluates_to<double>("atan(0.01)", "9.9996666866652ᴇ-3", System, Radian);
  assert_parsed_expression_evaluates_to<double>("atan(2)", "63.434948822922", System, Degree);
  assert_parsed_expression_evaluates_to<float>("atan(0.5)", "0.4636476", System, Radian);
  // Symmetry: odd
  assert_parsed_expression_evaluates_to<double>("atan(-2)", "-1.1071487177941", System, Radian);
  // On [-𝐢, 𝐢]
  assert_parsed_expression_evaluates_to<float>("atan(0.2×𝐢)", "0.202733×𝐢", System, Radian, Cartesian, 6);
  // Symmetry: odd
  assert_parsed_expression_evaluates_to<float>("atan(-0.2×𝐢)", "-0.202733×𝐢", System, Radian, Cartesian, 6);
  // On [𝐢, inf×𝐢[
  assert_parsed_expression_evaluates_to<double>("atan(26×𝐢)", "1.5707963267949+3.8480520568064ᴇ-2×𝐢", System, Radian);
  assert_parsed_expression_evaluates_to<double>("atan(26×𝐢)", "90+2.2047714220164×𝐢", System, Degree);
  // Symmetry: odd
  assert_parsed_expression_evaluates_to<double>("atan(-26×𝐢)", "-1.5707963267949-3.8480520568064ᴇ-2×𝐢", System, Radian);
  // On ]-inf×𝐢, -𝐢[
  assert_parsed_expression_evaluates_to<float>("atan(-3.4×𝐢)", "-1.570796-0.3030679×𝐢", System, Radian);
  assert_parsed_expression_evaluates_to<float>("atan(-3.4×𝐢)", "-90-17.3645×𝐢", System, Degree, Cartesian, 6);
  // On C
  assert_parsed_expression_evaluates_to<float>("atan(𝐢-4)", "-1.338973+0.05578589×𝐢", System, Radian);
  assert_parsed_expression_evaluates_to<float>("atan(𝐢-4)", "-76.7175+3.1963×𝐢", System, Degree, Cartesian, 6);
  // Key values
  assert_parsed_expression_evaluates_to<float>("atan(0)", "0", System, Degree);
  assert_parsed_expression_evaluates_to<double>("atan(-𝐢)", "-inf×𝐢", System, Radian);
  assert_parsed_expression_evaluates_to<double>("atan(𝐢)", "inf×𝐢", System, Radian);

  /* cosh: R         -> R (even)
   *       R×𝐢       -> R (oscillator)
   */
  // On R
  assert_parsed_expression_evaluates_to<double>("cosh(2)", "3.7621956910836", System, Radian);
  assert_parsed_expression_evaluates_to<double>("cosh(2)", "3.7621956910836", System, Degree);
  // Symmetry: even
  assert_parsed_expression_evaluates_to<double>("cosh(-2)", "3.7621956910836", System, Radian);
  assert_parsed_expression_evaluates_to<double>("cosh(-2)", "3.7621956910836", System, Degree);
  // On R×𝐢
  assert_parsed_expression_evaluates_to<double>("cosh(43×𝐢)", "5.5511330152063ᴇ-1", System, Radian);
  // Oscillator
  assert_parsed_expression_evaluates_to<float>("cosh(π×𝐢/2)", "0", System, Radian);
  assert_parsed_expression_evaluates_to<float>("cosh(5×π×𝐢/2)", "0", System, Radian);
  assert_parsed_expression_evaluates_to<float>("cosh(8×π×𝐢/2)", "1", System, Radian);
  assert_parsed_expression_evaluates_to<float>("cosh(9×π×𝐢/2)", "0", System, Radian);
  // On C
  assert_parsed_expression_evaluates_to<float>("cosh(𝐢-4)", "14.7547-22.9637×𝐢", System, Radian, Cartesian, 6);
  assert_parsed_expression_evaluates_to<float>("cosh(𝐢-4)", "14.7547-22.9637×𝐢", System, Degree, Cartesian, 6);

  /* sinh: R         -> R (odd)
   *       R×𝐢       -> R×𝐢 (oscillator)
   */
  // On R
  assert_parsed_expression_evaluates_to<double>("sinh(2)", "3.626860407847", System, Radian);
  assert_parsed_expression_evaluates_to<double>("sinh(2)", "3.626860407847", System, Degree);
  // Symmetry: odd
  assert_parsed_expression_evaluates_to<double>("sinh(-2)", "-3.626860407847", System, Radian);
  // On R×𝐢
  assert_parsed_expression_evaluates_to<double>("sinh(43×𝐢)", "-0.8317747426286×𝐢", System, Radian);
  // Oscillator
  assert_parsed_expression_evaluates_to<float>("sinh(π×𝐢/2)", "𝐢", System, Radian);
  assert_parsed_expression_evaluates_to<float>("sinh(5×π×𝐢/2)", "𝐢", System, Radian);
  assert_parsed_expression_evaluates_to<float>("sinh(7×π×𝐢/2)", "-𝐢", System, Radian);
  assert_parsed_expression_evaluates_to<float>("sinh(8×π×𝐢/2)", "0", System, Radian);
  assert_parsed_expression_evaluates_to<float>("sinh(9×π×𝐢/2)", "𝐢", System, Radian);
  // On C
  assert_parsed_expression_evaluates_to<float>("sinh(𝐢-4)", "-14.7448+22.9791×𝐢", System, Radian, Cartesian, 6);
  assert_parsed_expression_evaluates_to<float>("sinh(𝐢-4)", "-14.7448+22.9791×𝐢", System, Degree, Cartesian, 6);

  /* tanh: R         -> R (odd)
   *       R×𝐢       -> R×𝐢 (tangent-style)
   */
  // On R
  assert_parsed_expression_evaluates_to<double>("tanh(2)", "9.6402758007582ᴇ-1", System, Radian);
  // Symmetry: odd
  assert_parsed_expression_evaluates_to<double>("tanh(-2)", "-9.6402758007582ᴇ-1", System, Degree);
  // On R×i
  assert_parsed_expression_evaluates_to<double>("tanh(43×𝐢)", "-1.4983873388552×𝐢", System, Radian);
  // Tangent-style
  // FIXME: this depends on the libm implementation and does not work on travis/appveyor servers
  /*assert_parsed_expression_evaluates_to<float>("tanh(π×𝐢/2)", Undefined::Name(), System, Radian);
  assert_parsed_expression_evaluates_to<float>("tanh(5×π×𝐢/2)", Undefined::Name(), System, Radian);
  assert_parsed_expression_evaluates_to<float>("tanh(7×π×𝐢/2)", Undefined::Name(), System, Radian);
  assert_parsed_expression_evaluates_to<float>("tanh(8×π×𝐢/2)", "0", System, Radian);
  assert_parsed_expression_evaluates_to<float>("tanh(9×π×𝐢/2)", Undefined::Name(), System, Radian);*/
  // On C
  assert_parsed_expression_evaluates_to<float>("tanh(𝐢-4)", "-1.00028+0.000610241×𝐢", System, Radian, Cartesian, 6);
  assert_parsed_expression_evaluates_to<float>("tanh(𝐢-4)", "-1.00028+0.000610241×𝐢", System, Degree, Cartesian, 6);

  /* acosh: [-1,1]       -> R×𝐢
   *        ]-inf,-1[    -> π×𝐢+R (even on real)
   *        ]1, inf[     -> R (even on real)
   *        ]-inf×𝐢, 0[  -> -π/2×𝐢+R (even on real)
   *        ]0, inf*𝐢[   -> π/2×𝐢+R (even on real)
   */
  // On [-1,1]
  assert_parsed_expression_evaluates_to<double>("acosh(2)", "1.3169578969248", System, Radian);
  assert_parsed_expression_evaluates_to<double>("acosh(2)", "1.3169578969248", System, Degree);
  // On ]-inf, -1[
  assert_parsed_expression_evaluates_to<double>("acosh(-4)", "2.0634370688956+3.1415926535898×𝐢", System, Radian);
  assert_parsed_expression_evaluates_to<float>("acosh(-4)", "2.06344+3.14159×𝐢", System, Radian, Cartesian, 6);
  // On ]1,inf[: Symmetry: even on real
  assert_parsed_expression_evaluates_to<double>("acosh(4)", "2.0634370688956", System, Radian);
  assert_parsed_expression_evaluates_to<float>("acosh(4)", "2.063437", System, Radian);
  // On ]-inf×𝐢, 0[
  assert_parsed_expression_evaluates_to<double>("acosh(-42×𝐢)", "4.4309584920805-1.5707963267949×𝐢", System, Radian);
  assert_parsed_expression_evaluates_to<float>("acosh(-42×𝐢)", "4.431-1.571×𝐢", System, Radian, Cartesian, 4);
  // On ]0, 𝐢×inf[: Symmetry: even on real
  assert_parsed_expression_evaluates_to<double>("acosh(42×𝐢)", "4.4309584920805+1.5707963267949×𝐢", System, Radian);
  assert_parsed_expression_evaluates_to<float>("acosh(42×𝐢)", "4.431+1.571×𝐢", System, Radian, Cartesian, 4);
  // On C
  assert_parsed_expression_evaluates_to<float>("acosh(𝐢-4)", "2.0966+2.8894×𝐢", System, Radian, Cartesian, 5);
  assert_parsed_expression_evaluates_to<float>("acosh(𝐢-4)", "2.0966+2.8894×𝐢", System, Degree, Cartesian, 5);
  // Key values
  //assert_parsed_expression_evaluates_to<double>("acosh(-1)", "3.1415926535898×𝐢", System, Radian);
  assert_parsed_expression_evaluates_to<double>("acosh(1)", "0", System, Radian);
  assert_parsed_expression_evaluates_to<float>("acosh(0)", "1.570796×𝐢", System, Radian);

  /* asinh: R            -> R (odd)
   *        [-𝐢,𝐢]       -> R*𝐢 (odd)
   *        ]-inf×𝐢,-𝐢[  -> -π/2×𝐢+R (odd)
   *        ]𝐢, inf×𝐢[   -> π/2×𝐢+R (odd)
   */
  // On R
  assert_parsed_expression_evaluates_to<double>("asinh(2)", "1.4436354751788", System, Radian);
  assert_parsed_expression_evaluates_to<double>("asinh(2)", "1.4436354751788", System, Degree);
  // Symmetry: odd
  assert_parsed_expression_evaluates_to<double>("asinh(-2)", "-1.4436354751788", System, Radian);
  assert_parsed_expression_evaluates_to<double>("asinh(-2)", "-1.4436354751788", System, Degree);
  // On [-𝐢,𝐢]
  assert_parsed_expression_evaluates_to<double>("asinh(0.2×𝐢)", "2.0135792079033ᴇ-1×𝐢", System, Radian);
  assert_parsed_expression_evaluates_to<float>("asinh(0.2×𝐢)", "0.2013579×𝐢", System, Degree);
  // Symmetry: odd
  assert_parsed_expression_evaluates_to<double>("asinh(-0.2×𝐢)", "-2.0135792079033ᴇ-1×𝐢", System, Radian);
  assert_parsed_expression_evaluates_to<float>("asinh(-0.2×𝐢)", "-0.2013579×𝐢", System, Degree);
  // On ]-inf×𝐢, -𝐢[
  assert_parsed_expression_evaluates_to<double>("asinh(-22×𝐢)", "-3.7836727043295-1.5707963267949×𝐢", System, Radian);
  assert_parsed_expression_evaluates_to<float>("asinh(-22×𝐢)", "-3.784-1.571×𝐢", System, Degree, Cartesian, 4);
  // On ]𝐢, inf×𝐢[, Symmetry: odd
  assert_parsed_expression_evaluates_to<double>("asinh(22×𝐢)", "3.7836727043295+1.5707963267949×𝐢", System, Radian);
  assert_parsed_expression_evaluates_to<float>("asinh(22×𝐢)", "3.784+1.571×𝐢", System, Degree, Cartesian, 4);
  // On C
  assert_parsed_expression_evaluates_to<float>("asinh(𝐢-4)", "-2.123+0.2383×𝐢", System, Radian, Cartesian, 4);
  assert_parsed_expression_evaluates_to<float>("asinh(𝐢-4)", "-2.123+0.2383×𝐢", System, Degree, Cartesian, 4);

  /* atanh: [-1,1]       -> R (odd)
   *        ]-inf,-1[    -> π/2*𝐢+R (odd)
   *        ]1, inf[     -> -π/2×𝐢+R (odd)
   *        R×𝐢          -> R×𝐢 (odd)
   */
  // On [-1,1]
  assert_parsed_expression_evaluates_to<double>("atanh(0.4)", "0.4236489301936", System, Radian);
  assert_parsed_expression_evaluates_to<double>("atanh(0.4)", "0.4236489301936", System, Degree);
  // Symmetry: odd
  assert_parsed_expression_evaluates_to<double>("atanh(-0.4)", "-0.4236489301936", System, Radian);
  assert_parsed_expression_evaluates_to<double>("atanh(-0.4)", "-0.4236489301936", System, Degree);
  // On ]1, inf[
  assert_parsed_expression_evaluates_to<double>("atanh(4)", "0.255412811883-1.5707963267949×𝐢", System, Radian);
  assert_parsed_expression_evaluates_to<float>("atanh(4)", "0.2554128-1.570796×𝐢", System, Degree);
  // On ]-inf,-1[, Symmetry: odd
  assert_parsed_expression_evaluates_to<double>("atanh(-4)", "-0.255412811883+1.5707963267949×𝐢", System, Radian);
  assert_parsed_expression_evaluates_to<float>("atanh(-4)", "-0.2554128+1.570796×𝐢", System, Degree);
  // On R×𝐢
  assert_parsed_expression_evaluates_to<double>("atanh(4×𝐢)", "1.325817663668×𝐢", System, Radian);
  assert_parsed_expression_evaluates_to<float>("atanh(4×𝐢)", "1.325818×𝐢", System, Radian);
  // Symmetry: odd
  assert_parsed_expression_evaluates_to<double>("atanh(-4×𝐢)", "-1.325817663668×𝐢", System, Radian);
  assert_parsed_expression_evaluates_to<float>("atanh(-4×𝐢)", "-1.325818×𝐢", System, Radian);
  // On C
  assert_parsed_expression_evaluates_to<float>("atanh(𝐢-4)", "-0.238878+1.50862×𝐢", System, Radian, Cartesian, 6);
  assert_parsed_expression_evaluates_to<float>("atanh(𝐢-4)", "-0.238878+1.50862×𝐢", System, Degree, Cartesian, 6);

  // WARNING: evaluate on branch cut can be multivalued
  assert_parsed_expression_evaluates_to<double>("acos(2)", "1.3169578969248×𝐢", System, Radian);
  assert_parsed_expression_evaluates_to<double>("acos(2)", "75.456129290217×𝐢", System, Degree);
  assert_parsed_expression_evaluates_to<double>("asin(2)", "1.5707963267949-1.3169578969248×𝐢", System, Radian);
  assert_parsed_expression_evaluates_to<double>("asin(2)", "90-75.456129290217×𝐢", System, Degree);
  assert_parsed_expression_evaluates_to<double>("atanh(2)", "5.4930614433405ᴇ-1-1.5707963267949×𝐢", System, Radian);
  assert_parsed_expression_evaluates_to<double>("atan(2𝐢)", "1.5707963267949+5.4930614433405ᴇ-1×𝐢", System, Radian);
  assert_parsed_expression_evaluates_to<double>("atan(2𝐢)", "90+31.472923730945×𝐢", System, Degree);
  assert_parsed_expression_evaluates_to<double>("asinh(2𝐢)", "1.3169578969248+1.5707963267949×𝐢", System, Radian);
  assert_parsed_expression_evaluates_to<double>("acosh(-2)", "1.3169578969248+3.1415926535898×𝐢", System, Radian);
}

QUIZ_CASE(poincare_trigo_simplify) {
  // -- sin/cos -> tan
  assert_parsed_expression_simplify_to("sin(x)/cos(x)", "tan(x)");
  assert_parsed_expression_simplify_to("cos(x)/sin(x)", "1/tan(x)");
  assert_parsed_expression_simplify_to("sin(x)×π/cos(x)", "π×tan(x)");
  assert_parsed_expression_simplify_to("sin(x)/(π×cos(x))", "tan(x)/π");
  assert_parsed_expression_simplify_to("1×tan(2)×tan(5)", "tan(2)×tan(5)");
  assert_parsed_expression_simplify_to("tan(62π/21)", "-tan(π/21)");
  assert_parsed_expression_simplify_to("cos(26π/21)/sin(25π/17)", "cos((5×π)/21)/sin((8×π)/17)");
  assert_parsed_expression_simplify_to("cos(62π/21)×π×3/sin(62π/21)", "-(3×π)/tan(π/21)");
  assert_parsed_expression_simplify_to("cos(62π/21)/(π×3×sin(62π/21))", "-1/(3×π×tan(π/21))");
  assert_parsed_expression_simplify_to("sin(62π/21)×π×3/cos(62π/21)", "-3×π×tan(π/21)");
  assert_parsed_expression_simplify_to("sin(62π/21)/(π×3cos(62π/21))", "-tan(π/21)/(3×π)");
  assert_parsed_expression_simplify_to("-cos(π/62)ln(3)/(sin(π/62)π)", "-ln(3)/(π×tan(π/62))");
  assert_parsed_expression_simplify_to("-2cos(π/62)ln(3)/(sin(π/62)π)", "-(2×ln(3))/(π×tan(π/62))");
  // -- cos
  assert_parsed_expression_simplify_to("cos(0)", "1");
  assert_parsed_expression_simplify_to("cos(π)", "-1");
  assert_parsed_expression_simplify_to("cos(π×4/7)", "-cos((3×π)/7)");
  assert_parsed_expression_simplify_to("cos(π×35/29)", "-cos((6×π)/29)");
  assert_parsed_expression_simplify_to("cos(-π×35/29)", "-cos((6×π)/29)");
  assert_parsed_expression_simplify_to("cos(π×340000)", "1");
  assert_parsed_expression_simplify_to("cos(-π×340001)", "-1");
  assert_parsed_expression_simplify_to("cos(-π×√(2))", "cos(√(2)×π)");
  assert_parsed_expression_simplify_to("cos(1311π/6)", "0");
  assert_parsed_expression_simplify_to("cos(π/12)", "(√(6)+√(2))/4");
  assert_parsed_expression_simplify_to("cos(-π/12)", "(√(6)+√(2))/4");
  assert_parsed_expression_simplify_to("cos(-π17/8)", "√(√(2)+2)/2");
  assert_parsed_expression_simplify_to("cos(41π/6)", "-√(3)/2");
  assert_parsed_expression_simplify_to("cos(π/4+1000π)", "√(2)/2");
  assert_parsed_expression_simplify_to("cos(-π/3)", "1/2");
  assert_parsed_expression_simplify_to("cos(41π/5)", "(√(5)+1)/4");
  assert_parsed_expression_simplify_to("cos(7π/10)", "-(√(2)×√(-√(5)+5))/4");
  assert_parsed_expression_simplify_to("cos(0)", "1", User, Degree);
  assert_parsed_expression_simplify_to("cos(180)", "-1", User, Degree);
  assert_parsed_expression_simplify_to("cos(720/7)", "-cos(540/7)", User, Degree);
  assert_parsed_expression_simplify_to("cos(6300/29)", "-cos(1080/29)", User, Degree);
  assert_parsed_expression_simplify_to("cos(-6300/29)", "-cos(1080/29)", User, Degree);
  assert_parsed_expression_simplify_to("cos(61200000)", "1", User, Degree);
  assert_parsed_expression_simplify_to("cos(-61200180)", "-1", User, Degree);
  assert_parsed_expression_simplify_to("cos(-180×√(2))", "cos(180×√(2))", User, Degree);
  assert_parsed_expression_simplify_to("cos(39330)", "0", User, Degree);
  assert_parsed_expression_simplify_to("cos(15)", "(√(6)+√(2))/4", User, Degree);
  assert_parsed_expression_simplify_to("cos(-15)", "(√(6)+√(2))/4", User, Degree);
  assert_parsed_expression_simplify_to("cos(-765/2)", "√(√(2)+2)/2", User, Degree);
  assert_parsed_expression_simplify_to("cos(7380/6)", "-√(3)/2", User, Degree);
  assert_parsed_expression_simplify_to("cos(180045)", "√(2)/2", User, Degree);
  assert_parsed_expression_simplify_to("cos(-60)", "1/2", User, Degree);
  assert_parsed_expression_simplify_to("cos(7380/5)", "(√(5)+1)/4", User, Degree);
  assert_parsed_expression_simplify_to("cos(112.5)", "-√(-√(2)+2)/2", User, Degree);
  // -- sin
  assert_parsed_expression_simplify_to("sin(0)", "0");
  assert_parsed_expression_simplify_to("sin(π)", "0");
  assert_parsed_expression_simplify_to("sin(π×35/29)", "-sin((6×π)/29)");
  assert_parsed_expression_simplify_to("sin(-π×35/29)", "sin((6×π)/29)");
  assert_parsed_expression_simplify_to("sin(π×340000)", "0");
  assert_parsed_expression_simplify_to("sin(π×340001)", "0");
  assert_parsed_expression_simplify_to("sin(-π×340001)", "0");
  assert_parsed_expression_simplify_to("sin(π/12)", "(√(6)-√(2))/4");
  assert_parsed_expression_simplify_to("sin(-π/12)", "(-√(6)+√(2))/4");
  assert_parsed_expression_simplify_to("sin(-π×√(2))", "-sin(√(2)×π)");
  assert_parsed_expression_simplify_to("sin(1311π/6)", "1");
  assert_parsed_expression_simplify_to("sin(-π17/8)", "-√(-√(2)+2)/2");
  assert_parsed_expression_simplify_to("sin(41π/6)", "1/2");
  assert_parsed_expression_simplify_to("sin(-3π/10)", "(-√(5)-1)/4");
  assert_parsed_expression_simplify_to("sin(π/4+1000π)", "√(2)/2");
  assert_parsed_expression_simplify_to("sin(-π/3)", "-√(3)/2");
  assert_parsed_expression_simplify_to("sin(17π/5)", "-(√(2)×√(√(5)+5))/4");
  assert_parsed_expression_simplify_to("sin(π/5)", "(√(2)×√(-√(5)+5))/4");
  assert_parsed_expression_simplify_to("sin(0)", "0", User, Degree);
  assert_parsed_expression_simplify_to("sin(180)", "0", User, Degree);
  assert_parsed_expression_simplify_to("sin(6300/29)", "-sin(1080/29)", User, Degree);
  assert_parsed_expression_simplify_to("sin(-6300/29)", "sin(1080/29)", User, Degree);
  assert_parsed_expression_simplify_to("sin(61200000)", "0", User, Degree);
  assert_parsed_expression_simplify_to("sin(61200180)", "0", User, Degree);
  assert_parsed_expression_simplify_to("sin(-61200180)", "0", User, Degree);
  assert_parsed_expression_simplify_to("sin(15)", "(√(6)-√(2))/4", User, Degree);
  assert_parsed_expression_simplify_to("sin(-15)", "(-√(6)+√(2))/4", User, Degree);
  assert_parsed_expression_simplify_to("sin(-180×√(2))", "-sin(180×√(2))", User, Degree);
  assert_parsed_expression_simplify_to("sin(39330)", "1", User, Degree);
  assert_parsed_expression_simplify_to("sin(-765/2)", "-√(-√(2)+2)/2", User, Degree);
  assert_parsed_expression_simplify_to("sin(1230)", "1/2", User, Degree);
  assert_parsed_expression_simplify_to("sin(180045)", "√(2)/2", User, Degree);
  assert_parsed_expression_simplify_to("sin(-60)", "-√(3)/2", User, Degree);
  assert_parsed_expression_simplify_to("sin(612)", "-(√(2)×√(√(5)+5))/4", User, Degree);
  assert_parsed_expression_simplify_to("sin(36)", "(√(2)×√(-√(5)+5))/4", User, Degree);
  // -- tan
  assert_parsed_expression_simplify_to("tan(0)", "0");
  assert_parsed_expression_simplify_to("tan(π)", "0");
  assert_parsed_expression_simplify_to("tan(π×35/29)", "tan((6×π)/29)");
  assert_parsed_expression_simplify_to("tan(-π×35/29)", "-tan((6×π)/29)");
  assert_parsed_expression_simplify_to("tan(π×340000)", "0");
  assert_parsed_expression_simplify_to("tan(π×340001)", "0");
  assert_parsed_expression_simplify_to("tan(-π×340001)", "0");
  assert_parsed_expression_simplify_to("tan(π/12)", "-√(3)+2");
  assert_parsed_expression_simplify_to("tan(-π/12)", "√(3)-2");
  assert_parsed_expression_simplify_to("tan(-π×√(2))", "-tan(√(2)×π)");
  assert_parsed_expression_simplify_to("tan(1311π/6)", Undefined::Name());
  assert_parsed_expression_simplify_to("tan(-π17/8)", "-√(2)+1");
  assert_parsed_expression_simplify_to("tan(41π/6)", "-√(3)/3");
  assert_parsed_expression_simplify_to("tan(π/4+1000π)", "1");
  assert_parsed_expression_simplify_to("tan(-π/3)", "-√(3)");
  assert_parsed_expression_simplify_to("tan(-π/10)", "-(√(5)×√(-2×√(5)+5))/5");
  assert_parsed_expression_simplify_to("tan(0)", "0", User, Degree);
  assert_parsed_expression_simplify_to("tan(180)", "0", User, Degree);
  assert_parsed_expression_simplify_to("tan(6300/29)", "tan(1080/29)", User, Degree);
  assert_parsed_expression_simplify_to("tan(-6300/29)", "-tan(1080/29)", User, Degree);
  assert_parsed_expression_simplify_to("tan(61200000)", "0", User, Degree);
  assert_parsed_expression_simplify_to("tan(61200180)", "0", User, Degree);
  assert_parsed_expression_simplify_to("tan(-61200180)", "0", User, Degree);
  assert_parsed_expression_simplify_to("tan(15)", "-√(3)+2", User, Degree);
  assert_parsed_expression_simplify_to("tan(-15)", "√(3)-2", User, Degree);
  assert_parsed_expression_simplify_to("tan(-180×√(2))", "-tan(180×√(2))", User, Degree);
  assert_parsed_expression_simplify_to("tan(39330)", Undefined::Name(), User, Degree);
  assert_parsed_expression_simplify_to("tan(-382.5)", "-√(2)+1", User, Degree);
  assert_parsed_expression_simplify_to("tan(1230)", "-√(3)/3", User, Degree);
  assert_parsed_expression_simplify_to("tan(180045)", "1", User, Degree);
  assert_parsed_expression_simplify_to("tan(-60)", "-√(3)", User, Degree);
  assert_parsed_expression_simplify_to("tan(tan(tan(tan(9))))", "tan(tan(tan(tan(9))))");
  // -- acos
  assert_parsed_expression_simplify_to("acos(-1/2)", "(2×π)/3");
  assert_parsed_expression_simplify_to("acos(-1.2)", "-acos(6/5)+π");
  assert_parsed_expression_simplify_to("acos(cos(2/3))", "2/3");
  assert_parsed_expression_simplify_to("acos(cos(3/2))", "3/2");
  assert_parsed_expression_simplify_to("cos(acos(3/2))", "3/2");
  assert_parsed_expression_simplify_to("cos(acos(2/3))", "2/3");
  assert_parsed_expression_simplify_to("acos(cos(12))", "acos(cos(12))");
  assert_parsed_expression_simplify_to("acos(cos(4π/7))", "(4×π)/7");
  assert_parsed_expression_simplify_to("acos(-cos(2))", "π-2");
  assert_parsed_expression_simplify_to("acos(-1/2)", "120", User, Degree);
  assert_parsed_expression_simplify_to("acos(-1.2)", "-acos(6/5)+180", User, Degree);
  assert_parsed_expression_simplify_to("acos(cos(2/3))", "2/3", User, Degree);
  assert_parsed_expression_simplify_to("acos(cos(190))", "170", User, Degree);
  assert_parsed_expression_simplify_to("acos(cos(75))", "75", User, Degree);
  assert_parsed_expression_simplify_to("cos(acos(190))", "190", User, Degree);
  assert_parsed_expression_simplify_to("cos(acos(75))", "75", User, Degree);
  assert_parsed_expression_simplify_to("acos(cos(12))", "12", User, Degree);
  assert_parsed_expression_simplify_to("acos(cos(720/7))", "720/7", User, Degree);
  // -- asin
  assert_parsed_expression_simplify_to("asin(-1/2)", "-π/6");
  assert_parsed_expression_simplify_to("asin(-1.2)", "-asin(6/5)");
  assert_parsed_expression_simplify_to("asin(sin(2/3))", "2/3");
  assert_parsed_expression_simplify_to("sin(asin(2/3))", "2/3");
  assert_parsed_expression_simplify_to("sin(asin(3/2))", "3/2");
  assert_parsed_expression_simplify_to("asin(sin(3/2))", "3/2");
  assert_parsed_expression_simplify_to("asin(sin(12))", "asin(sin(12))");
  assert_parsed_expression_simplify_to("asin(sin(-π/7))", "-π/7");
  assert_parsed_expression_simplify_to("asin(sin(-√(2)))", "-√(2)");
  assert_parsed_expression_simplify_to("asin(-1/2)", "-30", User, Degree);
  assert_parsed_expression_simplify_to("asin(-1.2)", "-asin(6/5)", User, Degree);
  assert_parsed_expression_simplify_to("asin(sin(75))", "75", User, Degree);
  assert_parsed_expression_simplify_to("sin(asin(75))", "75", User, Degree);
  assert_parsed_expression_simplify_to("sin(asin(190))", "190", User, Degree);
  assert_parsed_expression_simplify_to("asin(sin(32))", "32", User, Degree);
  assert_parsed_expression_simplify_to("asin(sin(400))", "40", User, Degree);
  assert_parsed_expression_simplify_to("asin(sin(-180/7))", "-180/7", User, Degree);
  // -- atan
  assert_parsed_expression_simplify_to("atan(-1)", "-π/4");
  assert_parsed_expression_simplify_to("atan(-1.2)", "-atan(6/5)");
  assert_parsed_expression_simplify_to("atan(tan(2/3))", "2/3");
  assert_parsed_expression_simplify_to("tan(atan(2/3))", "2/3");
  assert_parsed_expression_simplify_to("tan(atan(5/2))", "5/2");
  assert_parsed_expression_simplify_to("atan(tan(5/2))", "atan(tan(5/2))");
  assert_parsed_expression_simplify_to("atan(tan(5/2))", "atan(tan(5/2))");
  assert_parsed_expression_simplify_to("atan(tan(-π/7))", "-π/7");
  assert_parsed_expression_simplify_to("atan(√(3))", "π/3");
  assert_parsed_expression_simplify_to("atan(tan(-√(2)))", "-√(2)");
  assert_parsed_expression_simplify_to("atan(-1)", "-45", User, Degree);
  assert_parsed_expression_simplify_to("atan(-1.2)", "-atan(6/5)", User, Degree);
  assert_parsed_expression_simplify_to("atan(tan(-45))", "-45", User, Degree);
  assert_parsed_expression_simplify_to("tan(atan(120))", "120", User, Degree);
  assert_parsed_expression_simplify_to("tan(atan(2293))", "2293", User, Degree);
  assert_parsed_expression_simplify_to("atan(tan(2293))", "-47", User, Degree);
  assert_parsed_expression_simplify_to("atan(tan(1808))", "8", User, Degree);
  assert_parsed_expression_simplify_to("atan(tan(-180/7))", "-180/7", User, Degree);
  assert_parsed_expression_simplify_to("atan(√(3))", "60", User, Degree);
  assert_parsed_expression_simplify_to("atan(1/x)", "(π×sign(x)-2×atan(x))/2", User, Degree);

  // cos(asin)
  assert_parsed_expression_simplify_to("cos(asin(x))", "√(-x^2+1)", User, Degree);
  assert_parsed_expression_simplify_to("cos(asin(-x))", "√(-x^2+1)", User, Degree);
  // cos(atan)
  assert_parsed_expression_simplify_to("cos(atan(x))", "1/√(x^2+1)", User, Degree);
  assert_parsed_expression_simplify_to("cos(atan(-x))", "1/√(x^2+1)", User, Degree);
  // sin(acos)
  assert_parsed_expression_simplify_to("sin(acos(x))", "√(-x^2+1)", User, Degree);
  assert_parsed_expression_simplify_to("sin(acos(-x))", "√(-x^2+1)", User, Degree);
  // sin(atan)
  assert_parsed_expression_simplify_to("sin(atan(x))", "x/√(x^2+1)", User, Degree);
  assert_parsed_expression_simplify_to("sin(atan(-x))", "-x/√(x^2+1)", User, Degree);
  // tan(acos)
  assert_parsed_expression_simplify_to("tan(acos(x))", "√(-x^2+1)/x", User, Degree);
  assert_parsed_expression_simplify_to("tan(acos(-x))", "-√(-x^2+1)/x", User, Degree);
  // tan(asin)
  assert_parsed_expression_simplify_to("tan(asin(x))", "x/√(-x^2+1)", User, Degree);
  assert_parsed_expression_simplify_to("tan(asin(-x))", "-x/√(-x^2+1)", User, Degree);
}
