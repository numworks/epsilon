#include <quiz.h>
#include <poincare/expression.h>
#include <poincare/rational.h>
#include <poincare/addition.h>
#include <apps/shared/global_context.h>
#include <ion.h>
#include <assert.h>
#include "helper.h"
#include "./tree/helpers.h"

using namespace Poincare;

QUIZ_CASE(poincare_simplification_decimal) {
  assert_parsed_expression_simplify_to("-2.3", "-23/10");
  assert_parsed_expression_simplify_to("-232.2ᴇ-4", "-1161/50000");
  assert_parsed_expression_simplify_to("0000.000000ᴇ-2", "0");
  assert_parsed_expression_simplify_to(".000000", "0");
  assert_parsed_expression_simplify_to("0000", "0");
}

QUIZ_CASE(poincare_simplification_rational) {
  // 1/MaxIntegerString()
  char buffer[400] = "1/";
  strlcpy(buffer+2, MaxIntegerString(), 400-2);
  assert_parsed_expression_simplify_to(buffer, buffer);
  // 1/OverflowedIntegerString()
  strlcpy(buffer+2, BigOverflowedIntegerString(), 400-2);
  assert_parsed_expression_simplify_to(buffer, "0");
  // MaxIntegerString()
  assert_parsed_expression_simplify_to(MaxIntegerString(), MaxIntegerString());
  // OverflowedIntegerString()
  assert_parsed_expression_simplify_to(BigOverflowedIntegerString(), Infinity::Name());
  assert_parsed_expression_simplify_to(BigOverflowedIntegerString(), Infinity::Name());
  // -OverflowedIntegerString()
  buffer[0] = '-';
  strlcpy(buffer+1, BigOverflowedIntegerString(), 400-1);
  assert_parsed_expression_simplify_to(buffer, "-inf");

  assert_parsed_expression_simplify_to("-1/3", "-1/3");
  assert_parsed_expression_simplify_to("22355/45325", "4471/9065");
  assert_parsed_expression_simplify_to("0000.000000", "0");
  assert_parsed_expression_simplify_to(".000000", "0");
  assert_parsed_expression_simplify_to("0000", "0");
  assert_parsed_expression_simplify_to("0.1234567", "1234567/10000000");
  assert_parsed_expression_simplify_to("123.4567", "1234567/10000");
  assert_parsed_expression_simplify_to("0.1234", "617/5000");
  assert_parsed_expression_simplify_to("0.1234000", "617/5000");
  assert_parsed_expression_simplify_to("001234000", "1234000");
  assert_parsed_expression_simplify_to("001.234000ᴇ3", "1234");
  assert_parsed_expression_simplify_to("001234000ᴇ-4", "617/5");
  assert_parsed_expression_simplify_to("3/4+5/4-12+1/567", "-5669/567");
  assert_parsed_expression_simplify_to("34/78+67^(-1)", "1178/2613");
  assert_parsed_expression_simplify_to("12348/34564", "3087/8641");
  assert_parsed_expression_simplify_to("1-0.3-0.7", "0");
  assert_parsed_expression_simplify_to("123456789123456789+112233445566778899", "235690234690235688");
  assert_parsed_expression_simplify_to("56^56", "79164324866862966607842406018063254671922245312646690223362402918484170424104310169552592050323456");
  assert_parsed_expression_simplify_to("999^999", "999^999");
  assert_parsed_expression_simplify_to("999^-999", "1/999^999");
  assert_parsed_expression_simplify_to("0^0", Undefined::Name());
  assert_parsed_expression_simplify_to("x^0", "1");
  assert_parsed_expression_simplify_to("π^0", "1");
  assert_parsed_expression_simplify_to("A^0", "1");
  assert_parsed_expression_simplify_to("(-3)^0", "1");
}

QUIZ_CASE(poincare_simplification_infinity) {
  // 0 and infinity
  assert_parsed_expression_simplify_to("0/0", Undefined::Name());
  assert_parsed_expression_simplify_to("0/inf", "0");
  assert_parsed_expression_simplify_to("inf/0", Undefined::Name());
  assert_parsed_expression_simplify_to("0×inf", Undefined::Name());
  assert_parsed_expression_simplify_to("3×inf/inf", "undef");
  assert_parsed_expression_simplify_to("1ᴇ1000", "inf");
  assert_parsed_expression_simplify_to("-1ᴇ1000", "-inf");
  assert_parsed_expression_simplify_to("-1ᴇ-1000", "0");
  assert_parsed_expression_simplify_to("1ᴇ-1000", "0");
  //assert_parsed_expression_simplify_to("1×10^1000", "inf");

  assert_parsed_expression_simplify_to("inf^0", "undef");
  assert_parsed_expression_simplify_to("1^inf", "1^inf");
  assert_parsed_expression_simplify_to("1^(X^inf)", "1^\u0012X^inf\u0013");
  assert_parsed_expression_simplify_to("inf^(-1)", "0");
  assert_parsed_expression_simplify_to("(-inf)^(-1)", "0");
  assert_parsed_expression_simplify_to("inf^(-√(2))", "0");
  assert_parsed_expression_simplify_to("(-inf)^(-√(2))", "0");
  assert_parsed_expression_simplify_to("inf^2", "inf");
  assert_parsed_expression_simplify_to("(-inf)^2", "inf");
  assert_parsed_expression_simplify_to("inf^√(2)", "inf");
  assert_parsed_expression_simplify_to("(-inf)^√(2)", "inf×(-1)^√(2)");
  assert_parsed_expression_simplify_to("inf^x", "inf^x");
  assert_parsed_expression_simplify_to("1/inf+24", "24");
  assert_parsed_expression_simplify_to("ℯ^(inf)/inf", "0×ℯ^inf");

  // Logarithm
  assert_parsed_expression_simplify_to("log(inf,0)", "undef");
  assert_parsed_expression_simplify_to("log(inf,1)", "undef");
  assert_parsed_expression_simplify_to("log(0,inf)", "undef");
  assert_parsed_expression_simplify_to("log(1,inf)", "0");
  assert_parsed_expression_simplify_to("log(inf,inf)", "undef");

  assert_parsed_expression_simplify_to("ln(inf)", "inf");
  assert_parsed_expression_simplify_to("log(inf,-3)", "log(inf,-3)");
  assert_parsed_expression_simplify_to("log(inf,3)", "inf");
  assert_parsed_expression_simplify_to("log(inf,0.3)", "-inf");
  assert_parsed_expression_simplify_to("log(inf,x)", "log(inf,x)");
  assert_parsed_expression_simplify_to("ln(inf)*0", "undef");
}

QUIZ_CASE(poincare_simplification_addition) {
  assert_parsed_expression_simplify_to("1+x", "x+1");
  assert_parsed_expression_simplify_to("1/2+1/3+1/4+1/5+1/6+1/7", "223/140");
  assert_parsed_expression_simplify_to("1+x+4-i-2x", "-i-x+5");
  assert_parsed_expression_simplify_to("2+1", "3");
  assert_parsed_expression_simplify_to("1+2", "3");
  assert_parsed_expression_simplify_to("1+2+3+4+5+6+7", "28");
  assert_parsed_expression_simplify_to("(0+0)", "0");
  assert_parsed_expression_simplify_to("2+A", "A+2");
  assert_parsed_expression_simplify_to("1+2+3+4+5+A+6+7", "A+28");
  assert_parsed_expression_simplify_to("1+A+2+B+3", "A+B+6");
  assert_parsed_expression_simplify_to("-2+6", "4");
  assert_parsed_expression_simplify_to("-2-6", "-8");
  assert_parsed_expression_simplify_to("-A", "-A");
  assert_parsed_expression_simplify_to("A-A", "0");
  assert_parsed_expression_simplify_to("-5π+3π", "-2×π");
  assert_parsed_expression_simplify_to("1-3+A-5+2A-4A", "-A-7");
  assert_parsed_expression_simplify_to("A+B-A-B", "0");
  assert_parsed_expression_simplify_to("A+B+(-1)×A+(-1)×B", "0");
  assert_parsed_expression_simplify_to("2+13cos(2)-23cos(2)", "-10×cos(2)+2");
  assert_parsed_expression_simplify_to("1+1+ln(2)+(5+3×2)/9-4/7+1/98", "\u0012882×ln(2)+2347\u0013/882");
  assert_parsed_expression_simplify_to("1+2+0+cos(2)", "cos(2)+3");
  assert_parsed_expression_simplify_to("A-A+2cos(2)+B-B-cos(2)", "cos(2)");
  assert_parsed_expression_simplify_to("x+3+π+2×x", "3×x+π+3");
  assert_parsed_expression_simplify_to("1/(x+1)+1/(π+2)", "\u0012x+π+3\u0013/\u0012π×x+2×x+π+2\u0013");
  assert_parsed_expression_simplify_to("1/x^2+1/(x^2×π)", "\u0012π+1\u0013/\u0012π×x^2\u0013");
  assert_parsed_expression_simplify_to("1/x^2+1/(x^3×π)", "\u0012π×x+1\u0013/\u0012π×x^3\u0013");
  assert_parsed_expression_simplify_to("4x/x^2+3π/(x^3×π)", "\u00124×x^2+3\u0013/x^3");
  assert_parsed_expression_simplify_to("3^(1/2)+2^(-2×3^(1/2)×ℯ^π)/2", "\u00122×2^\u00122×√(3)×ℯ^π\u0013×√(3)+1\u0013/\u00122×2^\u00122×√(3)×ℯ^π\u0013\u0013");
  assert_parsed_expression_simplify_to("[[1,2+𝐢][3,4][5,6]]+[[1,2+𝐢][3,4][5,6]]", "[[2,4+2×𝐢][6,8][10,12]]");
  assert_parsed_expression_simplify_to("3+[[1,2][3,4]]", "undef");
  assert_parsed_expression_simplify_to("[[1][3][5]]+[[1,2+𝐢][3,4][5,6]]", "undef");
  assert_parsed_expression_simplify_to("[[1,3]]+confidence(π/4, 6)+[[2,3]]", "[[3,6]]+confidence(π/4,6)");
}

QUIZ_CASE(poincare_simplification_multiplication) {
  assert_parsed_expression_simplify_to("undef×x", "undef");
  assert_parsed_expression_simplify_to("0×x+B", "B");
  assert_parsed_expression_simplify_to("0×x×0×32×cos(3)", "0");
  assert_parsed_expression_simplify_to("3×A^4×B^x×B^2×(A^2+2)×2×1.2", "\u001236×A^6×B^\u0012x+2\u0013+72×A^4×B^\u0012x+2\u0013\u0013/5");
  assert_parsed_expression_simplify_to("A×(B+C)×(D+3)", "3×A×B+3×A×C+A×B×D+A×C×D");
  assert_parsed_expression_simplify_to("A/B", "A/B");
  assert_parsed_expression_simplify_to("(A×B)^2", "A^2×B^2");
  assert_parsed_expression_simplify_to("(1/2)×A/B", "A/\u00122×B\u0013");
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
  assert_parsed_expression_simplify_to("11/(22π+11π)", "1/\u00123×π\u0013");
  assert_parsed_expression_simplify_to("-11/(22π+11π)", "-1/\u00123×π\u0013");
  assert_parsed_expression_simplify_to("A^2×B×A^(-2)×B^(-2)", "1/B");
  assert_parsed_expression_simplify_to("A^(-1)×B^(-1)", "1/\u0012A×B\u0013");
  assert_parsed_expression_simplify_to("x+x", "2×x");
  assert_parsed_expression_simplify_to("2×x+x", "3×x");
  assert_parsed_expression_simplify_to("x×2+x", "3×x");
  assert_parsed_expression_simplify_to("2×x+2×x", "4×x");
  assert_parsed_expression_simplify_to("x×2+2×n", "2×n+2×x");
  assert_parsed_expression_simplify_to("x+x+n+n", "2×n+2×x");
  assert_parsed_expression_simplify_to("x-x-n+n", "0");
  assert_parsed_expression_simplify_to("x+n-x-n", "0");
  assert_parsed_expression_simplify_to("x-x", "0");
  assert_parsed_expression_simplify_to("π×3^(1/2)×(5π)^(1/2)×(4/5)^(1/2)", "2×√(3)×π^\u00123/2\u0013");
  assert_parsed_expression_simplify_to("12^(1/4)×(π/6)×(12×π)^(1/4)", "\u0012√(3)×π^\u00125/4\u0013\u0013/3");
  assert_parsed_expression_simplify_to("[[1,2+𝐢][3,4][5,6]]×[[1,2+𝐢,3,4][5,6+𝐢,7,8]]", "[[11+5×𝐢,13+9×𝐢,17+7×𝐢,20+8×𝐢][23,30+7×𝐢,37,44][35,46+11×𝐢,57,68]]");
  assert_parsed_expression_simplify_to("[[1,2][3,4]]×[[1,3][5,6]]×[[2,3][4,6]]", "[[82,123][178,267]]");
  assert_parsed_expression_simplify_to("π×confidence(π/5,3)[[1,2]]", "π×confidence(π/5,3)×[[1,2]]");
}

QUIZ_CASE(poincare_simplification_power) {
  assert_parsed_expression_simplify_to("3^4", "81");
  assert_parsed_expression_simplify_to("3^(-4)", "1/81");
  assert_parsed_expression_simplify_to("(-3)^3", "-27");
  assert_parsed_expression_simplify_to("1256^(1/3)×x", "2×root(157,3)×x");
  assert_parsed_expression_simplify_to("1256^(-1/3)", "1/\u00122×root(157,3)\u0013");
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
  assert_parsed_expression_simplify_to("ℯ^log(πℯ)", "ℯ^\u0012log(ℯ)+log(π)\u0013");
  assert_parsed_expression_simplify_to("√(ℯ^2)", "ℯ");
  assert_parsed_expression_simplify_to("999^(10000/3)", "999^\u001210000/3\u0013");
  /* This does not reduce but should not as the integer is above
   * k_maxNumberOfPrimeFactors and thus it prime decomposition might overflow
   * 32 factors. */
  assert_parsed_expression_simplify_to("1881676377434183981909562699940347954480361860897069^(1/3)", "root(1881676377434183981909562699940347954480361860897069,3)");
  /* This does not reduce but should not as the prime decomposition involves
   * factors above k_maxNumberOfPrimeFactors. */
  assert_parsed_expression_simplify_to("1002101470343^(1/3)", "root(1002101470343,3)");
  assert_parsed_expression_simplify_to("π×π×π", "π^3");
  assert_parsed_expression_simplify_to("(x+π)^(3)", "x^3+3×π×x^2+3×π^2×x+π^3");
  assert_parsed_expression_simplify_to("(5+√(2))^(-8)", "\u0012-1003320×√(2)+1446241\u0013/78310985281");
  assert_parsed_expression_simplify_to("(5×π+√(2))^(-5)", "1/\u00123125×π^5+3125×√(2)×π^4+2500×π^3+500×√(2)×π^2+100×π+4×√(2)\u0013");
  assert_parsed_expression_simplify_to("(1+√(2)+√(3))^5", "120×√(6)+184×√(3)+224×√(2)+296");
  assert_parsed_expression_simplify_to("(π+√(2)+√(3)+x)^(-3)", "1/\u0012x^3+3×π×x^2+3×√(3)×x^2+3×√(2)×x^2+3×π^2×x+6×√(3)×π×x+6×√(2)×π×x+6×√(6)×x+15×x+π^3+3×√(3)×π^2+3×√(2)×π^2+6×√(6)×π+15×π+9×√(3)+11×√(2)\u0013");
  assert_parsed_expression_simplify_to("1.0066666666667^60", "(10066666666667/10000000000000)^60");
  assert_parsed_expression_simplify_to("2^(6+π+x)", "64×2^\u0012x+π\u0013");
  assert_parsed_expression_simplify_to("𝐢^(2/3)", "1/2+√(3)/2×𝐢");
  assert_parsed_expression_simplify_to("ℯ^(𝐢×π/3)", "1/2+√(3)/2×𝐢");
  assert_parsed_expression_simplify_to("(-1)^(1/3)", "1/2+√(3)/2×𝐢");
  assert_parsed_expression_simplify_to("R(-x)", "R(-x)");
  assert_parsed_expression_simplify_to("√(x)^2", "x", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("√(-3)^2", "unreal", User, Radian, Real);
  // Principal angle of root of unity
  assert_parsed_expression_simplify_to("(-5)^(-1/3)", "1/\u00122×root(5,3)\u0013-√(3)/\u00122×root(5,3)\u0013×𝐢", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("1+((8+√(6))^(1/2))^-1+(8+√(6))^(1/2)", "\u0012√(√(6)+8)+√(6)+9\u0013/√(√(6)+8)", User, Radian, Real);
  assert_parsed_expression_simplify_to("[[1,2][3,4]]^(-3)", "[[-59/4,27/4][81/8,-37/8]]");
  assert_parsed_expression_simplify_to("[[1,2][3,4]]^3", "[[37,54][81,118]]");
}

QUIZ_CASE(poincare_simplification_factorial) {
  assert_parsed_expression_simplify_to("1/3!", "1/6");
  assert_parsed_expression_simplify_to("5!", "120");
  assert_parsed_expression_simplify_to("(1/3)!", Undefined::Name());
  assert_parsed_expression_simplify_to("π!", Undefined::Name());
  assert_parsed_expression_simplify_to("ℯ!", Undefined::Name());
}

QUIZ_CASE(poincare_simplification_logarithm) {
  assert_parsed_expression_simplify_to("log(0,0)", Undefined::Name());
  assert_parsed_expression_simplify_to("log(0,1)", Undefined::Name());
  assert_parsed_expression_simplify_to("log(1,0)", "0");
  assert_parsed_expression_simplify_to("log(2,0)", "0");
  assert_parsed_expression_simplify_to("log(0,14)", "-inf");
  assert_parsed_expression_simplify_to("log(0,0.14)", Infinity::Name());
  assert_parsed_expression_simplify_to("log(0,0.14+𝐢)", Undefined::Name());
  assert_parsed_expression_simplify_to("log(2,1)", Undefined::Name());
  assert_parsed_expression_simplify_to("log(x,1)", Undefined::Name());
  assert_parsed_expression_simplify_to("log(12925)", "log(47)+log(11)+2×log(5)");
  assert_parsed_expression_simplify_to("ln(12925)", "ln(47)+ln(11)+2×ln(5)");
  assert_parsed_expression_simplify_to("log(1742279/12925, 6)", "-log(47,6)+log(17,6)+3×log(11,6)+log(7,6)-2×log(5,6)");
  assert_parsed_expression_simplify_to("ln(2/3)", "-ln(3)+ln(2)");
  assert_parsed_expression_simplify_to("log(1742279/12925, -6)", "log(158389/1175,-6)");
  assert_parsed_expression_simplify_to("ln(√(2))", "ln(2)/2");
  assert_parsed_expression_simplify_to("ln(ℯ^3)", "3");
  assert_parsed_expression_simplify_to("log(10)", "1");
  assert_parsed_expression_simplify_to("log(√(3),√(3))", "1");
  assert_parsed_expression_simplify_to("log(1/√(2))", "-log(2)/2");
  assert_parsed_expression_simplify_to("log(-𝐢)", "log(-𝐢)");
  assert_parsed_expression_simplify_to("ln(ℯ^(𝐢π/7))", "π/7×𝐢");
  assert_parsed_expression_simplify_to("log(10^24)", "24");
  assert_parsed_expression_simplify_to("log((23π)^4,23π)", "4");
  assert_parsed_expression_simplify_to("log(10^(2+π))", "π+2");
  assert_parsed_expression_simplify_to("ln(1881676377434183981909562699940347954480361860897069)", "ln(1881676377434183981909562699940347954480361860897069)");
  /* log(1002101470343) does no reduce to 3×log(10007) because it involves
   * prime factors above k_biggestPrimeFactor */
  assert_parsed_expression_simplify_to("log(1002101470343)", "log(1002101470343)");
  assert_parsed_expression_simplify_to("log(64,2)", "6");
  assert_parsed_expression_simplify_to("log(2,64)", "log(2,64)");
  assert_parsed_expression_simplify_to("log(1476225,5)", "10×log(3,5)+2");

  assert_parsed_expression_simplify_to("log(100)", "2");
  assert_parsed_expression_simplify_to("log(1000000)", "6");
  assert_parsed_expression_simplify_to("log(70992768,14)", "log(11,14)+log(3,14)+2×log(2,14)+5");
  assert_parsed_expression_simplify_to("log(1/6991712,14)", "-log(13,14)-5");
  assert_parsed_expression_simplify_to("log(4,10)", "2×log(2)");
}

QUIZ_CASE(poincare_simplification_random) {
  assert_parsed_expression_simplify_to("1/random()+1/3+1/4", "1/random()+7/12");
  assert_parsed_expression_simplify_to("random()+random()", "random()+random()");
  assert_parsed_expression_simplify_to("random()-random()", "-random()+random()");
  assert_parsed_expression_simplify_to("1/random()+1/3+1/4+1/random()", "1/random()+1/random()+7/12");
  assert_parsed_expression_simplify_to("random()×random()", "random()×random()");
  assert_parsed_expression_simplify_to("random()/random()", "random()/random()");
  assert_parsed_expression_simplify_to("3^random()×3^random()", "3^random()×3^random()");
  assert_parsed_expression_simplify_to("random()×ln(2)×3+random()×ln(2)×5", "5×ln(2)×random()+3×ln(2)×random()");
}

QUIZ_CASE(poincare_simplification_randint) {
  assert_parsed_expression_simplify_to("1/randint(2,2)+1/2", "1");
  assert_parsed_expression_simplify_to("randint(1, inf)", "undef");
  assert_parsed_expression_simplify_to("randint(-inf, 3)", "undef");
  assert_parsed_expression_simplify_to("randint(4, 3)", "undef");
  assert_parsed_expression_simplify_to("randint(2, 23345678909876545678)", "undef");
  assert_parsed_expression_simplify_to("randint(123456789876543, 123456789876543+10)", "undef");
}

QUIZ_CASE(poincare_simplification_function) {
  assert_parsed_expression_simplify_to("abs(π)", "π");
  assert_parsed_expression_simplify_to("abs(-π)", "π");
  assert_parsed_expression_simplify_to("abs(1+𝐢)", "√(2)");
  assert_parsed_expression_simplify_to("abs(0)", "0");
  assert_parsed_expression_simplify_to("arg(1+𝐢)", "π/4");
  assert_parsed_expression_simplify_to("binomial(20,3)", "1140");
  assert_parsed_expression_simplify_to("binomial(20,10)", "184756");
  assert_parsed_expression_simplify_to("ceil(-1.3)", "-1");
  assert_parsed_expression_simplify_to("ceil(2π)", "7");
  assert_parsed_expression_simplify_to("ceil(123456789012345678901234567892/3)", "41152263004115226300411522631");
  assert_parsed_expression_simplify_to("ceil(123456789*π)", "387850942");
  assert_parsed_expression_simplify_to("conj(1/2)", "1/2");
  assert_parsed_expression_simplify_to("quo(19,3)", "6");
  assert_parsed_expression_simplify_to("quo(19,0)", Infinity::Name());
  assert_parsed_expression_simplify_to("quo(-19,3)", "-7");
  assert_parsed_expression_simplify_to("rem(19,3)", "1");
  assert_parsed_expression_simplify_to("rem(-19,3)", "2");
  assert_parsed_expression_simplify_to("rem(19,0)", Infinity::Name());
  assert_parsed_expression_simplify_to("99!", "933262154439441526816992388562667004907159682643816214685929638952175999932299156089414639761565182862536979208272237582511852109168640000000000000000000000");
  assert_parsed_expression_simplify_to("factor(-10008/6895)", "-\u00122^3×3^2×139\u0013/\u00125×7×197\u0013");
  assert_parsed_expression_simplify_to("factor(1008/6895)", "\u00122^4×3^2\u0013/\u00125×197\u0013");
  assert_parsed_expression_simplify_to("factor(10007)", "10007");
  assert_parsed_expression_simplify_to("factor(10007^2)", Undefined::Name());
  assert_parsed_expression_simplify_to("floor(-1.3)", "-2");
  assert_parsed_expression_simplify_to("floor(2π)", "6");
  assert_parsed_expression_simplify_to("floor(123456789012345678901234567892/3)", "41152263004115226300411522630");
  assert_parsed_expression_simplify_to("floor(123456789*π)", "387850941");
  assert_parsed_expression_simplify_to("frac(-1.3)", "7/10");
  assert_parsed_expression_simplify_to("gcd(123,278)", "1");
  assert_parsed_expression_simplify_to("gcd(11,121)", "11");
  assert_parsed_expression_simplify_to("im(1+5×𝐢)", "5");
  assert_parsed_expression_simplify_to("lcm(123,278)", "34194");
  assert_parsed_expression_simplify_to("lcm(11,121)", "121");
  assert_parsed_expression_simplify_to("√(4)", "2");
  assert_parsed_expression_simplify_to("re(1+5×𝐢)", "1");
  assert_parsed_expression_simplify_to("root(4,3)", "root(4,3)");
  assert_parsed_expression_simplify_to("root(4,π)", "4^\u00121/π\u0013");
  assert_parsed_expression_simplify_to("root(27,3)", "3");
  assert_parsed_expression_simplify_to("round(4.235,2)", "106/25");
  assert_parsed_expression_simplify_to("round(4.23,0)", "4");
  assert_parsed_expression_simplify_to("round(4.9,0)", "5");
  assert_parsed_expression_simplify_to("round(12.9,-1)", "10");
  assert_parsed_expression_simplify_to("round(12.9,-2)", "0");
  assert_parsed_expression_simplify_to("sign(-23)", "-1");
  assert_parsed_expression_simplify_to("sign(-𝐢)", "sign(-𝐢)");
  assert_parsed_expression_simplify_to("sign(0)", "0");
  assert_parsed_expression_simplify_to("sign(inf)", "1");
  assert_parsed_expression_simplify_to("sign(-inf)", "-1");
  assert_parsed_expression_simplify_to("sign(undef)", "undef");
  assert_parsed_expression_simplify_to("sign(23)", "1");
  assert_parsed_expression_simplify_to("sign(log(18))", "1");
  assert_parsed_expression_simplify_to("sign(-√(2))", "-1");
  assert_parsed_expression_simplify_to("sign(x)", "sign(x)");
  assert_parsed_expression_simplify_to("sign(2+𝐢)", "sign(2+𝐢)");
  assert_parsed_expression_simplify_to("permute(99,4)", "90345024");
  assert_parsed_expression_simplify_to("permute(20,-10)", Undefined::Name());
  assert_parsed_expression_simplify_to("re(1/2)", "1/2");
}

QUIZ_CASE(poincare_simplication_trigonometry_functions) {
  // -- sin/cos -> tan
  assert_parsed_expression_simplify_to("sin(x)/cos(x)", "tan(x)");
  assert_parsed_expression_simplify_to("cos(x)/sin(x)", "1/tan(x)");
  assert_parsed_expression_simplify_to("sin(x)×π/cos(x)", "π×tan(x)");
  assert_parsed_expression_simplify_to("sin(x)/(π×cos(x))", "tan(x)/π");
  assert_parsed_expression_simplify_to("1×tan(2)×tan(5)", "tan(2)×tan(5)");
  assert_parsed_expression_simplify_to("tan(62π/21)", "-tan(π/21)");
  assert_parsed_expression_simplify_to("cos(26π/21)/sin(25π/17)", "cos(\u00125×π\u0013/21)/sin(\u00128×π\u0013/17)");
  assert_parsed_expression_simplify_to("cos(62π/21)×π×3/sin(62π/21)", "-\u00123×π\u0013/tan(π/21)");
  assert_parsed_expression_simplify_to("cos(62π/21)/(π×3×sin(62π/21))", "-1/\u00123×π×tan(π/21)\u0013");
  assert_parsed_expression_simplify_to("sin(62π/21)×π×3/cos(62π/21)", "-3×π×tan(π/21)");
  assert_parsed_expression_simplify_to("sin(62π/21)/(π×3cos(62π/21))", "-tan(π/21)/\u00123×π\u0013");
  assert_parsed_expression_simplify_to("-cos(π/62)ln(3)/(sin(π/62)π)", "-ln(3)/\u0012π×tan(π/62)\u0013");
  assert_parsed_expression_simplify_to("-2cos(π/62)ln(3)/(sin(π/62)π)", "-\u00122×ln(3)\u0013/\u0012π×tan(π/62)\u0013");
  // -- cos
  assert_parsed_expression_simplify_to("cos(0)", "1");
  assert_parsed_expression_simplify_to("cos(π)", "-1");
  assert_parsed_expression_simplify_to("cos(π×4/7)", "-cos(\u00123×π\u0013/7)");
  assert_parsed_expression_simplify_to("cos(π×35/29)", "-cos(\u00126×π\u0013/29)");
  assert_parsed_expression_simplify_to("cos(-π×35/29)", "-cos(\u00126×π\u0013/29)");
  assert_parsed_expression_simplify_to("cos(π×340000)", "1");
  assert_parsed_expression_simplify_to("cos(-π×340001)", "-1");
  assert_parsed_expression_simplify_to("cos(-π×√(2))", "cos(√(2)×π)");
  assert_parsed_expression_simplify_to("cos(1311π/6)", "0");
  assert_parsed_expression_simplify_to("cos(π/12)", "\u0012√(6)+√(2)\u0013/4");
  assert_parsed_expression_simplify_to("cos(-π/12)", "\u0012√(6)+√(2)\u0013/4");
  assert_parsed_expression_simplify_to("cos(-π17/8)", "√(√(2)+2)/2");
  assert_parsed_expression_simplify_to("cos(41π/6)", "-√(3)/2");
  assert_parsed_expression_simplify_to("cos(π/4+1000π)", "√(2)/2");
  assert_parsed_expression_simplify_to("cos(-π/3)", "1/2");
  assert_parsed_expression_simplify_to("cos(41π/5)", "\u0012√(5)+1\u0013/4");
  assert_parsed_expression_simplify_to("cos(7π/10)", "-\u0012√(2)×√(-√(5)+5)\u0013/4");
  assert_parsed_expression_simplify_to("cos(0)", "1", User, Degree);
  assert_parsed_expression_simplify_to("cos(180)", "-1", User, Degree);
  assert_parsed_expression_simplify_to("cos(720/7)", "-cos(540/7)", User, Degree);
  assert_parsed_expression_simplify_to("cos(6300/29)", "-cos(1080/29)", User, Degree);
  assert_parsed_expression_simplify_to("cos(-6300/29)", "-cos(1080/29)", User, Degree);
  assert_parsed_expression_simplify_to("cos(61200000)", "1", User, Degree);
  assert_parsed_expression_simplify_to("cos(-61200180)", "-1", User, Degree);
  assert_parsed_expression_simplify_to("cos(-180×√(2))", "cos(180×√(2))", User, Degree);
  assert_parsed_expression_simplify_to("cos(39330)", "0", User, Degree);
  assert_parsed_expression_simplify_to("cos(15)", "\u0012√(6)+√(2)\u0013/4", User, Degree);
  assert_parsed_expression_simplify_to("cos(-15)", "\u0012√(6)+√(2)\u0013/4", User, Degree);
  assert_parsed_expression_simplify_to("cos(-765/2)", "√(√(2)+2)/2", User, Degree);
  assert_parsed_expression_simplify_to("cos(7380/6)", "-√(3)/2", User, Degree);
  assert_parsed_expression_simplify_to("cos(180045)", "√(2)/2", User, Degree);
  assert_parsed_expression_simplify_to("cos(-60)", "1/2", User, Degree);
  assert_parsed_expression_simplify_to("cos(7380/5)", "\u0012√(5)+1\u0013/4", User, Degree);
  assert_parsed_expression_simplify_to("cos(112.5)", "-√(-√(2)+2)/2", User, Degree);
  // -- sin
  assert_parsed_expression_simplify_to("sin(0)", "0");
  assert_parsed_expression_simplify_to("sin(π)", "0");
  assert_parsed_expression_simplify_to("sin(π×35/29)", "-sin(\u00126×π\u0013/29)");
  assert_parsed_expression_simplify_to("sin(-π×35/29)", "sin(\u00126×π\u0013/29)");
  assert_parsed_expression_simplify_to("sin(π×340000)", "0");
  assert_parsed_expression_simplify_to("sin(π×340001)", "0");
  assert_parsed_expression_simplify_to("sin(-π×340001)", "0");
  assert_parsed_expression_simplify_to("sin(π/12)", "\u0012√(6)-√(2)\u0013/4");
  assert_parsed_expression_simplify_to("sin(-π/12)", "\u0012-√(6)+√(2)\u0013/4");
  assert_parsed_expression_simplify_to("sin(-π×√(2))", "-sin(√(2)×π)");
  assert_parsed_expression_simplify_to("sin(1311π/6)", "1");
  assert_parsed_expression_simplify_to("sin(-π17/8)", "-√(-√(2)+2)/2");
  assert_parsed_expression_simplify_to("sin(41π/6)", "1/2");
  assert_parsed_expression_simplify_to("sin(-3π/10)", "\u0012-√(5)-1\u0013/4");
  assert_parsed_expression_simplify_to("sin(π/4+1000π)", "√(2)/2");
  assert_parsed_expression_simplify_to("sin(-π/3)", "-√(3)/2");
  assert_parsed_expression_simplify_to("sin(17π/5)", "-\u0012√(2)×√(√(5)+5)\u0013/4");
  assert_parsed_expression_simplify_to("sin(π/5)", "\u0012√(2)×√(-√(5)+5)\u0013/4");
  assert_parsed_expression_simplify_to("sin(0)", "0", User, Degree);
  assert_parsed_expression_simplify_to("sin(180)", "0", User, Degree);
  assert_parsed_expression_simplify_to("sin(6300/29)", "-sin(1080/29)", User, Degree);
  assert_parsed_expression_simplify_to("sin(-6300/29)", "sin(1080/29)", User, Degree);
  assert_parsed_expression_simplify_to("sin(61200000)", "0", User, Degree);
  assert_parsed_expression_simplify_to("sin(61200180)", "0", User, Degree);
  assert_parsed_expression_simplify_to("sin(-61200180)", "0", User, Degree);
  assert_parsed_expression_simplify_to("sin(15)", "\u0012√(6)-√(2)\u0013/4", User, Degree);
  assert_parsed_expression_simplify_to("sin(-15)", "\u0012-√(6)+√(2)\u0013/4", User, Degree);
  assert_parsed_expression_simplify_to("sin(-180×√(2))", "-sin(180×√(2))", User, Degree);
  assert_parsed_expression_simplify_to("sin(39330)", "1", User, Degree);
  assert_parsed_expression_simplify_to("sin(-765/2)", "-√(-√(2)+2)/2", User, Degree);
  assert_parsed_expression_simplify_to("sin(1230)", "1/2", User, Degree);
  assert_parsed_expression_simplify_to("sin(180045)", "√(2)/2", User, Degree);
  assert_parsed_expression_simplify_to("sin(-60)", "-√(3)/2", User, Degree);
  assert_parsed_expression_simplify_to("sin(612)", "-\u0012√(2)×√(√(5)+5)\u0013/4", User, Degree);
  assert_parsed_expression_simplify_to("sin(36)", "\u0012√(2)×√(-√(5)+5)\u0013/4", User, Degree);
  // -- tan
  assert_parsed_expression_simplify_to("tan(0)", "0");
  assert_parsed_expression_simplify_to("tan(π)", "0");
  assert_parsed_expression_simplify_to("tan(3×π/2)", Undefined::Name());
  assert_parsed_expression_simplify_to("tan(π/2)", Undefined::Name());
  assert_parsed_expression_simplify_to("tan(90)", Undefined::Name(), User, Degree);
  assert_parsed_expression_simplify_to("tan(100)", Undefined::Name(), User, Gradian);
  assert_parsed_expression_simplify_to("tan(π×35/29)", "tan(\u00126×π\u0013/29)");
  assert_parsed_expression_simplify_to("tan(-π×35/29)", "-tan(\u00126×π\u0013/29)");
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
  assert_parsed_expression_simplify_to("tan(-π/10)", "-\u0012√(5)×√(-2×√(5)+5)\u0013/5");
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
  assert_parsed_expression_simplify_to("acos(-1/2)", "\u00122×π\u0013/3");
  assert_parsed_expression_simplify_to("acos(-1.2)", "-acos(6/5)+π");
  assert_parsed_expression_simplify_to("acos(cos(2/3))", "2/3");
  assert_parsed_expression_simplify_to("acos(cos(3/2))", "3/2");
  assert_parsed_expression_simplify_to("cos(acos(3/2))", "3/2");
  assert_parsed_expression_simplify_to("cos(acos(2/3))", "2/3");
  assert_parsed_expression_simplify_to("acos(cos(12))", "acos(cos(12))");
  assert_parsed_expression_simplify_to("acos(cos(4π/7))", "\u00124×π\u0013/7");
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
  assert_parsed_expression_simplify_to("atan(1/x)", "\u0012π×sign(x)-2×atan(x)\u0013/2", User, Degree);

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

QUIZ_CASE(poincare_simplication_hyperbolic_trigonometry_functions) {
  assert_parsed_expression_simplify_to("sinh(0)", "0");
  assert_parsed_expression_simplify_to("cosh(0)", "1");
  assert_parsed_expression_simplify_to("tanh(0)", "0");
  assert_parsed_expression_simplify_to("asinh(0)", "0");
  assert_parsed_expression_simplify_to("acosh(1)", "0");
  assert_parsed_expression_simplify_to("atanh(0)", "0");
}

QUIZ_CASE(poincare_simplication_matrix) {
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
  assert_parsed_expression_simplify_to("det(identity(5))", "1");

  // Dimension
  assert_parsed_expression_simplify_to("dim(3)", "[[1,1]]");
  assert_parsed_expression_simplify_to("dim([[1/√(2),1/2,3][2,1,-3]])", "[[2,3]]");

  // Inverse
  assert_parsed_expression_simplify_to("inverse([[1/√(2),1/2,3][2,1,-3]])", Undefined::Name());
  assert_parsed_expression_simplify_to("inverse([[1,2][3,4]])", "[[-2,1][3/2,-1/2]]");
  assert_parsed_expression_simplify_to("inverse([[π,2×π][3,2]])", "[[-1/\u00122×π\u0013,1/2][3/\u00124×π\u0013,-1/4]]");

  // Trace
  assert_parsed_expression_simplify_to("trace([[1/√(2),1/2,3][2,1,-3]])", Undefined::Name());
  assert_parsed_expression_simplify_to("trace([[√(2),2][4,3+log(3)]])", "log(3)+√(2)+3");
  assert_parsed_expression_simplify_to("trace(√(2)+log(3))", "log(3)+√(2)");

  // Transpose
  assert_parsed_expression_simplify_to("transpose([[1/√(2),1/2,3][2,1,-3]])", "[[√(2)/2,2][1/2,1][3,-3]]");
  assert_parsed_expression_simplify_to("transpose(√(4))", "2");

  // Expressions with unreduced matrix
  assert_simplify("confidence(cos(2)/25,3)→a");
  // Check that matrices are not permuted in multiplication
  assert_parsed_expression_simplify_to("cos(3a)*abs(transpose(a))", "cos(3×confidence(cos(2)/25,3))×abs(transpose(confidence(cos(2)/25,3)))");
  assert_parsed_expression_simplify_to("abs(transpose(a))*cos(3a)", "abs(transpose(confidence(cos(2)/25,3)))×cos(3×confidence(cos(2)/25,3))");
  Ion::Storage::sharedStorage()->recordNamed("a.exp").destroy();
}

QUIZ_CASE(poincare_simplification_functions_of_matrices) {
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
  assert_parsed_expression_simplify_to("floor([[1/√(2),1/2][1,-1.3]])", "[[0,0][1,-2]]");
  assert_parsed_expression_simplify_to("floor([[0.3,180]])", "[[0,180]]");
  assert_parsed_expression_simplify_to("frac([[1/√(2),1/2][1,-1.3]])", "[[frac(√(2)/2),1/2][0,7/10]]");
  assert_parsed_expression_simplify_to("frac([[0.3,180]])", "[[3/10,0]]");
  assert_parsed_expression_simplify_to("gcd([[0,180]],1)", Undefined::Name());
  assert_parsed_expression_simplify_to("gcd(1,[[0,180]])", Undefined::Name());
  assert_parsed_expression_simplify_to("gcd([[0,180]],[[1]])", Undefined::Name());
  assert_parsed_expression_simplify_to("acosh([[0,π]])", "[[acosh(0),acosh(π)]]");
  assert_parsed_expression_simplify_to("asinh([[0,π]])", "[[0,asinh(π)]]");
  assert_parsed_expression_simplify_to("atanh([[0,π]])", "[[0,atanh(π)]]");
  assert_parsed_expression_simplify_to("cosh([[0,π]])", "[[1,cosh(π)]]");
  assert_parsed_expression_simplify_to("sinh([[0,π]])", "[[0,sinh(π)]]");
  assert_parsed_expression_simplify_to("tanh([[0,π]])", "[[0,tanh(π)]]");
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
  assert_parsed_expression_simplify_to("prediction95(1/3, 25)", "[[\u0012-49×√(2)+125\u0013/375,\u001249×√(2)+125\u0013/375]]");
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

QUIZ_CASE(poincare_simplification_store) {
  assert_parsed_expression_simplify_to("1+2→x", "3");
  assert_parsed_expression_simplify_to("0.1+0.2→x", "3/10");
  assert_parsed_expression_simplify_to("a+a→x", "2×a");

  // Clean the storage for other tests
  Ion::Storage::sharedStorage()->recordNamed("x.exp").destroy();
}

QUIZ_CASE(poincare_simplification_store_matrix) {
  assert_parsed_expression_simplify_to("1+1→a", "2");
  assert_parsed_expression_simplify_to("[[8]]→f(x)", "[[8]]");
  assert_parsed_expression_simplify_to("[[x]]→f(x)", "[[x]]");

  // Clean the storage for other tests
  Ion::Storage::sharedStorage()->recordNamed("a.exp").destroy();
  Ion::Storage::sharedStorage()->recordNamed("f.func").destroy();
}

QUIZ_CASE(poincare_simplification_complex_format) {
  // Real
  assert_parsed_expression_simplify_to("𝐢", "unreal", User, Radian, Real);
  assert_parsed_expression_simplify_to("√(-1)", "unreal", User, Radian, Real);
  assert_parsed_expression_simplify_to("√(-1)×√(-1)", "unreal", User, Radian, Real);
  assert_parsed_expression_simplify_to("ln(-2)", "unreal", User, Radian, Real);
  assert_parsed_expression_simplify_to("(-8)^(2/3)", "4", User, Radian, Real);
  assert_parsed_expression_simplify_to("(-8)^(2/5)", "2×root(2,5)", User, Radian, Real);
  assert_parsed_expression_simplify_to("(-8)^(1/5)", "-root(8,5)", User, Radian, Real);
  assert_parsed_expression_simplify_to("(-8)^(1/4)", "unreal", User, Radian, Real);
  assert_parsed_expression_simplify_to("(-8)^(1/3)", "-2", User, Radian, Real);
  assert_parsed_expression_simplify_to("[[1,2+√(-1)]]", "unreal", User, Radian, Real);

  // User defined variable
  assert_parsed_expression_simplify_to("a", "a", User, Radian, Real);
  // a = 2+i
  assert_simplify("2+𝐢→a", Radian, Real);
  assert_parsed_expression_simplify_to("a", "unreal", User, Radian, Real);
  // Clean the storage for other tests
  Ion::Storage::sharedStorage()->recordNamed("a.exp").destroy();
  // User defined function
  assert_parsed_expression_simplify_to("f(3)", "f(3)", User, Radian, Real);
  // f : x → x+1
  assert_simplify("x+1+𝐢→f(x)", Radian, Real);
  assert_parsed_expression_simplify_to("f(3)", "unreal", User, Radian, Real);
  // Clean the storage for other tests
  Ion::Storage::sharedStorage()->recordNamed("f.func").destroy();


  // Cartesian
  assert_parsed_expression_simplify_to("-2.3ᴇ3", "-2300", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("3", "3", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("inf", "inf", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("1+2+𝐢", "3+𝐢", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("-(5+2×𝐢)", "-5-2×𝐢", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("(5+2×𝐢)", "5+2×𝐢", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("𝐢+𝐢", "2×𝐢", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("-2+2×𝐢", "-2+2×𝐢", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("(3+𝐢)-(2+4×𝐢)", "1-3×𝐢", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("(2+3×𝐢)×(4-2×𝐢)", "14+8×𝐢", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("(3+𝐢)/2", "3/2+1/2×𝐢", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("(3+𝐢)/(2+𝐢)", "7/5-1/5×𝐢", User, Radian, Cartesian);
  // The simplification of (3+𝐢)^(2+𝐢) in a Cartesian complex form generates to many nodes
  //assert_parsed_expression_simplify_to("(3+𝐢)^(2+𝐢)", "10×cos((-4×atan(3)+ln(2)+ln(5)+2×π)/2)×ℯ^((2×atan(3)-π)/2)+10×sin((-4×atan(3)+ln(2)+ln(5)+2×π)/2)×ℯ^((2×atan(3)-π)/2)𝐢", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("(3+𝐢)^(2+𝐢)", "(𝐢+3)^\u0012𝐢+2\u0013", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("√(1+6𝐢)", "√(2×√(37)+2)/2+√(2×√(37)-2)/2×𝐢", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("(1+𝐢)^2", "2×𝐢", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("2×𝐢", "2×𝐢", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("𝐢!", "𝐢!", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("3!", "6", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("x!", "x!", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("ℯ", "ℯ", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("π", "π", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("𝐢", "𝐢", User, Radian, Cartesian);

  assert_parsed_expression_simplify_to("abs(-3)", "3", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("abs(-3+𝐢)", "√(10)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("atan(2)", "atan(2)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("atan(2+𝐢)", "atan(2+𝐢)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("binomial(10, 4)", "210", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("ceil(-1.3)", "-1", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("arg(-2)", "π", User, Radian, Cartesian);
  // TODO: confidence is not simplified yet
  //assert_parsed_expression_simplify_to("confidence(-2,-3)", "confidence(-2)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("conj(-2)", "-2", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("conj(-2+2×𝐢+𝐢)", "-2-3×𝐢", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("cos(12)", "cos(12)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("cos(12+𝐢)", "cos(12+𝐢)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("diff(3×x, x, 3)", "diff(3×x,x,3)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("quo(34,x)", "quo(34,x)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("rem(5,3)", "2", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("floor(x)", "floor(x)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("frac(x)", "frac(x)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("gcd(x,y)", "gcd(x,y)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("im(1+𝐢)", "1", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("int(x^2, x, 1, 2)", "int(x^2,x,1,2)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("lcm(x,y)", "lcm(x,y)", User, Radian, Cartesian);
  // TODO: dim is not simplified yet
  //assert_parsed_expression_simplify_to("dim(x)", "dim(x)", User, Radian, Cartesian);

  assert_parsed_expression_simplify_to("root(2,𝐢)", "cos(ln(2))-sin(ln(2))×𝐢", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("root(2,𝐢+1)", "√(2)×cos(\u001290×ln(2)\u0013/π)-√(2)×sin(\u001290×ln(2)\u0013/π)×𝐢", User, Degree, Cartesian);
  assert_parsed_expression_simplify_to("root(2,𝐢+1)", "√(2)×cos(ln(2)/2)-√(2)×sin(ln(2)/2)×𝐢", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("permute(10, 4)", "5040", User, Radian, Cartesian);
  // TODO: prediction is not simplified yet
  //assert_parsed_expression_simplify_to("prediction(-2,-3)", "prediction(-2)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("randint(2,2)", "2", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("random()", "random()", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("re(x)", "re(x)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("round(x,y)", "round(x,y)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("sign(x)", "sign(x)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("sin(23)", "sin(23)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("sin(23+𝐢)", "sin(23+𝐢)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("√(1-𝐢)", "√(2×√(2)+2)/2-√(2×√(2)-2)/2×𝐢", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("tan(23)", "tan(23)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("tan(23+𝐢)", "tan(23+𝐢)", User, Radian, Cartesian);
  assert_parsed_expression_simplify_to("[[1,√(-1)]]", "[[1,𝐢]]", User, Radian, Cartesian);

  // User defined variable
  assert_parsed_expression_simplify_to("a", "a", User, Radian, Cartesian);
  // a = 2+i
  assert_simplify("2+𝐢→a", Radian, Cartesian);
  assert_parsed_expression_simplify_to("a", "2+𝐢", User, Radian, Cartesian);
  // Clean the storage for other tests
  Ion::Storage::sharedStorage()->recordNamed("a.exp").destroy();
  // User defined function
  assert_parsed_expression_simplify_to("f(3)", "f(3)", User, Radian, Cartesian);
  // f : x → x+1
  assert_simplify("x+1+𝐢→f(x)", Radian, Cartesian);
  assert_parsed_expression_simplify_to("f(3)", "4+𝐢", User, Radian, Cartesian);
  // Clean the storage for other tests
  Ion::Storage::sharedStorage()->recordNamed("f.func").destroy();

  // Polar
  assert_parsed_expression_simplify_to("-2.3ᴇ3", "2300×ℯ^\u0012π×𝐢\u0013", User, Radian, Polar);
  assert_parsed_expression_simplify_to("3", "3", User, Radian, Polar);
  assert_parsed_expression_simplify_to("inf", "inf", User, Radian, Polar);
  assert_parsed_expression_simplify_to("1+2+𝐢", "√(10)×ℯ^\u0012\u0012-2×atan(3)+π\u0013/2×𝐢\u0013", User, Radian, Polar);
  assert_parsed_expression_simplify_to("1+2+𝐢", "√(10)×ℯ^\u0012\u0012-π×atan(3)+90×π\u0013/180×𝐢\u0013", User, Degree, Polar);
  assert_parsed_expression_simplify_to("-(5+2×𝐢)", "√(29)×ℯ^\u0012\u0012-2×atan(5/2)-π\u0013/2×𝐢\u0013", User, Radian, Polar);
  assert_parsed_expression_simplify_to("(5+2×𝐢)", "√(29)×ℯ^\u0012\u0012-2×atan(5/2)+π\u0013/2×𝐢\u0013", User, Radian, Polar);
  assert_parsed_expression_simplify_to("𝐢+𝐢", "2×ℯ^\u0012π/2×𝐢\u0013", User, Radian, Polar);
  assert_parsed_expression_simplify_to("𝐢+𝐢", "2×ℯ^\u0012π/2×𝐢\u0013", User, Radian, Polar);
  assert_parsed_expression_simplify_to("-2+2×𝐢", "2×√(2)×ℯ^\u0012\u00123×π\u0013/4×𝐢\u0013", User, Radian, Polar);
  assert_parsed_expression_simplify_to("(3+𝐢)-(2+4×𝐢)", "√(10)×ℯ^\u0012\u00122×atan(1/3)-π\u0013/2×𝐢\u0013", User, Radian, Polar);
  assert_parsed_expression_simplify_to("(2+3×𝐢)×(4-2×𝐢)", "2×√(65)×ℯ^\u0012\u0012-2×atan(7/4)+π\u0013/2×𝐢\u0013", User, Radian, Polar);
  assert_parsed_expression_simplify_to("(3+𝐢)/2", "√(10)/2×ℯ^\u0012\u0012-2×atan(3)+π\u0013/2×𝐢\u0013", User, Radian, Polar);
  assert_parsed_expression_simplify_to("(3+𝐢)/(2+𝐢)", "√(2)×ℯ^\u0012\u00122×atan(7)-π\u0013/2×𝐢\u0013", User, Radian, Polar);
  // TODO: simplify atan(tan(x)) = x±k×pi?
  //assert_parsed_expression_simplify_to("(3+𝐢)^(2+𝐢)", "10ℯ^\u0012\u00122×atan(3)-π\u0013/2\u0013×ℯ^\u0012\u0012\u0012-4×atan(3)+ln(2)+ln(5)+2π\u0013/2\u0013𝐢\u0013", User, Radian, Polar);
  // The simplification of (3+𝐢)^(2+𝐢) in a Polar complex form generates to many nodes
  //assert_parsed_expression_simplify_to("(3+𝐢)^(2+𝐢)", "10ℯ^\u0012\u00122×atan(3)-π\u0013/2\u0013×ℯ^\u0012\u0012atan(tan((-4×atan(3)+ln(2)+ln(5)+2×π)/2))+π\u0013𝐢\u0013", User, Radian, Polar);
  assert_parsed_expression_simplify_to("(3+𝐢)^(2+𝐢)", "(𝐢+3)^\u0012𝐢+2\u0013", User, Radian, Polar);
  assert_parsed_expression_simplify_to("(1+𝐢)^2", "2×ℯ^\u0012π/2×𝐢\u0013", User, Radian, Polar);
  assert_parsed_expression_simplify_to("2×𝐢", "2×ℯ^\u0012π/2×𝐢\u0013", User, Radian, Polar);
  assert_parsed_expression_simplify_to("3!", "6", User, Radian, Polar);
  assert_parsed_expression_simplify_to("x!", "x!", User, Radian, Polar);
  assert_parsed_expression_simplify_to("ℯ", "ℯ", User, Radian, Polar);
  assert_parsed_expression_simplify_to("π", "π", User, Radian, Polar);
  assert_parsed_expression_simplify_to("𝐢", "ℯ^\u0012π/2×𝐢\u0013", User, Radian, Polar);
  assert_parsed_expression_simplify_to("abs(-3)", "3", User, Radian, Polar);
  assert_parsed_expression_simplify_to("abs(-3+𝐢)", "√(10)", User, Radian, Polar);
  assert_parsed_expression_simplify_to("conj(2×ℯ^(𝐢×π/2))", "2×ℯ^\u0012-π/2×𝐢\u0013", User, Radian, Polar);
  assert_parsed_expression_simplify_to("-2×ℯ^(𝐢×π/2)", "2×ℯ^\u0012-π/2×𝐢\u0013", User, Radian, Polar);
  assert_parsed_expression_simplify_to("[[1,√(-1)]]", "[[1,ℯ^\u0012π/2×𝐢\u0013]]", User, Radian, Polar);

  // User defined variable
  assert_parsed_expression_simplify_to("a", "a", User, Radian, Polar);
  // a = 2 + 𝐢
  assert_simplify("2+𝐢→a", Radian, Polar);
  assert_parsed_expression_simplify_to("a", "√(5)×ℯ^\u0012\u0012-2×atan(2)+π\u0013/2×𝐢\u0013", User, Radian, Polar);
  // Clean the storage for other tests
  Ion::Storage::sharedStorage()->recordNamed("a.exp").destroy();
  // User defined function
  assert_parsed_expression_simplify_to("f(3)", "f(3)", User, Radian, Polar);
  // f: x → x+1
  assert_simplify("x+1+𝐢→f(x)", Radian, Polar);
  assert_parsed_expression_simplify_to("f(3)", "√(17)×ℯ^\u0012\u0012-2×atan(4)+π\u0013/2×𝐢\u0013", User, Radian, Polar);
  // Clean the storage for other tests
  Ion::Storage::sharedStorage()->recordNamed("f.func").destroy();
}

QUIZ_CASE(poincare_simplification_reduction_target) {
  assert_parsed_expression_simplify_to("1/π+1/x", "1/x+1/π", System);
  assert_parsed_expression_simplify_to("1/π+1/x", "\u0012x+π\u0013/\u0012π×x\u0013", User);

  assert_parsed_expression_simplify_to("1/(1+𝐢)", "1/\u0012𝐢+1\u0013", System);
  assert_parsed_expression_simplify_to("1/(1+𝐢)", "1/2-1/2×𝐢", User);

  assert_parsed_expression_simplify_to("sin(x)/(cos(x)×cos(x))", "sin(x)/cos(x)^2", System);
  assert_parsed_expression_simplify_to("sin(x)/(cos(x)×cos(x))", "tan(x)/cos(x)", User);

  assert_parsed_expression_simplify_to("x^0", "x^0", System);
  assert_parsed_expression_simplify_to("x^0", "1", User);

  assert_parsed_expression_simplify_to("x^(2/3)", "root(x,3)^2", System);
  assert_parsed_expression_simplify_to("x^(2/3)", "x^\u00122/3\u0013", User);
  assert_parsed_expression_simplify_to("x^(1/3)", "root(x,3)", System);
  assert_parsed_expression_simplify_to("x^(1/3)", "root(x,3)", System);
  assert_parsed_expression_simplify_to("x^2", "x^2", System);
  assert_parsed_expression_simplify_to("x^2", "x^2", User);

  assert_parsed_expression_simplify_to("1/(√(2)+√(3))", "1/\u0012√(3)+√(2)\u0013", System);
  assert_parsed_expression_simplify_to("1/(√(2)+√(3))", "√(3)-√(2)", User);

  assert_parsed_expression_simplify_to("sign(abs(x))", "sign(abs(x))", System);
  assert_parsed_expression_simplify_to("sign(abs(x))", "1", User);

  assert_parsed_expression_simplify_to("atan(1/x)", "atan(1/x)", System);
  assert_parsed_expression_simplify_to("atan(1/x)", "\u0012π×sign(x)-2×atan(x)\u0013/2", User);

  assert_parsed_expression_simplify_to("(1+x)/(1+x)", "(x+1)^0", System);
  assert_parsed_expression_simplify_to("(1+x)/(1+x)", "1", User);
}

QUIZ_CASE(poincare_simplification_mix) {
  // Root at denominator
  assert_parsed_expression_simplify_to("1/(√(2)+√(3))", "√(3)-√(2)");
  assert_parsed_expression_simplify_to("1/(5+√(3))", "\u0012-√(3)+5\u0013/22");
  assert_parsed_expression_simplify_to("1/(√(2)+4)", "\u0012-√(2)+4\u0013/14");
  assert_parsed_expression_simplify_to("1/(2√(2)-4)", "\u0012-√(2)-2\u0013/4");
  assert_parsed_expression_simplify_to("1/(-2√(2)+4)", "\u0012√(2)+2\u0013/4");
  assert_parsed_expression_simplify_to("45^2", "2025");
  assert_parsed_expression_simplify_to("1/(√(2)ln(3))", "√(2)/\u00122×ln(3)\u0013");
  assert_parsed_expression_simplify_to("√(3/2)", "√(6)/2");

  // Common operation mix
  assert_parsed_expression_simplify_to("(√(2)×π + √(2)×ℯ)/√(2)", "ℯ+π");
  assert_parsed_expression_simplify_to("π+(3√(2)-2√(3))/25", "\u001225×π-2×√(3)+3×√(2)\u0013/25");
  assert_parsed_expression_simplify_to("ln(2+3)", "ln(5)");
  assert_parsed_expression_simplify_to("3×A×B×C+4×cos(2)-2×A×B×C+A×B×C+ln(3)+4×A×B-5×A×B×C+cos(3)×ln(5)+cos(2)-45×cos(2)", "cos(3)×ln(5)+ln(3)-40×cos(2)+4×A×B-3×A×B×C");
  assert_parsed_expression_simplify_to("2×A+3×cos(2)+3+4×ln(5)+5×A+2×ln(5)+1+0", "6×ln(5)+3×cos(2)+7×A+4");
  assert_parsed_expression_simplify_to("2.3×A+3×cos(2)+3+4×ln(5)+5×A+2×ln(5)+1.2+0.235", "\u00121200×ln(5)+600×cos(2)+1460×A+887\u0013/200");
  assert_parsed_expression_simplify_to("2×A×B×C+2.3×A×B+3×A^2+5.2×A×B×C+4×A^2", "\u001270×A^2+23×A×B+72×A×B×C\u0013/10");
  assert_parsed_expression_simplify_to("(A×B)^2×A+4×A^3", "4×A^3+A^3×B^2");
  assert_parsed_expression_simplify_to("(A×3)^2×A+4×A^3", "13×A^3");
  assert_parsed_expression_simplify_to("A^2^2×A+4×A^3", "A^5+4×A^3");
  assert_parsed_expression_simplify_to("0.5+4×A×B-2.3+2×A×B-2×B×A^C-cos(4)+2×A^C×B+A×B×C×D", "\u0012-5×cos(4)+30×A×B+5×A×B×C×D-9\u0013/5");
  assert_parsed_expression_simplify_to("(1+√(2))/5", "\u0012√(2)+1\u0013/5");
  assert_parsed_expression_simplify_to("(2+√(6))^2", "4×√(6)+10");
  assert_parsed_expression_simplify_to("tan(3)ln(2)+π", "tan(3)×ln(2)+π");
  assert_parsed_expression_simplify_to("abs(dim(2))", "[[1,1]]");

  // Complex
  assert_parsed_expression_simplify_to("𝐢", "𝐢");
  assert_parsed_expression_simplify_to("√(-33)", "√(33)×𝐢");
  assert_parsed_expression_simplify_to("𝐢^(3/5)", "\u0012√(2)×√(-√(5)+5)\u0013/4+\u0012√(5)+1\u0013/4×𝐢");
  assert_parsed_expression_simplify_to("𝐢𝐢𝐢𝐢", "1");
  assert_parsed_expression_simplify_to("√(-𝐢)", "√(2)/2-√(2)/2×𝐢");
  assert_parsed_expression_simplify_to("A×cos(9)𝐢𝐢ln(2)", "-A×cos(9)×ln(2)");
  assert_parsed_expression_simplify_to("(√(2)+√(2)×𝐢)/2(√(2)+√(2)×𝐢)/2(√(2)+√(2)×𝐢)/2", "√(2)/32-√(2)/32×𝐢");
  assert_parsed_expression_simplify_to("root(5^((-𝐢)3^9),𝐢)", "1/ℯ^atan(tan(19683×ln(5)))");
  assert_parsed_expression_simplify_to("𝐢^𝐢", "1/ℯ^\u0012π/2\u0013");
  assert_parsed_expression_simplify_to("𝐢/(1+𝐢×√(x))", "𝐢/\u0012√(x)×𝐢+1\u0013");
  assert_parsed_expression_simplify_to("x+𝐢/(1+𝐢×√(x))", "\u0012x^\u00123/2\u0013×𝐢+𝐢+x\u0013/\u0012√(x)×𝐢+1\u0013");

  //assert_parsed_expression_simplify_to("log(cos(9)^ln(6), cos(9))", "ln(2)+ln(3)"); // TODO: for this to work, we must know the sign of cos(9)
  //assert_parsed_expression_simplify_to("log(cos(9)^ln(6), 9)", "ln(6)×log(cos(9), 9)"); // TODO: for this to work, we must know the sign of cos(9)
  assert_parsed_expression_simplify_to("(((√(6)-√(2))/4)/((√(6)+√(2))/4))+1", "-√(3)+3");
  assert_parsed_expression_simplify_to("1/√(𝐢) × (√(2)-𝐢×√(2))", "-2×𝐢"); // TODO: get rid of complex at denominator?
}

QUIZ_CASE(poincare_hyperbolic_trigonometry) {
  // acosh(cosh)
  assert_parsed_expression_simplify_to("acosh(cosh(3))", "3");
  assert_parsed_expression_simplify_to("acosh(cosh(0.5))", "1/2");
  assert_parsed_expression_simplify_to("acosh(cosh(-3))", "3");
  assert_parsed_expression_simplify_to("acosh(cosh(3))", "3", User, Radian, Real);
  assert_parsed_expression_simplify_to("acosh(cosh(0.5))", "1/2", User, Radian, Real);
  assert_parsed_expression_simplify_to("acosh(cosh(-3))", "3", User, Radian, Real);

  // cosh(acosh)
  assert_parsed_expression_simplify_to("cosh(acosh(3))", "3");
  assert_parsed_expression_simplify_to("cosh(acosh(0.5))", "1/2");
  assert_parsed_expression_simplify_to("cosh(acosh(-3))", "-3");
  assert_parsed_expression_simplify_to("cosh(acosh(3))", "3", User, Radian, Real);
  assert_parsed_expression_simplify_to("cosh(acosh(0.5))", "cosh(acosh(1/2))", User, Radian, Real);
  assert_parsed_expression_simplify_to("cosh(acosh(-3))", "cosh(acosh(-3))", User, Radian, Real);

  // sinh(asinh)
  assert_parsed_expression_simplify_to("sinh(asinh(3))", "3");
  assert_parsed_expression_simplify_to("sinh(asinh(0.5))", "1/2");
  assert_parsed_expression_simplify_to("sinh(asinh(-3))", "-3");
  assert_parsed_expression_simplify_to("sinh(asinh(3))", "3", User, Radian, Real);
  assert_parsed_expression_simplify_to("sinh(asinh(0.5))", "1/2", User, Radian, Real);
  assert_parsed_expression_simplify_to("sinh(asinh(-3))", "-3", User, Radian, Real);

  // asinh(sinh)
  assert_parsed_expression_simplify_to("asinh(sinh(3))", "3");
  assert_parsed_expression_simplify_to("asinh(sinh(0.5))", "1/2");
  assert_parsed_expression_simplify_to("asinh(sinh(-3))", "-3");
  assert_parsed_expression_simplify_to("asinh(sinh(3))", "3", User, Radian, Real);
  assert_parsed_expression_simplify_to("asinh(sinh(0.5))", "1/2", User, Radian, Real);
  assert_parsed_expression_simplify_to("asinh(sinh(-3))", "-3", User, Radian, Real);

  // tanh(atanh)
  assert_parsed_expression_simplify_to("tanh(atanh(3))", "3");
  assert_parsed_expression_simplify_to("tanh(atanh(0.5))", "1/2");
  assert_parsed_expression_simplify_to("tanh(atanh(-3))", "-3");
  assert_parsed_expression_simplify_to("tanh(atanh(3))", "tanh(atanh(3))", User, Radian, Real);
  assert_parsed_expression_simplify_to("tanh(atanh(0.5))", "1/2", User, Radian, Real);
  assert_parsed_expression_simplify_to("tanh(atanh(-3))", "-tanh(atanh(3))", User, Radian, Real);

  // atanh(tanh)
  assert_parsed_expression_simplify_to("atanh(tanh(3))", "3");
  assert_parsed_expression_simplify_to("atanh(tanh(0.5))", "1/2");
  assert_parsed_expression_simplify_to("atanh(tanh(-3))", "-3");
  assert_parsed_expression_simplify_to("atanh(tanh(3))", "3", User, Radian, Real);
  assert_parsed_expression_simplify_to("atanh(tanh(0.5))", "1/2", User, Radian, Real);
  assert_parsed_expression_simplify_to("atanh(tanh(-3))", "-3", User, Radian, Real);
}

QUIZ_CASE(poincare_probability) {
  assert_parsed_expression_simplify_to("invnorm(-1.3,2,3)", Undefined::Name());
  assert_parsed_expression_simplify_to("invnorm(0,2,3)", "-inf");
  assert_parsed_expression_simplify_to("invnorm(0.5,2,3)", "2");
  assert_parsed_expression_simplify_to("invnorm(1,2,3)", "inf");
  assert_parsed_expression_simplify_to("invnorm(1.3,2,3)", "undef");
  assert_parsed_expression_simplify_to("invnorm(3/4,2,random())", "invnorm(3/4,2,random())"); // random can be 0
  assert_parsed_expression_simplify_to("invnorm(0.5,2,0)", Undefined::Name());
  assert_parsed_expression_simplify_to("invnorm(0.5,2,-1)", Undefined::Name());
  assert_parsed_expression_simplify_to("normcdf(2,0,1)", "normcdf(2,0,1)");
  assert_parsed_expression_simplify_to("normcdf2(1,2,0,1)", "normcdf2(1,2,0,1)");
  assert_parsed_expression_simplify_to("normpdf(2,0,1)", "normpdf(2,0,1)");
}
