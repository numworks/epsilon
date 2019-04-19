#include <quiz.h>
#include <ion.h>
#include <assert.h>
#include <cmath>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_power_evaluate) {
  assert_parsed_expression_evaluates_to<float>("2^3", "8");
  assert_parsed_expression_evaluates_to<double>("(3+𝐢)^4", "28+96×𝐢");
  assert_parsed_expression_evaluates_to<float>("4^(3+𝐢)", "11.74125+62.91378×𝐢");
  assert_parsed_expression_evaluates_to<double>("(3+𝐢)^(3+𝐢)", "-11.898191759852+19.592921596609×𝐢");

  assert_parsed_expression_evaluates_to<double>("0^0", Undefined::Name());
  assert_parsed_expression_evaluates_to<double>("0^2", "0");
  assert_parsed_expression_evaluates_to<double>("0^(-2)", Undefined::Name());

  assert_parsed_expression_evaluates_to<double>("(-2)^4.2", "14.8690638497+10.8030072384×𝐢", System, Radian, Cartesian, 12);
  assert_parsed_expression_evaluates_to<double>("(-0.1)^4", "0.0001", System, Radian, Cartesian, 12);

#if MATRICES_ARE_DEFINED
  assert_parsed_expression_evaluates_to<float>("[[1,2][3,4]]^(-3)", "[[-14.75,6.75][10.125,-4.625]]", System, Degree, Cartesian, 6);
  assert_parsed_expression_evaluates_to<double>("[[1,2][3,4]]^3", "[[37,54][81,118]]");
#endif
  assert_parsed_expression_evaluates_to<float>("0^2", "0");
  assert_parsed_expression_evaluates_to<double>("𝐢^𝐢", "2.0787957635076ᴇ-1");
  assert_parsed_expression_evaluates_to<float>("1.0066666666667^60", "1.48985", System, Radian, Cartesian, 6);
  assert_parsed_expression_evaluates_to<double>("1.0066666666667^60", "1.4898457083046");
  assert_parsed_expression_evaluates_to<float>("ℯ^(𝐢×π)", "-1");
  assert_parsed_expression_evaluates_to<double>("ℯ^(𝐢×π)", "-1");
  assert_parsed_expression_evaluates_to<float>("ℯ^(𝐢×π+2)", "-7.38906", System, Radian, Cartesian, 6);
  assert_parsed_expression_evaluates_to<double>("ℯ^(𝐢×π+2)", "-7.3890560989307");
  assert_parsed_expression_evaluates_to<float>("(-1)^(1/3)", "0.5+0.8660254×𝐢");
  assert_parsed_expression_evaluates_to<double>("(-1)^(1/3)", "0.5+8.6602540378444ᴇ-1×𝐢");
  assert_parsed_expression_evaluates_to<float>("ℯ^(𝐢×π/3)", "0.5+0.8660254×𝐢");
  assert_parsed_expression_evaluates_to<double>("ℯ^(𝐢×π/3)", "0.5+8.6602540378444ᴇ-1×𝐢");
  assert_parsed_expression_evaluates_to<float>("𝐢^(2/3)", "0.5+0.8660254×𝐢");
  assert_parsed_expression_evaluates_to<double>("𝐢^(2/3)", "0.5+8.6602540378444ᴇ-1×𝐢");
}

QUIZ_CASE(poincare_power_simplify) {
  assert_parsed_expression_simplify_to("3^4", "81");
  assert_parsed_expression_simplify_to("3^(-4)", "1/81");
  assert_parsed_expression_simplify_to("(-3)^3", "-27");
  assert_parsed_expression_simplify_to("1256^(1/3)×x", "2×root(157,3)×x");
  assert_parsed_expression_simplify_to("1256^(-1/3)", "1/(2×root(157,3))");
  assert_parsed_expression_simplify_to("32^(-1/5)", "1/2");
  assert_parsed_expression_simplify_to("(2+3-4)^(x)", "1");
  assert_parsed_expression_simplify_to("1^x", "1");
  assert_parsed_expression_simplify_to("x^1", "x");
  assert_parsed_expression_simplify_to("0^3", "0");
  assert_parsed_expression_simplify_to("0^0", Undefined::Name());
  assert_parsed_expression_simplify_to("0^(-3)", Undefined::Name());
  assert_parsed_expression_simplify_to("4^0.5", "2");
  assert_parsed_expression_simplify_to("8^0.5", "2×√(2)");
  assert_parsed_expression_simplify_to("(12^4×3)^(0.5)", "144×√(3)");
  assert_parsed_expression_simplify_to("(π^3)^4", "π^12");
  assert_parsed_expression_simplify_to("(A×B)^3", "A^3×B^3");
  assert_parsed_expression_simplify_to("(12^4×x)^(0.5)", "144×√(x)");
  assert_parsed_expression_simplify_to("√(32)", "4×√(2)");
  assert_parsed_expression_simplify_to("√(-1)", "𝐢");
  assert_parsed_expression_simplify_to("√(-1×√(-1))", "√(2)/2-√(2)/2×𝐢");
  assert_parsed_expression_simplify_to("√(3^2)", "3");
  assert_parsed_expression_simplify_to("2^(2+π)", "4×2^π");
  assert_parsed_expression_simplify_to("√(5513219850886344455940081)", "2348024669991");
  assert_parsed_expression_simplify_to("√(154355776)", "12424");
  assert_parsed_expression_simplify_to("√(π)^2", "π");
  assert_parsed_expression_simplify_to("√(π^2)", "π");
  assert_parsed_expression_simplify_to("√((-π)^2)", "π");
  assert_parsed_expression_simplify_to("√(x×144)", "12×√(x)");
  assert_parsed_expression_simplify_to("√(x×144×π^2)", "12×π×√(x)");
  assert_parsed_expression_simplify_to("√(x×144×π)", "12×√(π)×√(x)");
  assert_parsed_expression_simplify_to("(-1)×(2+(-4×√(2)))", "4×√(2)-2");
  assert_parsed_expression_simplify_to("x^(1/2)", "√(x)");
  assert_parsed_expression_simplify_to("x^(-1/2)", "1/√(x)");
  assert_parsed_expression_simplify_to("x^(1/7)", "root(x,7)");
  assert_parsed_expression_simplify_to("x^(-1/7)", "1/root(x,7)");
  assert_parsed_expression_simplify_to("1/(3√(2))", "√(2)/6");
  assert_parsed_expression_simplify_to("ℯ^ln(3)", "3");
  assert_parsed_expression_simplify_to("ℯ^ln(√(3))", "√(3)");
  assert_parsed_expression_simplify_to("π^log(√(3),π)", "√(3)");
  assert_parsed_expression_simplify_to("10^log(π)", "π");
  assert_parsed_expression_simplify_to("ℯ^ln(65)", "65");
  assert_parsed_expression_simplify_to("ℯ^ln(πℯ)", "π×ℯ");
  assert_parsed_expression_simplify_to("ℯ^log(πℯ)", "ℯ^(log(ℯ)+log(π))");
  assert_parsed_expression_simplify_to("√(ℯ^2)", "ℯ");
  assert_parsed_expression_simplify_to("999^(10000/3)", "999^(10000/3)");
  /* This does not reduce but should not as the integer is above
   * k_maxNumberOfPrimeFactors and thus it prime decomposition might overflow
   * 32 factors. */
  assert_parsed_expression_simplify_to("1881676377434183981909562699940347954480361860897069^(1/3)", "root(1881676377434183981909562699940347954480361860897069,3)");
  /* This does not reduce but should not as the prime decomposition involves
   * factors above k_maxNumberOfPrimeFactors. */
  assert_parsed_expression_simplify_to("1002101470343^(1/3)", "root(1002101470343,3)");
  assert_parsed_expression_simplify_to("π×π×π", "π^3");
  assert_parsed_expression_simplify_to("(x+π)^(3)", "x^3+3×π×x^2+3×π^2×x+π^3");
  assert_parsed_expression_simplify_to("(5+√(2))^(-8)", "(-1003320×√(2)+1446241)/78310985281");
  assert_parsed_expression_simplify_to("(5×π+√(2))^(-5)", "1/(3125×π^5+3125×√(2)×π^4+2500×π^3+500×√(2)×π^2+100×π+4×√(2))");
  assert_parsed_expression_simplify_to("(1+√(2)+√(3))^5", "120×√(6)+184×√(3)+224×√(2)+296");
  assert_parsed_expression_simplify_to("(π+√(2)+√(3)+x)^(-3)", "1/(x^3+3×π×x^2+3×√(3)×x^2+3×√(2)×x^2+3×π^2×x+6×√(3)×π×x+6×√(2)×π×x+6×√(6)×x+15×x+π^3+3×√(3)×π^2+3×√(2)×π^2+6×√(6)×π+15×π+9×√(3)+11×√(2))");
  assert_parsed_expression_simplify_to("1.0066666666667^60", "(10066666666667/10000000000000)^60");
  assert_parsed_expression_simplify_to("2^(6+π+x)", "64×2^(x+π)");
  assert_parsed_expression_simplify_to("𝐢^(2/3)", "1/2+√(3)/2×𝐢");
  assert_parsed_expression_simplify_to("ℯ^(𝐢×π/3)", "1/2+√(3)/2×𝐢");
  assert_parsed_expression_simplify_to("(-1)^(1/3)", "1/2+√(3)/2×𝐢");
  assert_parsed_expression_simplify_to("R(-x)", "R(-x)");
  assert_parsed_expression_simplify_to("√(x)^2", "x", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("√(-3)^2", "unreal", User, Radian, Real);
  assert_parsed_expression_simplify_to("1+((8+√(6))^(1/2))^-1+(8+√(6))^(1/2)", "(√(√(6)+8)+√(6)+9)/√(√(6)+8)", User, Radian, Real);
}
