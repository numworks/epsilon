#include <quiz.h>
#include <poincare.h>
#include <assert.h>
#include "helper.h"

using namespace Poincare;

QUIZ_CASE(poincare_matrix_evaluate) {
#if MATRICES_ARE_DEFINED
  Complex<float> a[6] = {Complex<float>::Float(1.0f), Complex<float>::Float(2.0f), Complex<float>::Float(3.0f), Complex<float>::Float(4.0f), Complex<float>::Float(5.0f), Complex<float>::Float(6.0f)};
  assert_parsed_expression_evaluates_to("[[1,2,3][4,5,6]]", a, 2, 3);

  Complex<double> b[6] = {Complex<double>::Float(1.0), Complex<double>::Float(2.0), Complex<double>::Float(3.0), Complex<double>::Float(4.0), Complex<double>::Float(5.0), Complex<double>::Float(6.0)};
  assert_parsed_expression_evaluates_to("[[1,2,3][4,5,6]]", b, 2, 3);
#endif
}

QUIZ_CASE(poincare_matrix_simplify) {
#if MATRICES_ARE_DEFINED
#if MATRIX_EXACT_REDUCING
  // Addition Matrix
  assert_parsed_expression_simplify_to("1+[[1,2,3][4,5,6]]", "[[2,3,4][5,6,7]]");
  assert_parsed_expression_simplify_to("[[1,2,3][4,5,6]]+1", "[[2,3,4][5,6,7]]");
  assert_parsed_expression_simplify_to("[[1,2][3,4]]+[[1,2,3][4,5,6]]", "undef");
  assert_parsed_expression_simplify_to("[[1,2,3][4,5,6]]+[[1,2,3][4,5,6]]", "[[2,4,6][8,10,12]]");
  assert_parsed_expression_simplify_to("2+[[1,2,3][4,5,6]]+[[1,2,3][4,5,6]]", "[[4,6,8][10,12,14]]");
  assert_parsed_expression_simplify_to("[[1,2,3][4,5,6]]+cos(2)+[[1,2,3][4,5,6]]", "[[2+cos(2),4+cos(2),6+cos(2)][8+cos(2),10+cos(2),12+cos(2)]]");
  assert_parsed_expression_simplify_to("[[1,2,3][4,5,6]]+10+[[1,2,3][4,5,6]]+R(2)", "[[12+R(2),14+R(2),16+R(2)][18+R(2),20+R(2),22+R(2)]]");
  assert_parsed_expression_simplify_to("[[1,2][3,4]]^(-1)+3", "inverse([[1,2][3,4]])+3");
  assert_parsed_expression_simplify_to("[[1,2][3,4]]^(-3)+3", "inverse([[37,54][81,118]])+3");
  assert_parsed_expression_simplify_to("[[1,2][3,4]]^(-3)+[[1,2][3,4]]", "inverse([[37,54][81,118]])+[[1,2][3,4]]");
  assert_parsed_expression_simplify_to("[[1,2][3,4]]^(-3)+[[1,2][3,4]]+4+R(2)", "inverse([[37,54][81,118]])+[[5+R(2),6+R(2)][7+R(2),8+R(2)]]");

  // Multiplication Matrix
  assert_parsed_expression_simplify_to("2*[[1,2,3][4,5,6]]", "[[2,4,6][8,10,12]]");
  assert_parsed_expression_simplify_to("[[1,2,3][4,5,6]]*R(2)", "[[R(2),2R(2),3R(2)][4R(2),5R(2),6R(2)]]");
  assert_parsed_expression_simplify_to("[[1,2][3,4]]*[[1,2,3][4,5,6]]", "[[9, 12, 15][19, 26, 33]]");
  assert_parsed_expression_simplify_to("[[1,2,3][4,5,6]]*[[1,2][2,3][5,6]]", "[[20, 26][44, 59]]");
  assert_parsed_expression_simplify_to("[[1,2,3,4][4,5,6,5]]*[[1,2][2,3][5,6]]", "undef");
  assert_parsed_expression_simplify_to("[[1,2][3,4]]^(-3)*[[1,2][3,4]]", "[[1,2][3,4]]^(-3)*[[1,2][3,4]]");
  assert_parsed_expression_simplify_to("[[1,2][3,4]]^(-3)*[[1,2,3][3,4,5]]*[[1,2][3,2][4,5]]*4", "[[37,54][81,118]]^(-1)*[[76,84][140,156]]");
  assert_parsed_expression_simplify_to("[[1,2][3,4]]^(-3)*[[1,2][3,4]]", "[[1,2][3,4]]^(-3)*[[1,2][3,4]]");

  // Power Matrix
  assert_parsed_expression_simplify_to("[[1,2,3][4,5,6][7,8,9]]^3", "[[468,576,684][1062,1305,1548][1656,2034,2412]]");
  assert_parsed_expression_simplify_to("[[1,2,3][4,5,6]]^(-1)", "undef");
  assert_parsed_expression_simplify_to("[[1,2][3,4]]^(-1)", "[[1,2][3,4]]^(-1)"); // TODO: Implement matrix inverse for dim < 3

  // Function on matrix
  assert_parsed_expression_simplify_to("abs([[1,-2][3,4]])", "[[1,2][3,4]]");
  assert_parsed_expression_simplify_to("acos([[1/R(2),1/2][1,-1]])", "[[P/4,P/3][0,P]]");
  assert_parsed_expression_simplify_to("asin([[1/R(2),1/2][1,-1]])", "[[P/4,P/6][P/2,-P/2]]");
  assert_parsed_expression_simplify_to("atan([[R(3),1][1/R(3),-1]])", "[[P/3,P/4][P/6,-P/4]]");
  assert_parsed_expression_simplify_to("acos([[1/R(2),1/2][1,-1]])", "[[P/4,P/3][0,P]]");
  assert_parsed_expression_simplify_to("binomial([[1,-2][3,4]], 2)", "undef");
  assert_parsed_expression_simplify_to("ceil([[1/R(2),1/2][1,-1.3]])", "[[ceil(R(2)/2),1][1,-1]]");
  assert_parsed_expression_simplify_to("confidence(1/3, 25)", "[[2/15,8/15]]");
  assert_parsed_expression_simplify_to("confidence(45, 25)", "undef");
  assert_parsed_expression_simplify_to("confidence(1/3, -34)", "undef");
  assert_parsed_expression_simplify_to("conj([[1/R(2),1/2][1,-1]])", "[[conj(1/R(2)),1/2][1,-1]]");
  assert_parsed_expression_simplify_to("cos([[P/3,0][P/7,P/2]])", "[[1/2,1][cos(P/7),0]]");
  assert_parsed_expression_simplify_to("diff([[P/3,0][P/7,P/2]],3)", "undef");
  assert_parsed_expression_simplify_to("det([[1,2][3,4]])", "det([[1,2][3,4]])"); // TODO: implement determinant if dim < 3
  assert_parsed_expression_simplify_to("det([[2,2][3,4]])", "det([[2,2][3,4]])");
  assert_parsed_expression_simplify_to("det([[2,2][3,3]])", "det([[2,2][3,3]])");
  assert_parsed_expression_simplify_to("quo([[2,2][3,3]],2)", "undef");
  assert_parsed_expression_simplify_to("rem([[2,2][3,3]],2)", "undef");
  assert_parsed_expression_simplify_to("[[1,2][3,4]]!", "[[1,2][6,24]]");
  assert_parsed_expression_simplify_to("floor([[1/R(2),1/2][1,-1.3]])", "[[floor(R(2)/2),0][1,-2]]");
  assert_parsed_expression_simplify_to("frac([[1/R(2),1/2][1,-1.3]])", "[[frac(R(2)/2),1/2][0,0.7]]");
  assert_parsed_expression_simplify_to("gcd([[1/R(2),1/2][1,-1.3]], [[1]])", "undef");
  assert_parsed_expression_simplify_to("asinh([[1/R(2),1/2][1,-1]])", "[[asinh(1/R(2)),asinh(1/2)][asinh(1),asinh(-1)]]");
  assert_parsed_expression_simplify_to("atanh([[R(3),1][1/R(3),-1]])", "[[atanh(R(3)),atanh(1)][atanh(1/R(3)),atanh(-1)]]");
  assert_parsed_expression_simplify_to("acosh([[1/R(2),1/2][1,-1]])", "[[acosh(1/R(2)),acosh(1/2)][acosh(1),acosh(-1)]]");
  assert_parsed_expression_simplify_to("sinh([[1/R(2),1/2][1,-1]])", "[[sinh(1/R(2)),sinh(1/2)][sinh(1),sinh(-1)]]");
  assert_parsed_expression_simplify_to("tanh([[R(3),1][1/R(3),-1]])", "[[tanh(R(3)),tanh(1)][tanh(1/R(3)),tanh(-1)]]");
  assert_parsed_expression_simplify_to("cosh([[1/R(2),1/2][1,-1]])", "[[cosh(1/R(2)),cosh(1/2)][cosh(1),cosh(-1)]]");
  assert_parsed_expression_simplify_to("im([[1/R(2),1/2][1,-1]])", "[[im(1/R(2)),0][0,0]]");
  assert_parsed_expression_simplify_to("int([[P/3,0][P/7,P/2]],3,2)", "undef");
  assert_parsed_expression_simplify_to("lcm(2, [[1]])", "undef");
  assert_parsed_expression_simplify_to("log([[R(2),1/2][1,3]])", "[[(1/2)*log(2),-log(2)][0,log(3)]]");
  assert_parsed_expression_simplify_to("log([[1/R(2),1/2][1,-3]])", "undef");
  assert_parsed_expression_simplify_to("log([[1/R(2),1/2][1,-3]],3)", "undef");
  assert_parsed_expression_simplify_to("ln([[R(2),1/2][1,3]])", "[[(1/2)*ln(2),-ln(2)][0,ln(3)]]");
  assert_parsed_expression_simplify_to("log([[1/R(2),1/2][1,-3]])", "undef");
  assert_parsed_expression_simplify_to("dim([[1/R(2),1/2,3][2,1,-3]])", "[[2,3]]");
  assert_parsed_expression_simplify_to("inverse([[1/R(2),1/2,3][2,1,-3]])", "undef");
  assert_parsed_expression_simplify_to("inverse([[1,2][3,4]])", "inverse([[1,2][3,4]])"); // TODO: implement matrix inverse if dim < 3
  assert_parsed_expression_simplify_to("trace([[1/R(2),1/2,3][2,1,-3]])", "undef");
  assert_parsed_expression_simplify_to("trace([[R(2),2][4,3+log(3)]])", "R(2)+3+log(3)");
  assert_parsed_expression_simplify_to("trace(R(2)+log(3))", "R(2)+log(3)");
  assert_parsed_expression_simplify_to("transpose([[1/R(2),1/2,3][2,1,-3]])", "[[1/R(2),2][1/2, 1][3,-3]]");
  assert_parsed_expression_simplify_to("transpose(R(4))", "2");
  assert_parsed_expression_simplify_to("root([[R(4)]],2)", "undef");
  assert_parsed_expression_simplify_to("root(4,3)", "4^(1/3)");
  assert_parsed_expression_simplify_to("-[[1/R(2),1/2,3][2,1,-3]]", "[[-1/R(2),-1/2,-3][-2,-1,3]]");
  assert_parsed_expression_simplify_to("permute([[1,-2][3,4]], 2)", "undef");
  assert_parsed_expression_simplify_to("prediction95(1/3, 25)", "[[1/3-49R(2)/375,1/3+49R(2)/375]]");
  assert_parsed_expression_simplify_to("prediction95(45, 25)", "undef");
  assert_parsed_expression_simplify_to("prediction95(1/3, -34)", "undef");
  assert_parsed_expression_simplify_to("product([[1,2][3,4]], 1/3, -34)", "product([[1,2][3,4]], 1/3, -34)");
  assert_parsed_expression_simplify_to("sum([[1,2][3,4]], 1/3, -34)", "sum([[1,2][3,4]], 1/3, -34)");
  assert_parsed_expression_simplify_to("re([[1/R(2),1/2][1,-1]])", "[[re(1/R(2)),1/2][1,-1]]");
  assert_parsed_expression_simplify_to("round([[1/R(2),1/2][1,-1]],2)", "undef");
  assert_parsed_expression_simplify_to("sin([[P/3,0][P/7,P/2]])", "[[R(3)/2,0][sin(P/7),1]]");
  assert_parsed_expression_simplify_to("R([[4,2][P/7,1]])", "[[2,R(2)][R(P/7),1]]");
  assert_parsed_expression_simplify_to("tan([[P/3,0][P/7,P/6]])", "[[R(3),0][tan(P/7),R(3)/3]]");
#else
  assert_parsed_expression_simplify_to("R([[4,2][P/7,1]])", "R([[4,2][P/7,1]])");
#endif
#endif
}
