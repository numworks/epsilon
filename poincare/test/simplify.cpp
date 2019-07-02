#include <quiz.h>
#include <ion.h>
#include <assert.h>
#include "helper.h"
#if POINCARE_TESTS_PRINT_EXPRESSIONS
#include "../src/expression_debug.h"
#include <iostream>
using namespace std;
#endif

using namespace Poincare;
QUIZ_CASE(poincare_reduction_target) {
  assert_parsed_expression_simplify_to("1/π+1/x", "1/x+1/π", System);
  assert_parsed_expression_simplify_to("1/π+1/x", "(x+π)/(π×x)", User);

  assert_parsed_expression_simplify_to("1/(1+𝐢)", "1/(𝐢+1)", System);
  assert_parsed_expression_simplify_to("1/(1+𝐢)", "1/2-1/2×𝐢", User);

  assert_parsed_expression_simplify_to("sin(x)/(cos(x)×cos(x))", "sin(x)/cos(x)^2", System);
  assert_parsed_expression_simplify_to("sin(x)/(cos(x)×cos(x))", "tan(x)/cos(x)", User);

  assert_parsed_expression_simplify_to("x^0", "x^0", System);
  assert_parsed_expression_simplify_to("x^0", "1", User);

  assert_parsed_expression_simplify_to("x^(2/3)", "root(x,3)^2", System);
  assert_parsed_expression_simplify_to("x^(2/3)", "x^(2/3)", User);
  assert_parsed_expression_simplify_to("x^(1/3)", "root(x,3)", System);
  assert_parsed_expression_simplify_to("x^(1/3)", "root(x,3)", System);
  assert_parsed_expression_simplify_to("x^2", "x^2", System);
  assert_parsed_expression_simplify_to("x^2", "x^2", User);

  assert_parsed_expression_simplify_to("1/(√(2)+√(3))", "1/(√(3)+√(2))", System);
  assert_parsed_expression_simplify_to("1/(√(2)+√(3))", "√(3)-√(2)", User);

  assert_parsed_expression_simplify_to("sign(abs(x))", "sign(abs(x))", System);
  assert_parsed_expression_simplify_to("sign(abs(x))", "1", User);

  assert_parsed_expression_simplify_to("atan(1/x)", "atan(1/x)", System);
  assert_parsed_expression_simplify_to("atan(1/x)", "(π×sign(x)-2×atan(x))/2", User);

  assert_parsed_expression_simplify_to("(1+x)/(1+x)", "(x+1)^0", System);
  assert_parsed_expression_simplify_to("(1+x)/(1+x)", "1", User);
}

QUIZ_CASE(poincare_simplify_mix) {
  // Root at denominator
  assert_parsed_expression_simplify_to("1/(√(2)+√(3))", "√(3)-√(2)");
  assert_parsed_expression_simplify_to("1/(5+√(3))", "(-√(3)+5)/22");
  assert_parsed_expression_simplify_to("1/(√(2)+4)", "(-√(2)+4)/14");
  assert_parsed_expression_simplify_to("1/(2√(2)-4)", "(-√(2)-2)/4");
  assert_parsed_expression_simplify_to("1/(-2√(2)+4)", "(√(2)+2)/4");
  assert_parsed_expression_simplify_to("45^2", "2025");
  assert_parsed_expression_simplify_to("1/(√(2)ln(3))", "√(2)/(2×ln(3))");
  assert_parsed_expression_simplify_to("√(3/2)", "√(6)/2");

  // Common operation mix
  assert_parsed_expression_simplify_to("(√(2)×π + √(2)×ℯ)/√(2)", "ℯ+π");
  assert_parsed_expression_simplify_to("π+(3√(2)-2√(3))/25", "(25×π-2×√(3)+3×√(2))/25");
  assert_parsed_expression_simplify_to("ln(2+3)", "ln(5)");
  assert_parsed_expression_simplify_to("3×A×B×C+4×cos(2)-2×A×B×C+A×B×C+ln(3)+4×A×B-5×A×B×C+cos(3)×ln(5)+cos(2)-45×cos(2)", "cos(3)×ln(5)+ln(3)-40×cos(2)+4×A×B-3×A×B×C");
  assert_parsed_expression_simplify_to("2×A+3×cos(2)+3+4×ln(5)+5×A+2×ln(5)+1+0", "6×ln(5)+3×cos(2)+7×A+4");
  assert_parsed_expression_simplify_to("2.3×A+3×cos(2)+3+4×ln(5)+5×A+2×ln(5)+1.2+0.235", "(1200×ln(5)+600×cos(2)+1460×A+887)/200");
  assert_parsed_expression_simplify_to("2×A×B×C+2.3×A×B+3×A^2+5.2×A×B×C+4×A^2", "(70×A^2+23×A×B+72×A×B×C)/10");
  assert_parsed_expression_simplify_to("(A×B)^2×A+4×A^3", "4×A^3+A^3×B^2");
  assert_parsed_expression_simplify_to("(A×3)^2×A+4×A^3", "13×A^3");
  assert_parsed_expression_simplify_to("A^2^2×A+4×A^3", "A^5+4×A^3");
  assert_parsed_expression_simplify_to("0.5+4×A×B-2.3+2×A×B-2×B×A^C-cos(4)+2×A^C×B+A×B×C×D", "(-5×cos(4)+30×A×B+5×A×B×C×D-9)/5");
  assert_parsed_expression_simplify_to("(1+√(2))/5", "(√(2)+1)/5");
  assert_parsed_expression_simplify_to("(2+√(6))^2", "4×√(6)+10");
  assert_parsed_expression_simplify_to("tan(3)ln(2)+π", "tan(3)×ln(2)+π");

  // Complex
  assert_parsed_expression_simplify_to("𝐢", "𝐢");
  assert_parsed_expression_simplify_to("√(-33)", "√(33)×𝐢");
  assert_parsed_expression_simplify_to("𝐢^(3/5)", "(√(2)×√(-√(5)+5))/4+(√(5)+1)/4×𝐢");
  assert_parsed_expression_simplify_to("𝐢𝐢𝐢𝐢", "1");
  assert_parsed_expression_simplify_to("√(-𝐢)", "√(2)/2-√(2)/2×𝐢");
  assert_parsed_expression_simplify_to("A×cos(9)𝐢𝐢ln(2)", "-A×cos(9)×ln(2)");
  assert_parsed_expression_simplify_to("(√(2)+√(2)×𝐢)/2(√(2)+√(2)×𝐢)/2(√(2)+√(2)×𝐢)/2", "√(2)/32-√(2)/32×𝐢");
  assert_parsed_expression_simplify_to("root(5^((-𝐢)3^9),𝐢)", "1/ℯ^atan(tan(19683×ln(5)))");
  assert_parsed_expression_simplify_to("𝐢^𝐢", "1/ℯ^(π/2)");
  assert_parsed_expression_simplify_to("𝐢/(1+𝐢×√(x))", "𝐢/(√(x)×𝐢+1)");
  assert_parsed_expression_simplify_to("x+𝐢/(1+𝐢×√(x))", "(x^(3/2)×𝐢+𝐢+x)/(√(x)×𝐢+1)");

  //assert_parsed_expression_simplify_to("log(cos(9)^ln(6), cos(9))", "ln(2)+ln(3)"); // TODO: for this to work, we must know the sign of cos(9)
  //assert_parsed_expression_simplify_to("log(cos(9)^ln(6), 9)", "ln(6)×log(cos(9), 9)"); // TODO: for this to work, we must know the sign of cos(9)
  assert_parsed_expression_simplify_to("(((√(6)-√(2))/4)/((√(6)+√(2))/4))+1", "-√(3)+3");
  //assert_parsed_expression_simplify_to("1/√(𝐢) × (√(2)-𝐢×√(2))", "-2𝐢"); // TODO: get rid of complex at denominator?
}

QUIZ_CASE(poincare_simplify_matrix) {
  assert_parsed_expression_simplify_to("abs([[1,-1][2,-3]])", "[[1,1][2,3]]");
  assert_parsed_expression_simplify_to("acos([[1,0]])", "[[0,π/2]]");
  assert_parsed_expression_simplify_to("asin([[1,0]])", "[[π/2,0]]");
  assert_parsed_expression_simplify_to("atan([[1,0]])", "[[π/4,0]]");
  assert_parsed_expression_simplify_to("binomial([[0,180]],1)", "undef");
  assert_parsed_expression_simplify_to("binomial(1,[[0,180]])", "undef");
  assert_parsed_expression_simplify_to("binomial([[0,180]],[[1]])", "undef");
  assert_parsed_expression_simplify_to("ceil([[0.3,180]])", "[[1,180]]");
  assert_parsed_expression_simplify_to("arg([[1,1+𝐢]])", "[[0,π/4]]");
  assert_parsed_expression_simplify_to("confidence([[0,180]],1)", "undef");
  assert_parsed_expression_simplify_to("confidence(1,[[0,180]])", "undef");
  assert_parsed_expression_simplify_to("confidence([[0,180]],[[1]])", "undef");
  assert_parsed_expression_simplify_to("conj([[1,1+𝐢]])", "[[1,1-𝐢]]");
  assert_parsed_expression_simplify_to("cos([[0,π]])", "[[1,-1]]");
  assert_parsed_expression_simplify_to("diff([[0,180]],x,1)", "undef");
  assert_parsed_expression_simplify_to("diff(1,x,[[0,180]])", "undef");
  assert_parsed_expression_simplify_to("quo([[0,180]],1)", "undef");
  assert_parsed_expression_simplify_to("quo(1,[[0,180]])", "undef");
  assert_parsed_expression_simplify_to("quo([[0,180]],[[1]])", "undef");
  assert_parsed_expression_simplify_to("rem([[0,180]],1)", "undef");
  assert_parsed_expression_simplify_to("rem(1,[[0,180]])", "undef");
  assert_parsed_expression_simplify_to("rem([[0,180]],[[1]])", "undef");
  assert_parsed_expression_simplify_to("factor([[0,180]])", "undef");
  assert_parsed_expression_simplify_to("[[1,3]]!", "[[1,6]]");
  assert_parsed_expression_simplify_to("floor([[0.3,180]])", "[[0,180]]");
  assert_parsed_expression_simplify_to("frac([[0.3,180]])", "[[3/10,0]]");
  assert_parsed_expression_simplify_to("gcd([[0,180]],1)", "undef");
  assert_parsed_expression_simplify_to("gcd(1,[[0,180]])", "undef");
  assert_parsed_expression_simplify_to("gcd([[0,180]],[[1]])", "undef");
  assert_parsed_expression_simplify_to("acosh([[0,π]])", "[[acosh(0),acosh(π)]]");
  assert_parsed_expression_simplify_to("asinh([[0,π]])", "[[asinh(0),asinh(π)]]");
  assert_parsed_expression_simplify_to("atanh([[0,π]])", "[[atanh(0),atanh(π)]]");
  assert_parsed_expression_simplify_to("cosh([[0,π]])", "[[cosh(0),cosh(π)]]");
  assert_parsed_expression_simplify_to("sinh([[0,π]])", "[[sinh(0),sinh(π)]]");
  assert_parsed_expression_simplify_to("tanh([[0,π]])", "[[tanh(0),tanh(π)]]");
  assert_parsed_expression_simplify_to("im([[1,1+𝐢]])", "[[0,1]]");
  assert_parsed_expression_simplify_to("int([[0,180]],x,1,2)", "undef");
  assert_parsed_expression_simplify_to("int(1,x,[[0,180]],1)", "undef");
  assert_parsed_expression_simplify_to("int(1,x,1,[[0,180]])", "undef");
  assert_parsed_expression_simplify_to("log([[2,3]])", "[[log(2),log(3)]]");
  assert_parsed_expression_simplify_to("log([[2,3]],5)", "[[log(2,5),log(3,5)]]");
  assert_parsed_expression_simplify_to("log(5,[[2,3]])", "undef");
  assert_parsed_expression_simplify_to("ln([[2,3]])", "[[ln(2),ln(3)]]");
  assert_parsed_expression_simplify_to("root([[2,3]],5)", "undef");
  assert_parsed_expression_simplify_to("root(5,[[2,3]])", "undef");
  assert_parsed_expression_simplify_to("permute([[2,3]],5)", "undef");
  assert_parsed_expression_simplify_to("permute(5,[[2,3]])", "undef");
  assert_parsed_expression_simplify_to("prediction([[2,3]],5)", "undef");
  assert_parsed_expression_simplify_to("prediction(5,[[2,3]])", "undef");
  assert_parsed_expression_simplify_to("prediction95([[2,3]],5)", "undef");
  assert_parsed_expression_simplify_to("prediction95(5,[[2,3]])", "undef");
  assert_parsed_expression_simplify_to("product(1,x,[[0,180]],1)", "undef");
  assert_parsed_expression_simplify_to("product(1,x,1,[[0,180]])", "undef");
  assert_parsed_expression_simplify_to("randint([[2,3]],5)", "undef");
  assert_parsed_expression_simplify_to("randint(5,[[2,3]])", "undef");
  assert_parsed_expression_simplify_to("re([[1,𝐢]])", "[[1,0]]");
  assert_parsed_expression_simplify_to("round([[2.12,3.42]], 1)", "[[21/10,17/5]]");
  assert_parsed_expression_simplify_to("round(1.3, [[2.1,3.4]])", "undef");
  assert_parsed_expression_simplify_to("round(1.3, [[2.1,3.4]])", "undef");
  assert_parsed_expression_simplify_to("sign([[2.1,3.4]])", "undef");
  assert_parsed_expression_simplify_to("sin([[0,π]])", "[[0,0]]");
  assert_parsed_expression_simplify_to("sum(1,x,[[0,180]],1)", "undef");
  assert_parsed_expression_simplify_to("sum(1,x,1,[[0,180]])", "undef");
  assert_parsed_expression_simplify_to("√([[2.1,3.4]])", "undef");
  assert_parsed_expression_simplify_to("[[2,3.4]]-[[0.1,3.1]]", "[[19/10,3/10]]");
  assert_parsed_expression_simplify_to("[[2,3.4]]-1", "undef");
  assert_parsed_expression_simplify_to("1-[[0.1,3.1]]", "undef");
  assert_parsed_expression_simplify_to("tan([[0,π/4]])", "[[0,1]]");
}
