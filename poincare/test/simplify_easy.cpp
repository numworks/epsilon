#include <quiz.h>
#include <poincare.h>
#include <ion.h>
#include <assert.h>
#include "helper.h"
#if POINCARE_TESTS_PRINT_EXPRESSIONS
#include "../src/expression_debug.h"
#include <iostream>
using namespace std;
#endif

using namespace Poincare;

void assert_parsed_expression_simplify_to(const char * expression, const char * simplifiedExpression, Expression::AngleUnit angleUnit = Expression::AngleUnit::Radian) {
  GlobalContext globalContext;
  Expression * e = parse_expression(expression);
#if POINCARE_TESTS_PRINT_EXPRESSIONS
  cout << "---- Simplify: " << expression << "----"  << endl;
#endif
  Expression::Simplify(&e, globalContext, angleUnit);
#if POINCARE_TESTS_PRINT_EXPRESSIONS
  print_expression(e, 0);
#endif
  Expression * f = parse_expression(simplifiedExpression);
  Expression::Simplify(&f, globalContext, angleUnit);
#if POINCARE_TESTS_PRINT_EXPRESSIONS
  cout << "---- compared to: " << simplifiedExpression << "----"  << endl;
  print_expression(f, 0);
#endif
  assert(e->isIdenticalTo(f));
  delete e;
  delete f;
}

QUIZ_CASE(poincare_simplify_easy) {
  //assert_parsed_expression_simplify_to("(((R(6)-R(2))/4)/((R(6)+R(2))/4))+1", "((1/2)*R(6))/((R(6)+R(2))/4)");
  // Addition Matrix
#if MATRIX_EXACT_REDUCING
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
  /* Complex */
  assert_parsed_expression_simplify_to("I", "I");
  assert_parsed_expression_simplify_to("R(-33)", "R(33)*I");
  assert_parsed_expression_simplify_to("I^(3/5)", "X^(IP3/10)");

  //Functions
  assert_parsed_expression_simplify_to("binomial(20,3)", "1140");
  assert_parsed_expression_simplify_to("binomial(20,10)", "184756");
  assert_parsed_expression_simplify_to("ceil(-1.3)", "-1");
  assert_parsed_expression_simplify_to("conj(1/2)", "1/2");
  assert_parsed_expression_simplify_to("quo(19,3)", "6");
  assert_parsed_expression_simplify_to("quo(-19,3)", "-7");
  assert_parsed_expression_simplify_to("rem(19,3)", "1");
  assert_parsed_expression_simplify_to("rem(-19,3)", "2");
  assert_parsed_expression_simplify_to("99!", "933262154439441526816992388562667004907159682643816214685929638952175999932299156089414639761565182862536979208272237582511852109168640000000000000000000000");
  assert_parsed_expression_simplify_to("floor(-1.3)", "-2");
  assert_parsed_expression_simplify_to("frac(-1.3)", "0.7");
  assert_parsed_expression_simplify_to("gcd(123,278)", "1");
  assert_parsed_expression_simplify_to("gcd(11,121)", "11");
  assert_parsed_expression_simplify_to("lcm(123,278)", "34194");
  assert_parsed_expression_simplify_to("lcm(11,121)", "121");
  assert_parsed_expression_simplify_to("root(4,3)", "4^(1/3)");
  assert_parsed_expression_simplify_to("permute(102,4)", "101989800");
  assert_parsed_expression_simplify_to("permute(20,-10)", "undef");
  assert_parsed_expression_simplify_to("re(1/2)", "1/2");

  assert_parsed_expression_simplify_to("1*tan(2)*tan(5)", "tan(2)*tan(5)");
  assert_parsed_expression_simplify_to("P+(3R(2)-2R(3))/25", "(3R(2)-2R(3)+25P)/25");
  assert_parsed_expression_simplify_to("-1/3", "-1/3");
  assert_parsed_expression_simplify_to("2+13cos(2)-23cos(2)", "2-10cos(2)");
  assert_parsed_expression_simplify_to("1/(R(2)+R(3))", "-(R(2)-R(3))");
  assert_parsed_expression_simplify_to("1/(5+R(3))", "(5-R(3))/22");
  assert_parsed_expression_simplify_to("1/(R(2)+4)", "(4-R(2))/14");
  assert_parsed_expression_simplify_to("1/(2R(2)-4)", "-R(2)/4-1/2");
  assert_parsed_expression_simplify_to("1/(-2R(2)+4)", "R(2)/4+1/2");
  assert_parsed_expression_simplify_to("5!", "120");
  assert_parsed_expression_simplify_to("1/3!", "1/6");
  assert_parsed_expression_simplify_to("(1/3)!", "undef");
  assert_parsed_expression_simplify_to("P!", "undef");
  assert_parsed_expression_simplify_to("X!", "undef");
  assert_parsed_expression_simplify_to("tan(62P/21)", "-tan(P/21)");
  assert_parsed_expression_simplify_to("cos(26P/21)/sin(25P/17)", "cos(5P/21)/sin(8P/17)");
  assert_parsed_expression_simplify_to("cos(62P/21)*P*3/sin(62P/21)", "-3P/tan(P/21)");
  assert_parsed_expression_simplify_to("cos(62P/21)/(P*3sin(62P/21))", "-1/(3Ptan(P/21))");
  assert_parsed_expression_simplify_to("sin(62P/21)*P*3/cos(62P/21)", "-3Ptan(P/21)");
  assert_parsed_expression_simplify_to("sin(62P/21)/(P*3cos(62P/21))", "-tan(P/21)/(3P)");
  assert_parsed_expression_simplify_to("-cos(P/62)ln(3)/(sin(P/62)P)", "-ln(3)/(tan(P/62)P)");
  assert_parsed_expression_simplify_to("-2cos(P/62)ln(3)/(sin(P/62)P)", "-2ln(3)/(tan(P/62)P)");
  assert_parsed_expression_simplify_to("0000.000000", "0");
  assert_parsed_expression_simplify_to(".000000", "0");
  assert_parsed_expression_simplify_to("0000", "0");
  assert_parsed_expression_simplify_to("0.1234567", "1234567/10000000");
  assert_parsed_expression_simplify_to("123.4567", "1234567/10000");
  assert_parsed_expression_simplify_to("0.1234", "1234/10000");
  assert_parsed_expression_simplify_to("0.1234000", "1234/10000");
  assert_parsed_expression_simplify_to("001234000", "1234000");
  assert_parsed_expression_simplify_to("001.234000E3", "1234");
  assert_parsed_expression_simplify_to("001234000E-4", "1234/10");
  assert_parsed_expression_simplify_to("1+1+ln(2)+(5+3*2)/9-4/7+1/98", "ln(2)+2347/882");
  assert_parsed_expression_simplify_to("1+1+ln(2)+(5+3*2)/9-4/7+1/98", "ln(2)+2347/882");
  assert_parsed_expression_simplify_to("1+1+ln(2)+(5+3*2)/9-4/7+1/98", "ln(2)+2347/882");
  assert_parsed_expression_simplify_to("3/4+5/4-12+1/567", "-5669/567");
  assert_parsed_expression_simplify_to("34/78+67^(-1)", "1178/2613");
  assert_parsed_expression_simplify_to("root(4,5)", "4^(1/5)");
  assert_parsed_expression_simplify_to("R(4)", "2");
  assert_parsed_expression_simplify_to("3^4", "81");
  assert_parsed_expression_simplify_to("3^(-4)", "1/81");
  assert_parsed_expression_simplify_to("12348/34564", "3087/8641");
  assert_parsed_expression_simplify_to("1256^(1/3)*x", "2*157^(1/3)*x");
  assert_parsed_expression_simplify_to("1256^(-1/3)", "2^(-1)*157^(-1/3)");
  assert_parsed_expression_simplify_to("32^(-1/5)", "1/2");
  assert_parsed_expression_simplify_to("ln(2+3)", "ln(5)");
  assert_parsed_expression_simplify_to("1-0.3-0.7", "0");
  assert_parsed_expression_simplify_to("(2+3-4)^(x)", "1");
  assert_parsed_expression_simplify_to("1^x", "1");
  assert_parsed_expression_simplify_to("x^1", "x");
  assert_parsed_expression_simplify_to("0^3", "0");
  assert_parsed_expression_simplify_to("0^0", "1");
  assert_parsed_expression_simplify_to("0^(-3)", "undef");
  assert_parsed_expression_simplify_to("0*x+B", "B");
  assert_parsed_expression_simplify_to("0*x*0*32*cos(3)", "0");
  assert_parsed_expression_simplify_to("1+2+0+cos(2)", "3+cos(2)");
  assert_parsed_expression_simplify_to("2+0", "2");
  assert_parsed_expression_simplify_to("3*A*B*C+4*cos(2)-2*A*B*C+A*B*C+ln(3)+4*A*B-5*A*B*C+cos(3)*ln(5)+cos(2)-45*cos(2)", "-3ABC+4AB-40cos(2)+cos(3)ln(5)+ln(3)");
  assert_parsed_expression_simplify_to("2*A+3*cos(2)+3+4*ln(5)+5*A+2*ln(5)+1+0", "7A+3cos(2)+6ln(5)+4");
  assert_parsed_expression_simplify_to("2.3*A+3*cos(2)+3+4*ln(5)+5*A+2*ln(5)+1.2+0.235", "73*A/10+3cos(2)+6ln(5)+4435/1000");
  assert_parsed_expression_simplify_to("2*A*B*C+2.3*A*B+3*A^2+5.2*A*B*C+4*A^2", "36/5*ABC+23/10*AB+7A^2");
  assert_parsed_expression_simplify_to("3*A^4*B^x*B^2*(A^2+2)*2*1.2", "36/5*A^6*B^(x+2)+72/5*A^4*B^(x+2)");
  assert_parsed_expression_simplify_to("A*(B+C)*(D+3)", "ABD+3AB+ACD+3AC");
  assert_parsed_expression_simplify_to("A/B", "A/B");
  assert_parsed_expression_simplify_to("-5P+3P", "-2P");
  assert_parsed_expression_simplify_to("(A*B)^2", "A^2*B^2");
  assert_parsed_expression_simplify_to("(A*B)^2*A+4*A^3", "A^3*B^2+4A^3");
  assert_parsed_expression_simplify_to("(A*3)^2*A+4*A^3", "13A^3");
  assert_parsed_expression_simplify_to("A^2^2*A+4*A^3", "A^5+4A^3");
  assert_parsed_expression_simplify_to("4^0.5", "2");
  assert_parsed_expression_simplify_to("8^0.5", "2R(2)");
  assert_parsed_expression_simplify_to("(12^4*3)^(0.5)", "144*R(3)");
  assert_parsed_expression_simplify_to("(2^A)^B", "2^(AB)");
  assert_parsed_expression_simplify_to("(2*A)^B", "2^B*A^B");
  assert_parsed_expression_simplify_to("(12^4*x)^(0.5)", "144R(x)");
  assert_parsed_expression_simplify_to("45^2", "2025");
  assert_parsed_expression_simplify_to("1-3+A-5+2A-4A", "-7-A");
  assert_parsed_expression_simplify_to("(1/2)*A/B", "A/(2B)");
  assert_parsed_expression_simplify_to("0.5+4*A*B-2.3+2*A*B-2*B*A^C-cos(4)+2*A^C*B+A*B*C*D", "ABCD+6AB-cos(4)-9/5");
  assert_parsed_expression_simplify_to("1+2", "3");
  assert_parsed_expression_simplify_to("123456789123456789+112233445566778899", "235690234690235688");
  assert_parsed_expression_simplify_to("1+2+3+4+5+6", "21");
  assert_parsed_expression_simplify_to("1-2+3-4+5-6", "-3");
  assert_parsed_expression_simplify_to("987654321123456789*998877665544332211", "986545842648570754445552922919330479");
  assert_parsed_expression_simplify_to("2/3", "2/3");
  assert_parsed_expression_simplify_to("9/17+5/4", "121/68");
  assert_parsed_expression_simplify_to("1/2*3/4", "3/8");
  assert_parsed_expression_simplify_to("0*2/3", "0");
  assert_parsed_expression_simplify_to("1+(1/(1+1/(1+1/(1+1))))", "8/5");
  assert_parsed_expression_simplify_to("1+2/(3+4/(5+6/(7+8)))", "155/101");
  assert_parsed_expression_simplify_to("3/4*16/12", "1");
  assert_parsed_expression_simplify_to("3/4*(8+8)/12", "1");
  assert_parsed_expression_simplify_to("916791/794976477", "305597/264992159");
  assert_parsed_expression_simplify_to("321654987123456789/112233445566778899", "3249040273974311/1133671167341201");
  assert_parsed_expression_simplify_to("0.1+0.2", "3/10");
  assert_parsed_expression_simplify_to("2^3", "8");
  assert_parsed_expression_simplify_to("(-1)*(-1)", "1");
  assert_parsed_expression_simplify_to("(-2)^2", "4");
  assert_parsed_expression_simplify_to("(-3)^3", "-27");
  assert_parsed_expression_simplify_to("(1/2)^-1", "2");
  assert_parsed_expression_simplify_to("R(32)", "4*R(2)");
  assert_parsed_expression_simplify_to("R(3^2)", "3");
  assert_parsed_expression_simplify_to("2^(2+P)", "4*2^P");
  assert_parsed_expression_simplify_to("R(15241578780673678515622620750190521)", "123456789123456789");
  assert_parsed_expression_simplify_to("R(P)^2", "P");
  assert_parsed_expression_simplify_to("R(P^2)", "P");
  assert_parsed_expression_simplify_to("R((-P)^2)", "P");
  assert_parsed_expression_simplify_to("R(x*144)", "12*R(x)");
  assert_parsed_expression_simplify_to("R(x*144*P^2)", "12*P*R(x)");
  assert_parsed_expression_simplify_to("R(x*144*P)", "12*R(xP)");
  assert_parsed_expression_simplify_to("abs(P)", "P");
  assert_parsed_expression_simplify_to("abs(-P)", "P");
  assert_parsed_expression_simplify_to("R(2)*R(3)", "R(6)");
  assert_parsed_expression_simplify_to("2*2^P", "2*2^P");
  assert_parsed_expression_simplify_to("A-A", "0");
  assert_parsed_expression_simplify_to("A-A+2cos(2)+B-B-cos(2)", "cos(2)");
  assert_parsed_expression_simplify_to("A^3*B*A^(-3)", "B");
  assert_parsed_expression_simplify_to("A^3*A^(-3)", "1");
  assert_parsed_expression_simplify_to("2^P*(1/2)^P", "1");
  assert_parsed_expression_simplify_to("A^3*A^(-3)", "1");
  assert_parsed_expression_simplify_to("1+A+2+B+3", "6+A+B");
  assert_parsed_expression_simplify_to("(x+1)*(x+2)", "x^2+3x+2");

  assert_parsed_expression_simplify_to("(x+1)*(x-1)", "-1+x^2");
  assert_parsed_expression_simplify_to("11P/(22P+11P)", "1/3");
  assert_parsed_expression_simplify_to("11/(22P+11P)", "1/(3P)");
  assert_parsed_expression_simplify_to("A^2*BA^(-2)*B^(-2)", "1/B");
  assert_parsed_expression_simplify_to("A^(-1)*B^(-1)", "1/(AB)");
  assert_parsed_expression_simplify_to("-11/(22P+11P)", "-1/(3P)");
  assert_parsed_expression_simplify_to("-A", "-A");
  assert_parsed_expression_simplify_to("1/(x+1)+1/(P+2)", "(P+x+3)/((x+1)(P+2))");
  assert_parsed_expression_simplify_to("1/x^2+1/(x^2*P)", "(P+1)/(x^2*P)");
  assert_parsed_expression_simplify_to("1/x^2+1/(x^3*P)", "(Px+1)/(x^3*P)");
  assert_parsed_expression_simplify_to("4x/x^2+3P/(x^3*P)", "(4*x^2*P+3P)/(x^3*P)");
  assert_parsed_expression_simplify_to("x^(1/2)", "R(x)");
  assert_parsed_expression_simplify_to("x^(-1/2)", "1/R(x)");
  assert_parsed_expression_simplify_to("x^(1/7)", "root(x, 7)");
  assert_parsed_expression_simplify_to("x^(-1/7)", "1/root(x, 7)");
  assert_parsed_expression_simplify_to("log(12925)", "log(47)+log(11)+2*log(5)");
  assert_parsed_expression_simplify_to("ln(12925)", "ln(47)+ln(11)+2*ln(5)");
  assert_parsed_expression_simplify_to("log(1742279/12925, 6)", "log(7, 6)+3log(11, 6)+log(17,6)-log(47,6)-2*log(5,6)");
  assert_parsed_expression_simplify_to("ln(2/3)", "ln(2)-ln(3)");
  assert_parsed_expression_simplify_to("log(1742279/12925, -6)", "undef");
  assert_parsed_expression_simplify_to("(1+R(2))/5", "(1+R(2))/5");
  assert_parsed_expression_simplify_to("(2+R(6))^2", "(2+R(6))^2"); // Check for parenthesis
  assert_parsed_expression_simplify_to("cos(0)", "1");
  assert_parsed_expression_simplify_to("cos(P)", "-1");
  assert_parsed_expression_simplify_to("cos(P*4/7)", "-cos(3P/7)");
  assert_parsed_expression_simplify_to("cos(P*35/29)", "-cos(P*6/29)");
  assert_parsed_expression_simplify_to("cos(-P*35/29)", "-cos(P*6/29)");
  assert_parsed_expression_simplify_to("cos(P*340000)", "1");
  assert_parsed_expression_simplify_to("cos(-P*340001)", "-1");
  assert_parsed_expression_simplify_to("cos(-P*R(2))", "cos(P*R(2))");
  assert_parsed_expression_simplify_to("cos(1311P/6)", "0");
  assert_parsed_expression_simplify_to("cos(P/12)", "(R(6)+R(2))/4");
  assert_parsed_expression_simplify_to("cos(-P/12)", "(R(6)+R(2))/4");
  assert_parsed_expression_simplify_to("cos(-P17/8)", "R(R(2)+2)/2");
  assert_parsed_expression_simplify_to("cos(41P/6)", "-R(3)/2");
  assert_parsed_expression_simplify_to("cos(P/4+1000P)", "R(2)/2");
  assert_parsed_expression_simplify_to("cos(-P/3)", "1/2");
  assert_parsed_expression_simplify_to("cos(41P/5)", "(5^(1/2)+1)*4^(-1)");
  assert_parsed_expression_simplify_to("cos(7P/10)", "-R(5/8-R(5)/8)");
  assert_parsed_expression_simplify_to("sin(0)", "0");
  assert_parsed_expression_simplify_to("sin(P)", "0");
  assert_parsed_expression_simplify_to("sin(P*35/29)", "-sin(P*6/29)");
  assert_parsed_expression_simplify_to("sin(-P*35/29)", "sin(P*6/29)");
  assert_parsed_expression_simplify_to("sin(P*340000)", "0");
  assert_parsed_expression_simplify_to("sin(P*340001)", "0");
  assert_parsed_expression_simplify_to("sin(-P*340001)", "0");
  assert_parsed_expression_simplify_to("sin(P/12)", "(R(6)-R(2))/4");
  assert_parsed_expression_simplify_to("sin(-P/12)", "(R(2)-R(6))/4");
  assert_parsed_expression_simplify_to("sin(-P*R(2))", "-sin(P*R(2))");
  assert_parsed_expression_simplify_to("sin(1311P/6)", "1");
  assert_parsed_expression_simplify_to("sin(-P17/8)", "-R(-R(2)+2)/2");
  assert_parsed_expression_simplify_to("sin(41P/6)", "1/2");
  assert_parsed_expression_simplify_to("sin(-3P/10)", "(-1-R(5))/4");
  assert_parsed_expression_simplify_to("sin(P/4+1000P)", "R(2)/2");
  assert_parsed_expression_simplify_to("sin(-P/3)", "-R(3)/2");
  assert_parsed_expression_simplify_to("sin(17P/5)", "-R(5/8+R(5)/8)");
  assert_parsed_expression_simplify_to("sin(P/5)", "R(5/8-R(5)/8)");

  assert_parsed_expression_simplify_to("tan(0)", "0");
  assert_parsed_expression_simplify_to("tan(P)", "0");
  assert_parsed_expression_simplify_to("tan(P*35/29)", "tan(P*6/29)");
  assert_parsed_expression_simplify_to("tan(-P*35/29)", "-tan(P*6/29)");
  assert_parsed_expression_simplify_to("tan(P*340000)", "0");
  assert_parsed_expression_simplify_to("tan(P*340001)", "0");
  assert_parsed_expression_simplify_to("tan(-P*340001)", "0");
  assert_parsed_expression_simplify_to("tan(P/12)", "2-R(3)");
  assert_parsed_expression_simplify_to("tan(-P/12)", "R(3)-2");
  assert_parsed_expression_simplify_to("tan(-P*R(2))", "-tan(P*R(2))");
  assert_parsed_expression_simplify_to("tan(1311P/6)", "undef");
  assert_parsed_expression_simplify_to("tan(-P17/8)", "1-R(2)");
  assert_parsed_expression_simplify_to("tan(41P/6)", "-1/R(3)");
  assert_parsed_expression_simplify_to("tan(P/4+1000P)", "1");
  assert_parsed_expression_simplify_to("tan(-P/3)", "-R(3)");
  assert_parsed_expression_simplify_to("tan(-P/10)", "-R(1-2/R(5))");
  assert_parsed_expression_simplify_to("sin(x)/cos(x)", "tan(x)");
  assert_parsed_expression_simplify_to("cos(x)/sin(x)", "1/tan(x)");
  assert_parsed_expression_simplify_to("sin(x)*P/cos(x)", "P*tan(x)");
  assert_parsed_expression_simplify_to("sin(x)/(P*cos(x))", "tan(x)/P");
  assert_parsed_expression_simplify_to("56^56", "79164324866862966607842406018063254671922245312646690223362402918484170424104310169552592050323456");

  assert_parsed_expression_simplify_to("acos(-1/2)", "P*2*3^(-1)");
  assert_parsed_expression_simplify_to("acos(-1.2)", "undef");
  assert_parsed_expression_simplify_to("acos(cos(2/3))", "2/3");
  assert_parsed_expression_simplify_to("acos(cos(3/2))", "3/2");
  assert_parsed_expression_simplify_to("cos(acos(3/2))", "undef");
  assert_parsed_expression_simplify_to("cos(acos(2/3))", "2/3");
  assert_parsed_expression_simplify_to("acos(cos(12))", "acos(cos(12))");
  assert_parsed_expression_simplify_to("acos(cos(4P/7))", "4P/7");
  assert_parsed_expression_simplify_to("acos(-cos(2))", "P-2");
  assert_parsed_expression_simplify_to("asin(-1/2)", "P*(-6)^(-1)");
  assert_parsed_expression_simplify_to("asin(-1.2)", "undef");
  assert_parsed_expression_simplify_to("asin(sin(2/3))", "2/3");
  assert_parsed_expression_simplify_to("sin(asin(2/3))", "2/3");
  assert_parsed_expression_simplify_to("sin(asin(3/2))", "sin(asin(3/2))");
  assert_parsed_expression_simplify_to("asin(sin(3/2))", "3/2");
  assert_parsed_expression_simplify_to("asin(sin(12))", "asin(sin(12))");
  assert_parsed_expression_simplify_to("asin(sin(-P/7))", "-P/7");
  assert_parsed_expression_simplify_to("asin(sin(-R(2)))", "-R(2)");
  assert_parsed_expression_simplify_to("atan(-1)", "P*(-4)^(-1)");
  assert_parsed_expression_simplify_to("atan(-1.2)", "atan(-1.2)");
  assert_parsed_expression_simplify_to("atan(tan(2/3))", "2/3");
  assert_parsed_expression_simplify_to("tan(atan(2/3))", "2/3");
  assert_parsed_expression_simplify_to("tan(atan(5/2))", "5/2");
  assert_parsed_expression_simplify_to("atan(tan(5/2))", "atan(tan(5/2))");
  assert_parsed_expression_simplify_to("atan(tan(5/2))", "atan(tan(5/2))");
  assert_parsed_expression_simplify_to("atan(tan(-P/7))", "-P/7");
  assert_parsed_expression_simplify_to("atan(R(3))", "P/3");
  assert_parsed_expression_simplify_to("atan(tan(-R(2)))", "-R(2)");

  assert_parsed_expression_simplify_to("cos(0)", "1", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("cos(180)", "-1", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("cos(720/7)", "-cos(540/7)", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("cos(6300/29)", "-cos(1080/29)", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("cos(-6300/29)", "-cos(1080/29)", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("cos(61200000)", "1", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("cos(-61200180)", "-1", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("cos(-180*R(2))", "cos(180*R(2))", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("cos(39330)", "0", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("cos(15)", "(R(6)+R(2))/4", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("cos(-15)", "(R(6)+R(2))/4", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("cos(-765/2)", "R(R(2)+2)/2", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("cos(7380/6)", "-R(3)/2", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("cos(180045)", "R(2)/2", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("cos(-60)", "1/2", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("cos(7380/5)", "(5^(1/2)+1)*4^(-1)", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("cos(112.5)", "-R(2-R(2))/2", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("sin(0)", "0", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("sin(180)", "0", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("sin(6300/29)", "-sin(1080/29)", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("sin(-6300/29)", "sin(1080/29)", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("sin(61200000)", "0", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("sin(61200180)", "0", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("sin(-61200180)", "0", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("sin(15)", "(R(6)-R(2))/4", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("sin(-15)", "(R(2)-R(6))/4", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("sin(-180*R(2))", "-sin(180*R(2))", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("sin(39330)", "1", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("sin(-765/2)", "-R(-R(2)+2)/2", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("sin(1230)", "1/2", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("sin(180045)", "R(2)/2", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("sin(-60)", "-R(3)/2", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("sin(612)", "-R(5/8+R(5)/8)", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("sin(36)", "R(5/8-R(5)/8)", Expression::AngleUnit::Degree);

  assert_parsed_expression_simplify_to("tan(0)", "0", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("tan(180)", "0", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("tan(6300/29)", "tan(1080/29)", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("tan(-6300/29)", "-tan(1080/29)", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("tan(61200000)", "0", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("tan(61200180)", "0", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("tan(-61200180)", "0", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("tan(15)", "2-R(3)", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("tan(-15)", "R(3)-2", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("tan(-180*R(2))", "-tan(180*R(2))", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("tan(39330)", "undef", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("tan(-382.5)", "1-R(2)", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("tan(1230)", "-1/R(3)", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("tan(180045)", "1", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("tan(-60)", "-R(3)", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("acos(-1/2)", "120", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("acos(-1.2)", "undef", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("acos(cos(2/3))", "2/3", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("acos(cos(190))", "acos(cos(190))", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("acos(cos(75))", "75", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("cos(acos(190))", "undef", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("cos(acos(75))", "undef", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("acos(cos(12))", "12", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("acos(cos(720/7))", "720/7", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("asin(-1/2)", "-30", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("asin(-1.2)", "undef", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("asin(sin(75))", "75", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("sin(asin(75))", "undef", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("sin(asin(190))", "sin(asin(190))", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("asin(sin(32))", "32", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("asin(sin(400))", "asin(sin(400))", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("asin(sin(-180/7))", "-180/7", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("atan(-1)", "-45", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("atan(-1.2)", "atan(-1.2)", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("atan(tan(-45))", "-45", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("tan(atan(120))", "120", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("tan(atan(2293))", "2293", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("atan(tan(2293))", "-47", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("atan(tan(1808))", "8", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("atan(tan(-180/7))", "-180/7", Expression::AngleUnit::Degree);
  assert_parsed_expression_simplify_to("atan(R(3))", "60", Expression::AngleUnit::Degree);

  assert_parsed_expression_simplify_to("1/(3R(2))", "R(2)/6");
  assert_parsed_expression_simplify_to("1/(R(2)ln(3))", "R(2)/(2ln(3))");


  assert_parsed_expression_simplify_to("A+B-A-B", "0");
  assert_parsed_expression_simplify_to("A+B+(-1)*A+(-1)*B", "0");
  assert_parsed_expression_simplify_to("ln(R(2))", "ln(2)/2");
  assert_parsed_expression_simplify_to("R(3/2)", "R(6)/2");
  assert_parsed_expression_simplify_to("tan(3)ln(2)+P", "tan(3)ln(2)+P");
  assert_parsed_expression_simplify_to("ln(X^3)", "3");
  assert_parsed_expression_simplify_to("log(10)", "1");
  assert_parsed_expression_simplify_to("log(R(3),R(3))", "1");
  assert_parsed_expression_simplify_to("X^ln(3)", "3");
  assert_parsed_expression_simplify_to("X^ln(R(3))", "R(3)");
  assert_parsed_expression_simplify_to("P^log(R(3),P)", "R(3)");
  assert_parsed_expression_simplify_to("10^log(P)", "P");
  assert_parsed_expression_simplify_to("log(1/R(2))", "-log(2)/2");
  assert_parsed_expression_simplify_to("log(-I)", "log(-I)");
  assert_parsed_expression_simplify_to("R(-I)", "R(-I)");

  /* This does not work but should not as it is above k_primorial32 = 1*3*5*7*11*... (product of first 32 primes. */
  //assert_parsed_expression_simplify_to("1881676377434183981909562699940347954480361860897069^(1/3)", "123456789123456789");

  //assert_parsed_expression_simplify_to("1/sqrt(2)", "sqrt(2)/2");
}
