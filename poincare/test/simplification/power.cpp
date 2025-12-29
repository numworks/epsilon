#include "helper.h"

QUIZ_CASE(pcj_simplification_power) {
  simplifies_to("3^4", "81");
  simplifies_to("3^(-4)", "1/81");
  simplifies_to("(-3)^3", "-27");
  simplifies_to("1256^(1/3)×x", "2×x×root(157,3)");
  simplifies_to("1256^(-1/3)", "157^(2/3)/314");
  simplifies_to("2^(64)", "18446744073709551616");
  simplifies_to("2^(64)/2^(63)", "2");
  simplifies_to("1-e^(-(0.09/(5.63*10^-7)))", "1-e^(-90000000/563)");
  simplifies_to("(100/3)^(1/3)", "30^(2/3)/3");
  simplifies_to("(200/3)^(1/3)", "(2×15^(2/3))/3");
  simplifies_to("ln((2-√(3))^10)+ln((2+√(3))^10)", "0");
  simplifies_to("32^(-1/5)", "1/2");
  simplifies_to("4^0.5", "2");
  simplifies_to("8^0.5", "2×√(2)");
  simplifies_to("(12^4×3)^(0.5)", "144×√(3)");
  simplifies_to("(π^3)^4", "π^12");
  simplifies_to("2^(2+π)", "4×2^π");
  simplifies_to("π×π×π", "π^3");
  simplifies_to("1.0066666666667^60", "(10066666666667/10^13)^60");
  simplifies_to("i^(2/3)", "1/2+√(3)/2×i", k_cartesianCtx);
  simplifies_to("e^(i×π/3)", "1/2+√(3)/2×i", k_cartesianCtx);
  simplifies_to("((-e^3)×π)^(1/3)", "e×root(π,3)×e^(π/3×i)", k_polarCtx);
  // TODO: find the sign of cos(2) to simplify
  simplifies_to("((-e^3)×cos(2))^(1/3)", "root(-cos(2)×e^(3),3)", k_polarCtx);
  simplifies_to("(-1)^2", "1");
  simplifies_to("(-1)^3", "-1");
  simplifies_to("(-1)^2006", "1");
  simplifies_to("(-1)^2007", "-1");
  simplifies_to("(-2)^2006", "(-2)^2006");
  simplifies_to("(-2)^2007", "(-2)^2007");

  // Factorization
  simplifies_to("999^(10000/3)", "3×999^3333×root(37,3)");
  simplifies_to("root(2^6*3^24*5^9*7^3,12)", "9×√(2)×e^((3×ln(5))/4+ln(7)/4)");
  simplifies_to("130972680290304^(1/3)", "50784");
  /* This does not reduce but should not as the prime decomposition involves too
   * many or too large factors. */
  simplifies_to("1002101470343^(1/3)", "root(1002101470343,3)");
  simplifies_to("1881676377434183981909562699940347954480361860897069^(1/3)",
                "root(1881676377434183981909562699940347954480361860897069,3)");

  // Principal angle of root of unity
  simplifies_to("(-5)^(-1/3)", "root(25,3)/10-(√(3)×root(25,3))/10×i",
                k_cartesianCtx);

  // Zero
  simplifies_to("0^3", "0");
  simplifies_to("0^0", "undef");
  simplifies_to("0^(-3)", "undef");
  simplifies_to("0^3.1", "0");
  simplifies_to("0^(-4.2)", "undef");
  simplifies_to("0^(1+x^2)", "0");
  simplifies_to("0^i", "undef");
  simplifies_to("0^(-5+ln(5))", "undef");
  simplifies_to("0^(5+ln(5))", "0");

  // With variables
  simplifies_to("1/a", "1/a");
  simplifies_to("1/(1/a)", "dep(a,{nonNull(a)})");
  simplifies_to("1/(a^-3)", "dep(a^3,{nonNull(a)})");
  simplifies_to("a×a^(-1)", "dep(1,{a^0})");
  simplifies_to("a×a^(1+1)", "a^3");
  simplifies_to("2×a^1×(2a)^(-1)", "dep(1,{a^0})");
  simplifies_to("cos(π×a×a^(-1))^(b×b^(-2)×b)", "dep(-1,{a^0,b^0})");
  simplifies_to("(cos(x)^2+sin(x)^2-1)^π", "0", k_cartesianCtx);
  simplifies_to("2^(6+π+x)", "64×2^(x+π)");
  simplifies_to("(2+3-4)^(x)", "1");
  simplifies_to("(12^4×x)^(0.5)", "144×√(x)", k_cartesianCtx);
  simplifies_to("1^x", "1");
  simplifies_to("x^1", "x");
  simplifies_to("x^(1/2)", "√(x)");
  simplifies_to("x^(-1/2)", "1/√(x)");
  simplifies_to("x^(1/7)", "root(x,7)");
  simplifies_to("x^(-1/7)", "1/root(x,7)");
  simplifies_to("(P×Q)^3", "P^3×Q^3");

  // Expand/Contract
  simplifies_to("e^(ln(2)+π)", "2e^π");
  simplifies_to("√(12)-2×√(3)", "0");
  simplifies_to("3^(1/3)×41^(1/3)-123^(1/3)", "0");
  simplifies_to("√(2)*√(7)-√(14)", "0");
  simplifies_to("x^(1-y^0)", "dep(1,{x^0,y^0})");
  simplifies_to("i^5+i^10+i^15+i^20", "0");

  // Development of mult and integer power
  simplifies_to("π*(π+1)", "π^2+π");
  simplifies_to("(π+1)^2", "π^2+1+2×π");
  simplifies_to("(π-1)^2", "π^2+1-2×π");
  simplifies_to("(π+1)^3", "π^3+3×π^2+1+3×π");
  simplifies_to("(π+1)^(-2)", "1/(π^2+1+2×π)");
#if TODO_PCJ  // Develop
  simplifies_to("(x+π)^(3)", "x^3+3×π×x^2+3×π^2×x+π^3");
#endif

  // √(a^2±2ab+b^2)
  simplifies_to("√((π+1)^2)", "1+π");
  simplifies_to("√((π-1)^2)", "-1+π");
  simplifies_to("√(1/(ln(2)^2-2πln(2)+π^2))", "-1/(-π+ln(2))");

  // Assert this does not crash
  simplifies_to("90000000005^(x+0.5)", "90000000005^(x+1/2)");
  simplifies_to("(-123456789012345)^3",
                "-1881676372353626729966819028056573540963625");
}

QUIZ_CASE(pcj_simplification_power_real_complex) {
  // Real powers
  simplifies_to("√(x)^2", "√(x)^2");
  // - x^y if x is complex or positive
  simplifies_to("41^(1/3)", "root(41,3)");
  // - PowerReal(x,y) y is not a rational
  simplifies_to("x^(e^3)", "x^e^3");
  simplifies_to("(x^e)^3", "(x^e)^3");
  // - Looking at y's reduced rational form p/q :
  //   * PowerReal(x,y) if x is of unknown sign and p odd
  simplifies_to("x^(1/3)", "root(x,3)");
  //   * Unreal if q is even and x negative
  simplifies_to("(-1)^(1/2)", "nonreal");
  //   * |x|^y if p is even
  simplifies_to("(-41)^(4/5)", "41^(4/5)");
  //   * -|x|^y if p is odd
  simplifies_to("(-41)^(5/7)", "-(41^(5/7))");
  // Do not merge PowReal if the exponents are unknown
  simplifies_to("(-1)^y×(-1)^y", "((-1)^y)^2");
  simplifies_to("x^(1/3)×x^(1/3)×x^(1/3)", "root(x,3)^3");

  // Real vs complex
  simplifies_to("(x^2)^3", "x^6");
  simplifies_to("(x^2)^3", "x^6", k_cartesianCtx);
  simplifies_to("(x^(2/3))^3", "x^2");
  simplifies_to("(x^(2/3))^3", "x^2", k_cartesianCtx);
  simplifies_to("(x^(3/4))^4", "(x^(3/4))^4");
  simplifies_to("(x^(3/4))^4", "x^3", k_cartesianCtx);
  simplifies_to("√(-x)", "√(-x)");
  simplifies_to("√(x)^2", "√(x)^2");
  simplifies_to("√(x)^2", "x", k_cartesianCtx);
  simplifies_to("√(-3)^2", "nonreal");
  simplifies_to("(-0.0001)^6.3", "nonreal");

  // Complex Power
  simplifies_to("√(x)^2", "x", k_cartesianCtx);
  /* TODO: Should be 0, (exp(i*(arg(A) + arg(B) - arg(A*B))) should be
   * simplified to 1 */
  simplifies_to("√(-i-1)*√(-i+1)+√((-i-1)*(-i+1))", "√(-1-i)×√(1-i)+√(2)×i",
                k_cartesianCtx);
}

QUIZ_CASE(pcj_simplification_roots) {
  // Square roots
  simplifies_to("√(9)", "3");
  simplifies_to("√(865)", "√(865)");
  simplifies_to("√(865)/6", "√(865)/6");
  simplifies_to("√(30)", "√(30)");
  simplifies_to("√(32)", "4×√(2)");
  simplifies_to("√(-1)", "i", k_cartesianCtx);
  simplifies_to("√(-1×√(-1))", "√(2)/2-√(2)/2×i", k_cartesianCtx);
  simplifies_to("√(3^2)", "3");
  simplifies_to("√(-9)", "3×i", k_cartesianCtx);
  simplifies_to("√(i)", "√(2)/2+√(2)/2×i", k_cartesianCtx);
  simplifies_to("√(-i)", "√(2)/2-√(2)/2×i", k_cartesianCtx);
  simplifies_to("√(2eπ)*√(2eπ)", "2×π×e");
  simplifies_to("(-1)×(2+(-4×√(2)))", "-2+4×√(2)");
  simplifies_to("2/√(2)", "√(2)");
  simplifies_to("4/√(2)", "2*√(2)");
  simplifies_to("1/√(2)", "√(2)/2");
  simplifies_to("√(2)/2", "√(2)/2");
  simplifies_to("√(-12)/2", "√(3)×i", k_cartesianCtx);
  simplifies_to("-2+√(-12)/2", "-2+√(3)×i", k_cartesianCtx);
  simplifies_to("√(-5)*√(-3)", "-√(15)", k_cartesianCtx);
  simplifies_to("√(-5)*√(-5)", "-5", k_cartesianCtx);
  simplifies_to("1/√(2+√(3))", "(-√(2)+√(6))/2");
  simplifies_to("1/√(-2×√(3)+3×√(2))", "√(√(2)/2+√(3)/3)");
  simplifies_to("1/√(-4+√(17))", "√(4+√(17))");
  simplifies_to("1/√(-3+√(19))", "√((3+√(19))/10)");
  simplifies_to("√(5513219850886344455940081)", "2348024669991");
  simplifies_to("√(154355776)", "12424");
  simplifies_to("√(π)^2", "π");
  simplifies_to("√(π^2)", "π");
  simplifies_to("√((-π)^2)", "π");
  simplifies_to("√(e^2)", "e");
  simplifies_to("1/(3√(2))", "√(2)/6");
  simplifies_to("(5+√(2))^(-8)", "(1446241-1003320×√(2))/78310985281");
  simplifies_to("(5×π+√(2))^(-5)", "1/(5×π+√(2))^5");
  simplifies_to("(1+√(2))^5", "41+29×√(2)");
  simplifies_to("(π+√(3)+x)^(-2)", "1/(x^2+π^2+3+2×(x×√(3)+π×(x+√(3))))");
  simplifies_to("√(3.3×10^(-37))", "√(33)/10^19");
  simplifies_to("√(3.3×10^(-38))", "√(330)/10^20");
  simplifies_to("√(3.3×10^(-39))", "√(33)/10^20");
  simplifies_to("(√(2)^√(2))^√(2)", "2");
  simplifies_to("√(-arcsin(-e-1))", "√(arcsin(1+e))", k_cartesianCtx);

  // With variables
  simplifies_to("√(x×144)", "12×√(x)", k_cartesianCtx);
  simplifies_to("√(x×144×π)", "12×√(π×x)", k_cartesianCtx);
  simplifies_to("√(x×π^2)", "π×√(x)", k_cartesianCtx);
  // TODO: should reduce to 12×π×√(x) (works with +1 advanced reduction depth)
  simplifies_to("√(x×144×π^2)", "12×√(π^2×x)", k_cartesianCtx);

  // Root
  simplifies_to("root(-8,3)", "-2");
  simplifies_to("root(48,3)", "2*root(6,3)");
  simplifies_to("root(81,3)", "3*root(3,3)");
  simplifies_to("2*root(81,3)", "6*root(3,3)");
  simplifies_to("7*root(81,3)", "21*root(3,3)");
  simplifies_to("21*root(3,3)", "21*root(3,3)");
  simplifies_to("root(4,4)", "√(2)");
  simplifies_to("root(4,3)", "root(4,3)");
  simplifies_to("root(4,π)", "2^(2/π)");
  simplifies_to("root(27,3)", "3");
  simplifies_to("root(4^4,3)", "4*root(4,3)");
  simplifies_to("root(5^4,3)", "5*root(5,3)");
  simplifies_to("root(8/9,3)", "(2×root(3,3))/3");
  simplifies_to("√(2)×root(8,4)", "2×root(2,4)");
}

QUIZ_CASE(pcj_simplification_nested_radicals) {
  // Denesting of square roots
  simplifies_to("√(2+√(3))", "(√(2)+√(6))/2");
  simplifies_to("√(3-√(7))", "√(3-√(7))");
  simplifies_to("√(-2+√(3))", "(-√(2)+√(6))/2×i", k_cartesianCtx);
  simplifies_to("√(-3-√(8))", "(1+√(2))×i", k_cartesianCtx);
  simplifies_to("√(17+4×√(13))", "2+√(13)");
  simplifies_to("√(√(1058)-√(896))", "root(2,4)×(4-√(7))", k_cartesianCtx);
  simplifies_to("√(57×√(17)+68×√(10))", "17^(3/4)×(1+(2×√(170))/17)");
  simplifies_to("(-8)^(1/3)-1-√(3)×i", "0", k_cartesianCtx);
  simplifies_to("√(-3)-√(3)×i", "0", k_cartesianCtx);
  simplifies_to("(√(2+√(3))+√(2-√(3))×i)^2", "2×√(3)+2i", k_cartesianCtx);
  simplifies_to("√(2+√(4.5))", "root(2,4)+2^(3/4)/2", k_cartesianCtx);
  simplifies_to("√(2-√(4.5))", "(root(2,4)-2^(3/4)/2)×i", k_cartesianCtx);
  simplifies_to("arctan(√(((((800^2)^2)^2)^2)^2√(8)))",
                "arctan(2.81474976710656×10^46×2^(3/4))");
}

QUIZ_CASE(pcj_simplification_logarithm) {
  // Big numbers in logarithms
  simplifies_to("log(10^2022)", "2022");
  simplifies_to("3/log(3000000000)", "3×(log(2,3000000000)+log(5,3000000000))");

  simplifies_to("ln(i)", "π/2×i", k_cartesianCtx);
  simplifies_to("π×ln(2)+ln(4)", "(2+π)×ln(2)");
  simplifies_to("ln(6)", "ln(2)+ln(3)");
  simplifies_to("log(6)", "log(2)+log(3)");
  simplifies_to("ln(6^2)", "2×ln(2)+2×ln(3)");
  simplifies_to("ln(1/999999)", "-3×ln(3)-ln(7)-ln(11)-ln(13)-ln(37)");
  simplifies_to("300×ln(2)", "300×ln(2)");
  simplifies_to("ln(ln(25))", "ln(2)+ln(ln(5))");
  simplifies_to("log(log(25))", "log(log(5))+log(2)");
  simplifies_to("ln(5/2)", "-ln(2)+ln(5)");
  simplifies_to("log(2/7)", "log(2)-log(7)");
  simplifies_to("1+ln(x)+ln(y)",
                "dep(1+ln(x)+ln(y),{nonNull(x),nonNull(y),realPos(x),"
                "realPos(y)})");
  simplifies_to("ln(π)-ln(1/π)", "2×ln(π)");
  simplifies_to("cos(x)^2+sin(x)^2-ln(x)",
                "dep(1-ln(x),{nonNull(x),realPos(x)})");
  simplifies_to("1-ln(x)", "dep(1-ln(x),{nonNull(x)})", k_cartesianCtx);
  simplifies_to("ln(0)", "undef");
  simplifies_to("ln(0)", "undef", k_cartesianCtx);
  simplifies_to("ln(0^Z)×5", "undef", k_keepAllSymbolsCtx);
  simplifies_to("ln(cos(x)^2+sin(x)^2)", "dep(0,{nonNull(cos(x)^2+sin(x)^2)})");
  simplifies_to("ln(cos(x)^2+sin(x)^2-1)", "undef");
  simplifies_to("ln(-10)-ln(5)", "ln(2)+π×i", k_cartesianCtx);
  simplifies_to("im(ln(-120))", "π", k_cartesianCtx);
  simplifies_to("ln(-1-i)+ln(-1+i)", "ln(2)", k_cartesianCtx);
  simplifies_to("im(ln(i-2)+ln(i-1))-2π", "im(ln(1-3×i))", k_cartesianCtx);
  simplifies_to("ln(x)+ln(y)-ln(x×y)",
                "dep(ln(x)+ln(y)-ln(x×y),{nonNull(x),nonNull(y)})",
                k_cartesianCtx);
  simplifies_to(
      "ln(abs(x))+ln(abs(y))-ln(abs(x)×abs(y))",
      "dep(0,{0×ln(abs(x)),0×ln(abs(y)),nonNull(abs(x)),nonNull(abs(y))})",
      k_cartesianCtx);
  simplifies_to("log(14142135623731/5000000000000)",
                "log(14142135623731/5000000000000)");
  simplifies_to(
      "-ln(arccos(x))-ln(arcsin(x))-ln(arctan(x))-ln("
      "arccot(x))-ln(arcsec(x))-ln(arccsc(x))",
      "dep(-ln(arccos(x))-ln(arccos(1/x))-ln(arcsin(x))-ln(arcsin(1/"
      "x))-ln(arctan(x))-ln(π/2-arctan(x)),{nonNull(arccos(1/"
      "x)),nonNull(arccos(x)),nonNull(arcsin(1/"
      "x)),nonNull(arcsin(x)),nonNull(arctan(x)),nonNull(arccos(0)-arctan(x))}"
      ")",
      k_cartesianCtx);

  // Use complex logarithm internally
  simplifies_to("√(x^2)", "√(x^2)", k_cartesianCtx);
  simplifies_to("√(abs(x)^2)", "abs(x)", k_cartesianCtx);
  simplifies_to("√(0)", "0", k_cartesianCtx);
  simplifies_to("√(cos(x)^2+sin(x)^2-1)", "0", k_cartesianCtx);

  // Simplification with exponential
  simplifies_to("e^ln(3)", "3");
  simplifies_to("e^ln(√(3))", "√(3)");
  simplifies_to("π^log(√(3),π)", "√(3)");
  simplifies_to("10^log(π)", "π");
  simplifies_to("10^log(4)", "4");
  simplifies_to("10^(3*log(4))", "64");
  simplifies_to("10^(3*log(π))", "π^3");
  simplifies_to("10^(π*log(3))", "3^π");
  simplifies_to("10^(log(12)+log(10))", "120");
  simplifies_to("10^(log(π)+log(10))", "10×π");
  simplifies_to("10^(3*log(5)+log(4))", "500");
  simplifies_to("10^(3*log(π)+log(4))", "4×π^3");
  simplifies_to("10^(3+log(4))", "4000");
  simplifies_to("10^(3+2*log(4))", "16000");
  simplifies_to("10^(3+2*log(π))", "1000×π^2");
  simplifies_to("e^ln(65)", "65");
  simplifies_to("e^(2ln(3))", "9");
  simplifies_to("e^ln(πe)", "π×e");
  simplifies_to("e^ln(π)", "π");
  simplifies_to("e^(ln(x))", "dep(x,{nonNull(x)})", k_cartesianCtx);
  simplifies_to("ln(e^x)", "x", k_cartesianCtx);
  simplifies_to("ln(e^(i×π))", "π×i", k_cartesianCtx);
  simplifies_to("ln(e^(-i×π))", "π×i", k_cartesianCtx);
  simplifies_to("ln(e^(i×2×π))", "0", k_cartesianCtx);
  // TODO: should simplify as ln(10) instead of ln(2)+ln(5)
  simplifies_to("e^log(πe)", "e^(log(π)+1/(ln(2)+ln(5)))");

  // Log with base
  simplifies_to("log(3,27)", "1/3");
  simplifies_to("log(27,3)", "3");
  simplifies_to("10^log(1.23)", "123/100");
  simplifies_to("2^log(3,2)", "3");
  simplifies_to("log(9,7)", "2×log(3,7)");
  simplifies_to("log(9,8)", "2×log(3,8)");
  simplifies_to("log(3,7)+log(5,7)", "log(3,7)+log(5,7)");
  simplifies_to("log(3,8)+log(5,8)", "log(3,8)+log(5,8)");
}
