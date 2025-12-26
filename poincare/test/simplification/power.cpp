#include "helper.h"

QUIZ_CASE(pcj_simplification_power) {
  simplifies_to("2^(64)", "18446744073709551616");
  simplifies_to("2^(64)/2^(63)", "2");
  simplifies_to("(cos(x)^2+sin(x)^2-1)^π", "0", k_cartesianCtx);
  simplifies_to("1-e^(-(0.09/(5.63*10^-7)))", "1-e^(-90000000/563)");
  simplifies_to("(100/3)^(1/3)", "30^(2/3)/3");
  simplifies_to("(200/3)^(1/3)", "(2×15^(2/3))/3");
  simplifies_to("ln((2-√(3))^10)+ln((2+√(3))^10)", "0");

  // Zero
  simplifies_to("0^3.1", "0");
  simplifies_to("0^(-4.2)", "undef");
  simplifies_to("0^(1+x^2)", "0");

  // With variables
  simplifies_to("1/a", "1/a");
  simplifies_to("1/(1/a)", "dep(a,{nonNull(a)})");
  simplifies_to("1/(a^-3)", "dep(a^3,{nonNull(a)})");
  simplifies_to("a×a^(-1)", "dep(1,{a^0})");
  simplifies_to("a×a^(1+1)", "a^3");
  simplifies_to("2×a^1×(2a)^(-1)", "dep(1,{a^0})");
  simplifies_to("cos(π×a×a^(-1))^(b×b^(-2)×b)", "dep(-1,{a^0,b^0})");

  // Complex Power
  simplifies_to("√(x)^2", "x", k_cartesianCtx);
  /* TODO: Should be 0, (exp(i*(arg(A) + arg(B) - arg(A*B))) should be
   * simplified to 1 */
  simplifies_to("√(-i-1)*√(-i+1)+√((-i-1)*(-i+1))", "√(-1-i)×√(1-i)+√(2)×i",
                k_cartesianCtx);

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
}

QUIZ_CASE(pcj_simplification_power_real) {
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
}

QUIZ_CASE(pcj_simplification_roots) {
  simplifies_to("√(9)", "3");
  simplifies_to("√(865)", "√(865)");
  simplifies_to("√(865)/6", "√(865)/6");
  simplifies_to("√(30)", "√(30)");
  simplifies_to("√(-9)", "3×i", k_cartesianCtx);
  simplifies_to("√(i)", "√(2)/2+√(2)/2×i", k_cartesianCtx);
  simplifies_to("√(-i)", "√(2)/2-√(2)/2×i", k_cartesianCtx);
  simplifies_to("√(2eπ)*√(2eπ)", "2×π×e");
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

  simplifies_to("root(-8,3)", "-2");
  simplifies_to("root(48,3)", "2*root(6,3)");
  simplifies_to("root(81,3)", "3*root(3,3)");
  simplifies_to("2*root(81,3)", "6*root(3,3)");
  simplifies_to("7*root(81,3)", "21*root(3,3)");
  simplifies_to("21*root(3,3)", "21*root(3,3)");
  simplifies_to("root(4,3)", "root(4,3)");
  simplifies_to("root(4,π)", "2^(2/π)");
  simplifies_to("root(27,3)", "3");
  simplifies_to("root(4^4,3)", "4*root(4,3)");
  simplifies_to("root(5^4,3)", "5*root(5,3)");
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

  simplifies_to("log(3,27)", "1/3");
  simplifies_to("log(27,3)", "3");
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
  simplifies_to("e^(ln(x))", "dep(x,{nonNull(x)})", k_cartesianCtx);
  simplifies_to("ln(e^x)", "x", k_cartesianCtx);
  simplifies_to("ln(e^(i×π))", "π×i", k_cartesianCtx);
  simplifies_to("ln(e^(-i×π))", "π×i", k_cartesianCtx);
  simplifies_to("ln(e^(i×2×π))", "0", k_cartesianCtx);

  simplifies_to("log(9,7)", "2×log(3,7)");
  simplifies_to("log(9,8)", "2×log(3,8)");
  simplifies_to("log(3,7)+log(5,7)", "log(3,7)+log(5,7)");
  simplifies_to("log(3,8)+log(5,8)", "log(3,8)+log(5,8)");
}
