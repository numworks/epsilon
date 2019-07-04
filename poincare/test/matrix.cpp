#include <quiz.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_matrix_evaluate) {
  assert_parsed_expression_evaluates_to<float>("[[1,2,3][4,5,6]]", "[[1,2,3][4,5,6]]");
  assert_parsed_expression_evaluates_to<double>("[[1,2,3][4,5,6]]", "[[1,2,3][4,5,6]]");
}

QUIZ_CASE(poincare_matrix_simplify) {
  // Addition Matrix
  assert_parsed_expression_simplify_to("1+[[1,2,3][4,5,6]]", Undefined::Name());
  assert_parsed_expression_simplify_to("[[1,2,3][4,5,6]]+1", Undefined::Name());
  assert_parsed_expression_simplify_to("[[1,2][3,4]]+[[1,2,3][4,5,6]]", Undefined::Name());
  assert_parsed_expression_simplify_to("2+[[1,2,3][4,5,6]]+[[1,2,3][4,5,6]]", Undefined::Name());
  assert_parsed_expression_simplify_to("[[1,2,3][4,5,6]]+cos(2)+[[1,2,3][4,5,6]]", Undefined::Name());
  assert_parsed_expression_simplify_to("[[1,2,3][4,5,6]]+[[1,2,3][4,5,6]]", "[[2,4,6][8,10,12]]");

  // Multiplication Matrix
  assert_parsed_expression_simplify_to("2*[[1,2,3][4,5,6]]", "[[2,4,6][8,10,12]]");
  assert_parsed_expression_simplify_to("[[1,2,3][4,5,6]]×√(2)", "[[√(2),2×√(2),3×√(2)][4×√(2),5×√(2),6×√(2)]]");
  assert_parsed_expression_simplify_to("[[1,2][3,4]]*[[1,2,3][4,5,6]]", "[[9,12,15][19,26,33]]");
  assert_parsed_expression_simplify_to("[[1,2,3][4,5,6]]*[[1,2][2,3][5,6]]", "[[20,26][44,59]]");
  assert_parsed_expression_simplify_to("[[1,2,3,4][4,5,6,5]]*[[1,2][2,3][5,6]]", Undefined::Name());
  assert_parsed_expression_simplify_to("[[1,2][3,4]]^(-3)*[[1,2][3,4]]", "[[11/2,-5/2][-15/4,7/4]]");
  assert_parsed_expression_simplify_to("[[1,2][3,4]]^(-3)*[[1,2,3][3,4,5]]*[[1,2][3,2][4,5]]*4", "[[-176,-186][122,129]]");

  // Power Matrix
  assert_parsed_expression_simplify_to("[[1,2,3][4,5,6][7,8,9]]^3", "[[468,576,684][1062,1305,1548][1656,2034,2412]]");
  assert_parsed_expression_simplify_to("[[1,2,3][4,5,6]]^(-1)", Undefined::Name());
  assert_parsed_expression_simplify_to("[[1,2][3,4]]^(-1)", "[[-2,1][3/2,-1/2]]");

  // Determinant
  assert_parsed_expression_simplify_to("det(π+π)", "2×π");
  assert_parsed_expression_simplify_to("det([[π+π]])", "2×π");
  assert_parsed_expression_simplify_to("det([[1,2][3,4]])", "-2");
  assert_parsed_expression_simplify_to("det([[2,2][3,4]])", "2");
  assert_parsed_expression_simplify_to("det([[2,2][3,4][3,4]])", Undefined::Name());
  assert_parsed_expression_simplify_to("det([[2,2][3,3]])", "0");
  assert_parsed_expression_simplify_to("det([[1,2,3][4,5,6][7,8,9]])", "0");
  assert_parsed_expression_simplify_to("det([[1,2,3][4,5,6][7,8,9]])", "0");
  assert_parsed_expression_simplify_to("det([[1,2,3][4π,5,6][7,8,9]])", "24×π-24");

  // Dimension
  assert_parsed_expression_simplify_to("dim(3)", "[[1,1]]");
  assert_parsed_expression_simplify_to("dim([[1/√(2),1/2,3][2,1,-3]])", "[[2,3]]");

  // Inverse
  assert_parsed_expression_simplify_to("inverse([[1/√(2),1/2,3][2,1,-3]])", Undefined::Name());
  assert_parsed_expression_simplify_to("inverse([[1,2][3,4]])", "[[-2,1][3/2,-1/2]]");
  assert_parsed_expression_simplify_to("inverse([[π,2π][3,2]])", "[[-1/(2×π),1/2][3/(4×π),-1/4]]");

  // Trace
  assert_parsed_expression_simplify_to("trace([[1/√(2),1/2,3][2,1,-3]])", Undefined::Name());
  assert_parsed_expression_simplify_to("trace([[√(2),2][4,3+log(3)]])", "log(3)+√(2)+3");
  assert_parsed_expression_simplify_to("trace(√(2)+log(3))", "log(3)+√(2)");

  // Transpose
  assert_parsed_expression_simplify_to("transpose([[1/√(2),1/2,3][2,1,-3]])", "[[√(2)/2,2][1/2,1][3,-3]]");
  assert_parsed_expression_simplify_to("transpose(√(4))", "2");
}

QUIZ_CASE(poincare_matrix_simplify_with_functions) {
  assert_parsed_expression_simplify_to("abs([[1,-1][2,-3]])", "[[1,1][2,3]]");
  assert_parsed_expression_simplify_to("acos([[1/√(2),1/2][1,-1]])", "[[π/4,π/3][0,π]]");
  assert_parsed_expression_simplify_to("acos([[1,0]])", "[[0,π/2]]");
  assert_parsed_expression_simplify_to("asin([[1/√(2),1/2][1,-1]])", "[[π/4,π/6][π/2,-π/2]]");
  assert_parsed_expression_simplify_to("asin([[1,0]])", "[[π/2,0]]");
  assert_parsed_expression_simplify_to("atan([[√(3),1][1/√(3),-1]])", "[[π/3,π/4][π/6,-π/4]]");
  assert_parsed_expression_simplify_to("atan([[1,0]])", "[[π/4,0]]");
  assert_parsed_expression_simplify_to("binomial([[0,180]],1)", Undefined::Name());
  assert_parsed_expression_simplify_to("binomial(1,[[0,180]])", Undefined::Name());
  assert_parsed_expression_simplify_to("binomial([[0,180]],[[1]])", Undefined::Name());
  assert_parsed_expression_simplify_to("ceil([[0.3,180]])", "[[1,180]]");
  assert_parsed_expression_simplify_to("arg([[1,1+𝐢]])", "[[0,π/4]]");
  assert_parsed_expression_simplify_to("confidence([[0,180]],1)", Undefined::Name());
  assert_parsed_expression_simplify_to("confidence(1,[[0,180]])", Undefined::Name());
  assert_parsed_expression_simplify_to("confidence([[0,180]],[[1]])", Undefined::Name());
  assert_parsed_expression_simplify_to("confidence(1/3, 25)", "[[2/15,8/15]]");
  assert_parsed_expression_simplify_to("confidence(45, 25)", Undefined::Name());
  assert_parsed_expression_simplify_to("confidence(1/3, -34)", Undefined::Name());
  assert_parsed_expression_simplify_to("conj([[1,1+𝐢]])", "[[1,1-𝐢]]");
  assert_parsed_expression_simplify_to("cos([[π/3,0][π/7,π/2]])", "[[1/2,1][cos(π/7),0]]");
  assert_parsed_expression_simplify_to("cos([[0,π]])", "[[1,-1]]");
  assert_parsed_expression_simplify_to("diff([[0,180]],x,1)", Undefined::Name());
  assert_parsed_expression_simplify_to("diff(1,x,[[0,180]])", Undefined::Name());
  assert_parsed_expression_simplify_to("quo([[0,180]],1)", Undefined::Name());
  assert_parsed_expression_simplify_to("quo(1,[[0,180]])", Undefined::Name());
  assert_parsed_expression_simplify_to("quo([[0,180]],[[1]])", Undefined::Name());
  assert_parsed_expression_simplify_to("rem([[0,180]],1)", Undefined::Name());
  assert_parsed_expression_simplify_to("rem(1,[[0,180]])", Undefined::Name());
  assert_parsed_expression_simplify_to("rem([[0,180]],[[1]])", Undefined::Name());
  assert_parsed_expression_simplify_to("factor([[0,180]])", Undefined::Name());
  assert_parsed_expression_simplify_to("[[1,3]]!", "[[1,6]]");
  assert_parsed_expression_simplify_to("[[1,2][3,4]]!", "[[1,2][6,24]]");
  assert_parsed_expression_simplify_to("floor([[1/√(2),1/2][1,-1.3]])", "[[floor(√(2)/2),0][1,-2]]");
  assert_parsed_expression_simplify_to("floor([[0.3,180]])", "[[0,180]]");
  assert_parsed_expression_simplify_to("frac([[1/√(2),1/2][1,-1.3]])", "[[frac(√(2)/2),1/2][0,7/10]]");
  assert_parsed_expression_simplify_to("frac([[0.3,180]])", "[[3/10,0]]");
  assert_parsed_expression_simplify_to("gcd([[0,180]],1)", Undefined::Name());
  assert_parsed_expression_simplify_to("gcd(1,[[0,180]])", Undefined::Name());
  assert_parsed_expression_simplify_to("gcd([[0,180]],[[1]])", Undefined::Name());
  assert_parsed_expression_simplify_to("acosh([[0,π]])", "[[acosh(0),acosh(π)]]");
  assert_parsed_expression_simplify_to("asinh([[0,π]])", "[[asinh(0),asinh(π)]]");
  assert_parsed_expression_simplify_to("atanh([[0,π]])", "[[atanh(0),atanh(π)]]");
  assert_parsed_expression_simplify_to("cosh([[0,π]])", "[[cosh(0),cosh(π)]]");
  assert_parsed_expression_simplify_to("sinh([[0,π]])", "[[sinh(0),sinh(π)]]");
  assert_parsed_expression_simplify_to("tanh([[0,π]])", "[[tanh(0),tanh(π)]]");
  assert_parsed_expression_simplify_to("im([[1/√(2),1/2][1,-1]])", "[[0,0][0,0]]");
  assert_parsed_expression_simplify_to("im([[1,1+𝐢]])", "[[0,1]]");
  assert_parsed_expression_simplify_to("int([[0,180]],x,1,2)", Undefined::Name());
  assert_parsed_expression_simplify_to("int(1,x,[[0,180]],1)", Undefined::Name());
  assert_parsed_expression_simplify_to("int(1,x,1,[[0,180]])", Undefined::Name());
  assert_parsed_expression_simplify_to("log([[2,3]])", "[[log(2),log(3)]]");
  assert_parsed_expression_simplify_to("log([[2,3]],5)", "[[log(2,5),log(3,5)]]");
  assert_parsed_expression_simplify_to("log(5,[[2,3]])", Undefined::Name());
  assert_parsed_expression_simplify_to("log([[√(2),1/2][1,3]])", "[[log(2)/2,-log(2)][0,log(3)]]");
  assert_parsed_expression_simplify_to("log([[1/√(2),1/2][1,-3]])", "[[-log(2)/2,-log(2)][0,log(-3)]]"); // ComplexFormat is Cartesian
  assert_parsed_expression_simplify_to("log([[1/√(2),1/2][1,-3]],3)", "[[-log(2,3)/2,-log(2,3)][0,log(-3,3)]]");
  assert_parsed_expression_simplify_to("ln([[2,3]])", "[[ln(2),ln(3)]]");
  assert_parsed_expression_simplify_to("ln([[√(2),1/2][1,3]])", "[[ln(2)/2,-ln(2)][0,ln(3)]]");
  assert_parsed_expression_simplify_to("root([[2,3]],5)", Undefined::Name());
  assert_parsed_expression_simplify_to("root(5,[[2,3]])", Undefined::Name());
  assert_parsed_expression_simplify_to("-[[1/√(2),1/2,3][2,1,-3]]", "[[-√(2)/2,-1/2,-3][-2,-1,3]]");
  assert_parsed_expression_simplify_to("permute([[2,3]],5)", Undefined::Name());
  assert_parsed_expression_simplify_to("permute(5,[[2,3]])", Undefined::Name());
  assert_parsed_expression_simplify_to("prediction([[2,3]],5)", Undefined::Name());
  assert_parsed_expression_simplify_to("prediction(5,[[2,3]])", Undefined::Name());
  assert_parsed_expression_simplify_to("prediction95([[2,3]],5)", Undefined::Name());
  assert_parsed_expression_simplify_to("prediction95(5,[[2,3]])", Undefined::Name());
  assert_parsed_expression_simplify_to("prediction95(1/3, 25)", "[[(-49×√(2)+125)/375,(49×√(2)+125)/375]]");
  assert_parsed_expression_simplify_to("prediction95(45, 25)", Undefined::Name());
  assert_parsed_expression_simplify_to("prediction95(1/3, -34)", Undefined::Name());
  assert_parsed_expression_simplify_to("product(1,x,[[0,180]],1)", Undefined::Name());
  assert_parsed_expression_simplify_to("product(1,x,1,[[0,180]])", Undefined::Name());
  assert_parsed_expression_simplify_to("randint([[2,3]],5)", Undefined::Name());
  assert_parsed_expression_simplify_to("randint(5,[[2,3]])", Undefined::Name());
  assert_parsed_expression_simplify_to("re([[1,𝐢]])", "[[1,0]]");
  assert_parsed_expression_simplify_to("round([[2.12,3.42]], 1)", "[[21/10,17/5]]");
  assert_parsed_expression_simplify_to("round(1.3, [[2.1,3.4]])", Undefined::Name());
  assert_parsed_expression_simplify_to("round(1.3, [[2.1,3.4]])", Undefined::Name());
  assert_parsed_expression_simplify_to("sign([[2.1,3.4]])", Undefined::Name());
  assert_parsed_expression_simplify_to("sin([[π/3,0][π/7,π/2]])", "[[√(3)/2,0][sin(π/7),1]]");
  assert_parsed_expression_simplify_to("sin([[0,π]])", "[[0,0]]");
  assert_parsed_expression_simplify_to("sum(1,x,[[0,180]],1)", Undefined::Name());
  assert_parsed_expression_simplify_to("sum(1,x,1,[[0,180]])", Undefined::Name());
  assert_parsed_expression_simplify_to("√([[2.1,3.4]])", Undefined::Name());
  assert_parsed_expression_simplify_to("[[2,3.4]]-[[0.1,3.1]]", "[[19/10,3/10]]");
  assert_parsed_expression_simplify_to("[[2,3.4]]-1", Undefined::Name());
  assert_parsed_expression_simplify_to("1-[[0.1,3.1]]", Undefined::Name());
  assert_parsed_expression_simplify_to("tan([[0,π/4]])", "[[0,1]]");
}
