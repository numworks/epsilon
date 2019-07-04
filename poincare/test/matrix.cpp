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
#if MATRIX_EXACT_REDUCING
  assert_parsed_expression_simplify_to("[[1,2][3,4]]^(-1)+3", "inverse([[1,2][3,4]])+3");
  assert_parsed_expression_simplify_to("[[1,2][3,4]]^(-3)+3", "inverse([[37,54][81,118]])+3");
  assert_parsed_expression_simplify_to("[[1,2][3,4]]^(-3)+[[1,2][3,4]]", "inverse([[37,54][81,118]])+[[1,2][3,4]]");
  assert_parsed_expression_simplify_to("[[1,2][3,4]]^(-3)+[[1,2][3,4]]+4+√(2)", "inverse([[37,54][81,118]])+[[5+√(2),6+√(2)][7+√(2),8+√(2)]]");
#endif

  // Multiplication Matrix
  assert_parsed_expression_simplify_to("2*[[1,2,3][4,5,6]]", "[[2,4,6][8,10,12]]");
  assert_parsed_expression_simplify_to("[[1,2,3][4,5,6]]×√(2)", "[[√(2),2×√(2),3×√(2)][4×√(2),5×√(2),6×√(2)]]");
  assert_parsed_expression_simplify_to("[[1,2][3,4]]*[[1,2,3][4,5,6]]", "[[9,12,15][19,26,33]]");
  assert_parsed_expression_simplify_to("[[1,2,3][4,5,6]]*[[1,2][2,3][5,6]]", "[[20,26][44,59]]");
  assert_parsed_expression_simplify_to("[[1,2,3,4][4,5,6,5]]*[[1,2][2,3][5,6]]", Undefined::Name());
#if MATRIX_EXACT_REDUCING
  assert_parsed_expression_simplify_to("[[1,2][3,4]]^(-3)*[[1,2][3,4]]", "[[1,2][3,4]]^(-3)*[[1,2][3,4]]");
  assert_parsed_expression_simplify_to("[[1,2][3,4]]^(-3)*[[1,2,3][3,4,5]]*[[1,2][3,2][4,5]]*4", "[[37,54][81,118]]^(-1)*[[76,84][140,156]]");
  assert_parsed_expression_simplify_to("[[1,2][3,4]]^(-3)*[[1,2][3,4]]", "[[1,2][3,4]]^(-3)*[[1,2][3,4]]");
#endif

  // Power Matrix
  assert_parsed_expression_simplify_to("[[1,2,3][4,5,6][7,8,9]]^3", "[[468,576,684][1062,1305,1548][1656,2034,2412]]");
#if MATRIX_EXACT_REDUCING
  assert_parsed_expression_simplify_to("[[1,2,3][4,5,6]]^(-1)", Undefined::Name());
  assert_parsed_expression_simplify_to("[[1,2][3,4]]^(-1)", "[[1,2][3,4]]^(-1)"); // TODO: Implement matrix inverse for dim < 3
#endif

#if MATRIX_EXACT_REDUCING
  // Function on matrix
  assert_parsed_expression_simplify_to("abs([[1,-2][3,4]])", "[[1,2][3,4]]");
  assert_parsed_expression_simplify_to("acos([[1/√(2),1/2][1,-1]])", "[[π/4,π/3][0,π]]");
  assert_parsed_expression_simplify_to("asin([[1/√(2),1/2][1,-1]])", "[[π/4,π/6][π/2,-π/2]]");
  assert_parsed_expression_simplify_to("atan([[√(3),1][1/√(3),-1]])", "[[π/3,π/4][π/6,-π/4]]");
  assert_parsed_expression_simplify_to("acos([[1/√(2),1/2][1,-1]])", "[[π/4,π/3][0,π]]");
  assert_parsed_expression_simplify_to("binomial([[1,-2][3,4]], 2)", Undefined::Name());
  assert_parsed_expression_simplify_to("ceil([[1/√(2),1/2][1,-1.3]])", "[[ceil(√(2)/2),1][1,-1]]");
  assert_parsed_expression_simplify_to("confidence(1/3, 25)", "[[2/15,8/15]]");
  assert_parsed_expression_simplify_to("confidence(45, 25)", Undefined::Name());
  assert_parsed_expression_simplify_to("confidence(1/3, -34)", Undefined::Name());
  assert_parsed_expression_simplify_to("conj([[1/√(2),1/2][1,-1]])", "[[conj(1/√(2)),1/2][1,-1]]");
  assert_parsed_expression_simplify_to("cos([[π/3,0][π/7,π/2]])", "[[1/2,1][cos(π/7),0]]");
  assert_parsed_expression_simplify_to("diff([[π/3,0][π/7,π/2]],x,3)", Undefined::Name());
  assert_parsed_expression_simplify_to("det([[1,2][3,4]])", "det([[1,2][3,4]])"); // TODO: implement determinant if dim < 3
  assert_parsed_expression_simplify_to("det([[2,2][3,4]])", "det([[2,2][3,4]])");
  assert_parsed_expression_simplify_to("det([[2,2][3,3]])", "det([[2,2][3,3]])");
  assert_parsed_expression_simplify_to("quo([[2,2][3,3]],2)", Undefined::Name());
  assert_parsed_expression_simplify_to("rem([[2,2][3,3]],2)", Undefined::Name());
  assert_parsed_expression_simplify_to("[[1,2][3,4]]!", "[[1,2][6,24]]");
  assert_parsed_expression_simplify_to("floor([[1/√(2),1/2][1,-1.3]])", "[[floor(√(2)/2),0][1,-2]]");
  assert_parsed_expression_simplify_to("frac([[1/√(2),1/2][1,-1.3]])", "[[frac(√(2)/2),1/2][0,0.7]]");
  assert_parsed_expression_simplify_to("gcd([[1/√(2),1/2][1,-1.3]], [[1]])", Undefined::Name());
  assert_parsed_expression_simplify_to("asinh([[1/√(2),1/2][1,-1]])", "[[asinh(1/√(2)),asinh(1/2)][asinh(1),asinh(-1)]]");
  assert_parsed_expression_simplify_to("atanh([[√(3),1][1/√(3),-1]])", "[[atanh(√(3)),atanh(1)][atanh(1/√(3)),atanh(-1)]]");
  assert_parsed_expression_simplify_to("acosh([[1/√(2),1/2][1,-1]])", "[[acosh(1/√(2)),acosh(1/2)][acosh(1),acosh(-1)]]");
  assert_parsed_expression_simplify_to("sinh([[1/√(2),1/2][1,-1]])", "[[sinh(1/√(2)),sinh(1/2)][sinh(1),sinh(-1)]]");
  assert_parsed_expression_simplify_to("tanh([[√(3),1][1/√(3),-1]])", "[[tanh(√(3)),tanh(1)][tanh(1/√(3)),tanh(-1)]]");
  assert_parsed_expression_simplify_to("cosh([[1/√(2),1/2][1,-1]])", "[[cosh(1/√(2)),cosh(1/2)][cosh(1),cosh(-1)]]");
  assert_parsed_expression_simplify_to("im([[1/√(2),1/2][1,-1]])", "[[im(1/√(2)),0][0,0]]");
  assert_parsed_expression_simplify_to("int([[π/3,0][π/7,π/2]],3,2)", Undefined::Name());
  assert_parsed_expression_simplify_to("lcm(2, [[1]])", Undefined::Name());
  assert_parsed_expression_simplify_to("log([[√(2),1/2][1,3]])", "[[(1/2)*log(2),-log(2)][0,log(3)]]");
  assert_parsed_expression_simplify_to("log([[1/√(2),1/2][1,-3]])", Undefined::Name());
  assert_parsed_expression_simplify_to("log([[1/√(2),1/2][1,-3]],3)", Undefined::Name());
  assert_parsed_expression_simplify_to("ln([[√(2),1/2][1,3]])", "[[(1/2)*ln(2),-ln(2)][0,ln(3)]]");
  assert_parsed_expression_simplify_to("log([[1/√(2),1/2][1,-3]])", Undefined::Name());
  assert_parsed_expression_simplify_to("dim([[1/√(2),1/2,3][2,1,-3]])", "[[2,3]]");
  assert_parsed_expression_simplify_to("inverse([[1/√(2),1/2,3][2,1,-3]])", Undefined::Name());
  assert_parsed_expression_simplify_to("inverse([[1,2][3,4]])", "inverse([[1,2][3,4]])"); // TODO: implement matrix inverse if dim < 3
#endif
  assert_parsed_expression_simplify_to("trace([[1/√(2),1/2,3][2,1,-3]])", Undefined::Name());
  assert_parsed_expression_simplify_to("trace([[√(2),2][4,3+log(3)]])", "log(3)+√(2)+3");
  assert_parsed_expression_simplify_to("trace(√(2)+log(3))", "log(3)+√(2)");
  assert_parsed_expression_simplify_to("transpose([[1/√(2),1/2,3][2,1,-3]])", "[[√(2)/2,2][1/2,1][3,-3]]");
  assert_parsed_expression_simplify_to("transpose(√(4))", "2");
#if MATRIX_EXACT_REDUCING
  assert_parsed_expression_simplify_to("root([[√(4)]],2)", Undefined::Name());
  assert_parsed_expression_simplify_to("root(4,3)", "4^(1/3)");
  assert_parsed_expression_simplify_to("-[[1/√(2),1/2,3][2,1,-3]]", "[[-1/√(2),-1/2,-3][-2,-1,3]]");
  assert_parsed_expression_simplify_to("permute([[1,-2][3,4]], 2)", Undefined::Name());
  assert_parsed_expression_simplify_to("prediction95(1/3, 25)", "[[1/3-49√(2)/375,1/3+49√(2)/375]]");
  assert_parsed_expression_simplify_to("prediction95(45, 25)", Undefined::Name());
  assert_parsed_expression_simplify_to("prediction95(1/3, -34)", Undefined::Name());
  assert_parsed_expression_simplify_to("product([[1,2][3,4]], 1/3, -34)", "product([[1,2][3,4]], 1/3, -34)");
  assert_parsed_expression_simplify_to("sum([[1,2][3,4]], 1/3, -34)", "sum([[1,2][3,4]], 1/3, -34)");
  assert_parsed_expression_simplify_to("re([[1/√(2),1/2][1,-1]])", "[[re(1/√(2)),1/2][1,-1]]");
  assert_parsed_expression_simplify_to("round([[1/√(2),1/2][1,-1]],2)", Undefined::Name());
  assert_parsed_expression_simplify_to("sin([[π/3,0][π/7,π/2]])", "[[√(3)/2,0][sin(π/7),1]]");
  assert_parsed_expression_simplify_to("√([[4,2][π/7,1]])", "[[2,√(2)][√(π/7),1]]");
  assert_parsed_expression_simplify_to("tan([[π/3,0][π/7,π/6]])", "[[√(3),0][tan(π/7),√(3)/3]]");
#endif
}
