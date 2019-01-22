#include <quiz.h>
#include <ion.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_multiplication_evaluate) {
  assert_parsed_expression_evaluates_to<float>("1×2", "2");
  assert_parsed_expression_evaluates_to<double>("(3+𝐢)×(4+𝐢)", "11+7×𝐢");

#if MATRICES_ARE_DEFINED
  assert_parsed_expression_evaluates_to<float>("[[1,2][3,4][5,6]]×2", "[[2,4][6,8][10,12]]");
  assert_parsed_expression_evaluates_to<double>("[[1,2+𝐢][3,4][5,6]]×(3+𝐢)", "[[3+𝐢,5+5×𝐢][9+3×𝐢,12+4×𝐢][15+5×𝐢,18+6×𝐢]]");
  assert_parsed_expression_evaluates_to<float>("2×[[1,2][3,4][5,6]]", "[[2,4][6,8][10,12]]");
  assert_parsed_expression_evaluates_to<double>("(3+𝐢)×[[1,2+𝐢][3,4][5,6]]", "[[3+𝐢,5+5×𝐢][9+3×𝐢,12+4×𝐢][15+5×𝐢,18+6×𝐢]]");
  assert_parsed_expression_evaluates_to<float>("[[1,2][3,4][5,6]]×[[1,2,3,4][5,6,7,8]]", "[[11,14,17,20][23,30,37,44][35,46,57,68]]");
  assert_parsed_expression_evaluates_to<double>("[[1,2+𝐢][3,4][5,6]]×[[1,2+𝐢,3,4][5,6+𝐢,7,8]]", "[[11+5×𝐢,13+9×𝐢,17+7×𝐢,20+8×𝐢][23,30+7×𝐢,37,44][35,46+11×𝐢,57,68]]");
#endif
}

QUIZ_CASE(poincare_multiplication_simplify) {
  assert_parsed_expression_simplify_to("undef×x", "undef");
  assert_parsed_expression_simplify_to("0×x+B", "B");
  assert_parsed_expression_simplify_to("0×x×0×32×cos(3)", "0");
  assert_parsed_expression_simplify_to("3×A^4×B^x×B^2×(A^2+2)×2×1.2", "(36×A^6×B^(x+2)+72×A^4×B^(x+2))/5");
  assert_parsed_expression_simplify_to("A×(B+C)×(D+3)", "3×A×B+3×A×C+A×B×D+A×C×D");
  assert_parsed_expression_simplify_to("A/B", "A/B");
  assert_parsed_expression_simplify_to("(A×B)^2", "A^2×B^2");
  assert_parsed_expression_simplify_to("(1/2)×A/B", "A/(2×B)");
  assert_parsed_expression_simplify_to("1+2+3+4+5+6", "21");
  assert_parsed_expression_simplify_to("1-2+3-4+5-6", "-3");
  assert_parsed_expression_simplify_to("987654321123456789×998877665544332211", "986545842648570754445552922919330479");
  assert_parsed_expression_simplify_to("2/3", "2/3");
  assert_parsed_expression_simplify_to("9/17+5/4", "121/68");
  assert_parsed_expression_simplify_to("1/2×3/4", "3/8");
  assert_parsed_expression_simplify_to("0×2/3", "0");
  assert_parsed_expression_simplify_to("1+(1/(1+1/(1+1/(1+1))))", "8/5");
  assert_parsed_expression_simplify_to("1+2/(3+4/(5+6/(7+8)))", "155/101");
  assert_parsed_expression_simplify_to("3/4×16/12", "1");
  assert_parsed_expression_simplify_to("3/4×(8+8)/12", "1");
  assert_parsed_expression_simplify_to("916791/794976477", "305597/264992159");
  assert_parsed_expression_simplify_to("321654987123456789/112233445566778899", "3249040273974311/1133671167341201");
  assert_parsed_expression_simplify_to("0.1+0.2", "3/10");
  assert_parsed_expression_simplify_to("2^3", "8");
  assert_parsed_expression_simplify_to("(-1)×(-1)", "1");
  assert_parsed_expression_simplify_to("(-2)^2", "4");
  assert_parsed_expression_simplify_to("(-3)^3", "-27");
  assert_parsed_expression_simplify_to("(1/2)^-1", "2");
  assert_parsed_expression_simplify_to("√(2)×√(3)", "√(6)");
  assert_parsed_expression_simplify_to("2×2^π", "2×2^π");
  assert_parsed_expression_simplify_to("A^3×B×A^(-3)", "B");
  assert_parsed_expression_simplify_to("A^3×A^(-3)", "1");
  assert_parsed_expression_simplify_to("2^π×(1/2)^π", "1");
  assert_parsed_expression_simplify_to("A^3×A^(-3)", "1");
  assert_parsed_expression_simplify_to("(x+1)×(x+2)", "x^2+3×x+2");
  assert_parsed_expression_simplify_to("(x+1)×(x-1)", "x^2-1");
  assert_parsed_expression_simplify_to("11π/(22π+11π)", "1/3");
  assert_parsed_expression_simplify_to("11/(22π+11π)", "1/(3×π)");
  assert_parsed_expression_simplify_to("-11/(22π+11π)", "-1/(3×π)");
  assert_parsed_expression_simplify_to("A^2×B×A^(-2)×B^(-2)", "1/B");
  assert_parsed_expression_simplify_to("A^(-1)×B^(-1)", "1/(A×B)");
  assert_parsed_expression_simplify_to("x+x", "2×x");
  assert_parsed_expression_simplify_to("2×x+x", "3×x");
  assert_parsed_expression_simplify_to("x×2+x", "3×x");
  assert_parsed_expression_simplify_to("2×x+2×x", "4×x");
  assert_parsed_expression_simplify_to("x×2+2×n", "2×n+2×x");
  assert_parsed_expression_simplify_to("x+x+n+n", "2×n+2×x");
  assert_parsed_expression_simplify_to("x-x-n+n", "0");
  assert_parsed_expression_simplify_to("x+n-x-n", "0");
  assert_parsed_expression_simplify_to("x-x", "0");
  assert_parsed_expression_simplify_to("π×3^(1/2)×(5π)^(1/2)×(4/5)^(1/2)", "2×√(3)×π^(3/2)");
  assert_parsed_expression_simplify_to("12^(1/4)×(π/6)×(12×π)^(1/4)", "(√(3)×π^(5/4))/3");
}
