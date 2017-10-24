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

void assert_parsed_expression_simplify_to(const char * expression, const char * simplifiedExpression) {
  GlobalContext globalContext;
  Expression * e = parse_expression(expression);
#if POINCARE_TESTS_PRINT_EXPRESSIONS
  cout << "---- Simplify: " << expression << "----"  << endl;
#endif
  Expression::simplifyAndBeautify(&e, globalContext, Expression::AngleUnit::Radian);
#if POINCARE_TESTS_PRINT_EXPRESSIONS
  print_expression(e, 0);
#endif
  Expression * f = parse_expression(simplifiedExpression);
  Expression::simplifyAndBeautify(&f, globalContext, Expression::AngleUnit::Radian);
#if POINCARE_TESTS_PRINT_EXPRESSIONS
  cout << "---- compared to: " << simplifiedExpression << "----"  << endl;
  print_expression(f, 0);
#endif
  assert(e->compareTo(f) == 0);
  delete e;
  delete f;
}


QUIZ_CASE(poincare_simplify_easy) {
  assert_parsed_expression_simplify_to("X^(P)*X^(5)", "X^(P+5)");
  assert_parsed_expression_simplify_to("tan(62P/21)", "-tan(P/21)");
  assert_parsed_expression_simplify_to("cos(26P/21)/sin(25P/17)", "cos(5P/21)/sin(8P/17)");
  assert_parsed_expression_simplify_to("cos(62P/21)*P*3/sin(62P/21)", "-3P/tan(P/21)");
  assert_parsed_expression_simplify_to("cos(62P/21)/(P*3sin(62P/21))", "-1/(3Ptan(P/21))");
  assert_parsed_expression_simplify_to("sin(62P/21)*P*3/cos(62P/21)", "-3Ptan(P/21)");
  assert_parsed_expression_simplify_to("sin(62P/21)/(P*3cos(62P/21))", "-tan(P/21)/(3P)");
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
  assert_parsed_expression_simplify_to("(1+R(2))/5", "(1+R(2))/5"); // TODO: put 5 on denominator?
  assert_parsed_expression_simplify_to("(2+R(6))^2", "(2+R(6))^2"); // Check for parenthesis
  assert_parsed_expression_simplify_to("cos(0)", "1");
  assert_parsed_expression_simplify_to("cos(P)", "-1");
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
  assert_parsed_expression_simplify_to("cos(P/4+1000P)", "1/R(2)"); // TODO: change result to R(2)/2
  assert_parsed_expression_simplify_to("cos(-P/3)", "1/2");
  assert_parsed_expression_simplify_to("cos(41P/5)", "(5^(1/2)+1)*4^(-1)");
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
  assert_parsed_expression_simplify_to("sin(P/4+1000P)", "1/R(2)"); // TODO: change result to R(2)/2
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
  assert_parsed_expression_simplify_to("tan(P/12)", "(-R(2)/4+R(6)/4)/(R(2)/4+R(6)/4)"); // Replace by 2-R(3)
  assert_parsed_expression_simplify_to("tan(-P/12)", "(R(2)/4-R(6)/4)/(R(2)/4+R(6)/4)"); // Replace by R(3)-2
  assert_parsed_expression_simplify_to("tan(-P*R(2))", "-tan(P*R(2))");
  assert_parsed_expression_simplify_to("tan(1311P/6)", "undef");
  assert_parsed_expression_simplify_to("tan(-P17/8)", "-R(2-R(2))/R(2+R(2))");
  assert_parsed_expression_simplify_to("tan(41P/6)", "-1/R(3)");
  assert_parsed_expression_simplify_to("tan(P/4+1000P)", "R(2)/R(2)"); // TODO: replace by 1
  assert_parsed_expression_simplify_to("tan(-P/3)", "-R(3)");
  assert_parsed_expression_simplify_to("sin(x)/cos(x)", "tan(x)");
  assert_parsed_expression_simplify_to("cos(x)/sin(x)", "1/tan(x)");
  assert_parsed_expression_simplify_to("sin(x)*P/cos(x)", "P*tan(x)");
  assert_parsed_expression_simplify_to("sin(x)/(P*cos(x))", "tan(x)/P");
  assert_parsed_expression_simplify_to("56^56", "79164324866862966607842406018063254671922245312646690223362402918484170424104310169552592050323456");

  assert_parsed_expression_simplify_to("acos(-1/2)", "P*2*3^(-1)");
  assert_parsed_expression_simplify_to("acos(-1.2)", "undef");
  assert_parsed_expression_simplify_to("acos(cos(2/3))", "2/3");
  assert_parsed_expression_simplify_to("acos(cos(3/2))", "3/2");
  assert_parsed_expression_simplify_to("acos(cos(12))", "undef");
  /* This does not work but should not as it is above k_primorial32 = 1*3*5*7*11*... (product of first 32 primes. */
  //assert_parsed_expression_simplify_to("1881676377434183981909562699940347954480361860897069^(1/3)", "123456789123456789");

  //assert_parsed_expression_simplify_to("1/sqrt(2)", "sqrt(2)/2");
}
