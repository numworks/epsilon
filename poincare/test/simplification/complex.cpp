#include "../helpers/symbol_store.h"
#include "helper.h"

using namespace Poincare;
using namespace Poincare::Internal;

QUIZ_CASE(pcj_simplification_complex) {
  ProjectionContext ctx = {
      .m_complexFormat = ComplexFormat::Cartesian,
      .m_symbolic = SymbolicComputation::KeepAllSymbols,
  };
  ProjectionContext PolarCtx = {
      .m_complexFormat = ComplexFormat::Polar,
      .m_symbolic = SymbolicComputation::KeepAllSymbols,
  };
  simplifies_to("2×i×i", "-2", ctx);
  simplifies_to("1+i×(1+i×(1+i))", "0", ctx);
  simplifies_to("(1+i)×(3+2i)", "1+5×i", ctx);
  simplifies_to("√(-1)", "i", ctx);
  simplifies_to("re(2+i×π)", "2", ctx);
  simplifies_to("im(2+i×π)", "π", ctx);
  simplifies_to("conj(2+i×π)", "2-π×i", ctx);
  simplifies_to("i×im(x)+re(x)", "x");
  simplifies_to("re(ln(1-3×i)+2×π×i)", "re(ln(1-3×i))", ctx);
  // z being a finite unknown real, arccos(z) is a finite unknown complex.
  simplifies_to("im(x+i*y+arccos(z))", "y+im(arccos(z))", ctx);
  simplifies_to("im(x)", "0", ctx);
  simplifies_to("im(i*y)", "y", ctx);
  simplifies_to("im(arccos(z))", "im(arccos(z))", ctx);
  simplifies_to("im(x1+x2+i*y1+i*y2+arccos(z1)+arccos(z2))",
                "y1+y2+im(arccos(z1))+im(arccos(z2))", ctx);
  simplifies_to("arccos(z)-i×im(arccos(z))", "re(arccos(z))", ctx);
  simplifies_to("i×im(arccos(z))+re(arccos(z))", "arccos(z)", ctx);
  simplifies_to("conj(√(i))", "√(2)/2-√(2)/2×i", ctx);
  simplifies_to("re(conj(arccos(z)))-re(arccos(z))", "0", ctx);
  simplifies_to("conj(conj(arccos(z)))", "arccos(z)", ctx);
  simplifies_to("re(arccos(z)+y)-y", "re(arccos(z))", ctx);
  simplifies_to("re(i×arccos(w))+im(arccos(w))", "0", ctx);
  simplifies_to("im(i×arccos(w))", "re(arccos(w))", ctx);
  simplifies_to("i×(conj(arccos(z)+i×arccos(w))+im(arccos(w))-re(arccos(z)))",
                "im(arccos(z))+re(arccos(w))", ctx);
  simplifies_to("im(re(arccos(z))+i×im(arccos(z)))", "im(arccos(z))", ctx);
  simplifies_to("re(re(arccos(z))+i×im(arccos(z)))", "re(arccos(z))", ctx);
  // TODO: Should simplify to 0
  simplifies_to(
      "abs(arccos(z)+i×arccos(w))^2-(-im(arccos(w))+re(arccos(z)))^2-(im("
      "arccos(z))+re(arccos(w)))^2",
      "im(arccos(z)+arccos(w)×i)^2-(im(arccos(z))+re(arccos(w)))^2", ctx);
  simplifies_to("arg(x+y×i)", "arg(x+y×i)", ctx);
  simplifies_to("arg(π+i×2)", "arctan(2/π)", ctx);
  simplifies_to("arg(-π+i×2)-π", "-arctan(2/π)", ctx);
  simplifies_to("arg(i×2)", "π/2", ctx);
  simplifies_to("arg(-i×2)", "-π/2", ctx);
  simplifies_to("arg(0)", "undef", ctx);
  simplifies_to("π-arg(-π+i×abs(y))", "arctan(abs(y)/π)", ctx);
  simplifies_to("arg(exp(i*π/7))", "π/7", ctx);
  simplifies_to("arg(exp(-i*π/7))", "-π/7", ctx);
  simplifies_to("arg(exp(i*π*10))", "0", ctx);
  simplifies_to("arg(exp(-i*π))", "π", ctx);
  simplifies_to("abs(arccos(z)^2)", "abs(arccos(z))^2", ctx);
  simplifies_to("e^(arg(e^(x×i))×i)", "e^(x×i)", PolarCtx);
  simplifies_to("e^(arg(e^(x×i))×i)", "cos(x)+sin(x)×i", ctx);
  simplifies_to("arg(abs(x)×e^(arg(z)×i))", "dep(arg(z),{nonNull(abs(x))})",
                ctx);
  simplifies_to("arg(-3×(x+y×i))", "arg(-(x+y×i))", ctx);
  // TODO: Simplify arg between ]-π,π]
  simplifies_to("arg(e^(534/7×i))", "arg(e^(534/7×i))");
  simplifies_to("arg(e^(3.1415×i))", "arg(e^(6283/2000×i))");
  // TODO: Improve sign detection
  simplifies_to("abs(-1+π)", "-1+π");
  simplifies_to("abs(1-π)", "-1+π");

  simplifies_to("2/(4+4*i)", "1/4-1/4×i", cartesianCtx);
  simplifies_to("e^(π*i/6)+e^(-π*i/3)", "(1+√(3))/2+(1/2-√(3)/2)×i",
                cartesianCtx);
  simplifies_to("e^(π*i/6)*e^(-π*i/3)", "√(3)/2-1/2×i", cartesianCtx);
  simplifies_to("re(√(√(-8)))", "root(2,4)", cartesianCtx);
  simplifies_to("re(√(15+2×i))", "√((15+√(229))/2)", cartesianCtx);
  simplifies_to("5*ln(1+i)-ln(exp(5*ln(1+i)))", "2×π×i", cartesianCtx);
  simplifies_to("0.3*ln(1+i)-ln(exp(0.3*ln(1+i)))", "0", cartesianCtx);
  simplifies_to("2×cos(π/12)×e^(5×i×π/12)", "1/2+(1+√(3)/2)×i", cartesianCtx);
  simplifies_to("(1+i)^201", "(1+i)^201", cartesianCtx);

  simplifies_to("arg(0)", "undef", cartesianCtx);
  simplifies_to("arg(123)", "0", cartesianCtx);
  simplifies_to("arg(-1ᴇ123)", "π", cartesianCtx);
  simplifies_to("arg(i)", "π/2", cartesianCtx);
  simplifies_to("arg(-6i)", "-π/2", cartesianCtx);
  simplifies_to("arg(1+i)", "π/4", cartesianCtx);
  simplifies_to("conj(1/2)", "1/2", cartesianCtx);
  simplifies_to("re(1/2)", "1/2", cartesianCtx);
  simplifies_to("im(1+5×i)", "5", cartesianCtx);
  simplifies_to("re(1+5×i)", "1", cartesianCtx);
}

QUIZ_CASE(pcj_simplification_polar) {
  simplifies_to("0", "0", polarCtx);
  simplifies_to("1", "1", polarCtx);
  simplifies_to("-1", "e^(π×i)", polarCtx);
  simplifies_to("2i", "2×e^(π/2×i)", polarCtx);
  simplifies_to("cos(i)", "cosh(1)", polarCtx);
  simplifies_to("[[42, -2/3][1+i, -iπ]]",
                "[[42,(2×e^(π×i))/3][√(2)×e^(π/4×i),π×e^(-π/2×i)]]", polarCtx);
  simplifies_to("-2×_m", "-2×_m", polarCtx);
  simplifies_to("(-2,i)", "(2×e^(π×i),e^(π/2×i))", polarCtx);
  simplifies_to("{-2,-i}", "{2×e^(π×i),e^(-π/2×i)}", polarCtx);
  simplifies_to("(y/y+3)×e^(i×(x-x+2))", "dep(4×e^(2×i),{y^0})", polarCtx);
  simplifies_to("3+4i", "5×e^(arctan(4/3)×i)", polarCtx);
  simplifies_to("e^(3.14×i)", "e^(157/50×i)", polarCtx);
  simplifies_to("e^(-2.1×i)", "e^(-21/10×i)", polarCtx);
  simplifies_to("root(-8,3)", "2×e^(π/3×i)", polarCtx);
  simplifies_to("e^(π/6×i)+e^(-π/3×i)", "√(2)×e^(-π/12×i)", polarCtx);
  simplifies_to("e^(π/6×i)+e^(-10π/6×i)", "(√(2)/2+√(6)/2)×e^(π/4×i)",
                polarCtx);
  // Not simplified into polars
  simplifies_to("-1+π", "-1+π", polarCtx);
  simplifies_to("1-π", "(-1+π)×e^(π×i)", polarCtx);
  // TODO: Simplify arg between ]-π,π]
  simplifies_to("e^(534/7×i)", "e^(534/7×i)", polarCtx);
  simplifies_to("e^(3.1415×i)", "e^(6283/2000×i)", polarCtx);
}

QUIZ_CASE(poincare_simplification_complex_format) {
  // Real
  simplifies_to("i", "nonreal");
  simplifies_to("√(-1)", "nonreal");
  simplifies_to("√(-1)×√(-1)", "nonreal");
  simplifies_to("ln(-2)", "nonreal");
  simplifies_to("(-8)^(2/3)", "4");
#if TODO_PCJ
  simplifies_to("(-8)^(2/5)", "2×root(2,5)");
  simplifies_to("(-8)^(1/5)", "-root(8,5)");
#endif
  simplifies_to("(-8)^(1/4)", "nonreal");
  simplifies_to("(-8)^(1/3)", "-2");
  simplifies_to("[[1,2+√(-1)]]", "[[1,nonreal]]");
  simplifies_to("atan(2)", "arctan(2)");
  simplifies_to("atan(-2)", "-arctan(2)");

  {
    // User defined variable
    simplifies_to("a", "a");
    // a = 2+i
    PoincareTest::SymbolStore symbolStore;
    Poincare::ProjectionContext projCtx = {
        .m_symbolic = SymbolicComputation::ReplaceDefinedSymbols,
        .m_context = symbolStore};
    store("2+i→a", symbolStore);
    simplifies_to("a", "nonreal", projCtx);
    symbolStore.reset();
    // User defined function
    // f : x → x+1
    store("x+1+i→f(x)", symbolStore);
    simplifies_to("f(3)", "nonreal", projCtx);
  }

  // Cartesian
  simplifies_to("-2.3ᴇ3", "-2300", cartesianCtx);
  simplifies_to("3", "3", cartesianCtx);
  simplifies_to("∞", "∞", cartesianCtx);
  simplifies_to("1+2+i", "3+i", cartesianCtx);
  simplifies_to("-(5+2×i)", "-5-2×i", cartesianCtx);
  simplifies_to("(5+2×i)", "5+2×i", cartesianCtx);
  simplifies_to("i+i", "2×i", cartesianCtx);
  simplifies_to("-2+2×i", "-2+2×i", cartesianCtx);
  simplifies_to("(3+i)-(2+4×i)", "1-3×i", cartesianCtx);
  simplifies_to("(2+3×i)×(4-2×i)", "14+8×i", cartesianCtx);
  simplifies_to("(3+i)/2", "3/2+1/2×i", cartesianCtx);
  simplifies_to("(3+i)/(2+i)", "7/5-1/5×i", cartesianCtx);
  // The simplification of (3+i)^(2+i) in a Cartesian complex form generates to
  // many nodes
  // simplifies_to("(3+i)^(2+i)",
  // "10×cos((-4×atan(3)+ln(2)+ln(5)+2×π)/2)×e^((2×atan(3)-π)/2)+10×sin((-4×atan(3)+ln(2)+ln(5)+2×π)/2)×e^((2×atan(3)-π)/2)i",
  // cartesianCtx);
  simplifies_to("(3+i)^(2+i)", "(3+i)^(2+i)", cartesianCtx);
  simplifies_to("√(1+6i)", "√((1+√(37))/2)+√((-1+√(37))/2)×i", cartesianCtx);
  simplifies_to("(1+i)^2", "2×i", cartesianCtx);
  simplifies_to("2×i", "2×i", cartesianCtx);
  simplifies_to("i!", "undef", cartesianCtx);
  simplifies_to("3!", "6", cartesianCtx);
  simplifies_to("x!", "x!", cartesianCtx);
  simplifies_to("e", "e", cartesianCtx);
  simplifies_to("π", "π", cartesianCtx);
  simplifies_to("i", "i", cartesianCtx);

  simplifies_to("arctan(2)", "arctan(2)", cartesianCtx);
  simplifies_to("arctan(-2)", "-arctan(2)", cartesianCtx);
  simplifies_to("abs(-3)", "3", cartesianCtx);
  simplifies_to("abs(-3+i)", "√(10)", cartesianCtx);
  simplifies_to("arctan(2)", "arctan(2)", cartesianCtx);
  simplifies_to("arctan(2+i)", "arctan(2+i)", cartesianCtx);
  simplifies_to("binomial(10, 4)", "210", cartesianCtx);
  simplifies_to("ceil(-1.3)", "-1", cartesianCtx);
  simplifies_to("arg(-2)", "π", cartesianCtx);
  simplifies_to("conj(-2)", "-2", cartesianCtx);
  simplifies_to("conj(-2+2×i+i)", "-2-3×i", cartesianCtx);
  simplifies_to("cos(12)", "cos(12)", cartesianCtx);
  simplifies_to("cos(12+i)", "cos(12)×cosh(1)-sin(12)×sinh(1)×i", cartesianCtx);
  simplifies_to("diff(3×x, x, 3)", "3", cartesianCtx);
  simplifies_to("quo(34,x)", "quo(34,x)", cartesianCtx);
  simplifies_to("rem(5,3)", "2", cartesianCtx);
  simplifies_to("floor(x)", "floor(x)", cartesianCtx);
  simplifies_to("frac(x)", "frac(x)", cartesianCtx);
  simplifies_to("gcd(x,y)", "gcd(x,y)", cartesianCtx);
  simplifies_to("gcd(x,gcd(y,z))", "gcd(x,y,z)", cartesianCtx);
  simplifies_to("gcd(3, 1, 2, x, x^2)", "gcd(1,x^2,x)", cartesianCtx);
  simplifies_to("im(1+i)", "1", cartesianCtx);
  simplifies_to("int(x^2, x, 1, 2)", "int(x^2,x,1,2)", cartesianCtx);
  simplifies_to("lcm(x,y)", "lcm(x,y)", cartesianCtx);
  simplifies_to("lcm(x,lcm(y,z))", "lcm(x,y,z)", cartesianCtx);
  simplifies_to("lcm(3, 1, 2, x, x^2)", "lcm(6,x^2,x)", cartesianCtx);
  // TODO: dim is not simplified yet
  // simplifies_to("dim(x)", "dim(x)", User, Radian,
  // MetricUnitFormat, Cartesian);

  simplifies_to("root(2,i)", "cos(ln(2))-sin(ln(2))×i", cartesianCtx);
  // TODO_PCJ: get minus sign out of cos and sin
  simplifies_to("root(2,i+1)", "√(2)×(cos(-(90×ln(2))/π)+sin(-(90×ln(2))/π)×i)",
                degreeCartesianCtx);
  simplifies_to("root(2,i+1)", "cos(-ln(2)/2)×√(2)+sin(-ln(2)/2)×√(2)×i",
                cartesianCtx);
  simplifies_to("permute(10, 4)", "5040", cartesianCtx);
  simplifies_to("random()", "random()", cartesianCtx);
  simplifies_to("re(x)", "x", cartesianCtx);
  simplifies_to("round(x,y)", "round(x,y)", cartesianCtx);
  simplifies_to("sign(x)", "sign(x)", cartesianCtx);
  simplifies_to("sin(23)", "sin(23)", cartesianCtx);
  simplifies_to("sin(23+i)", "sin(23)×cosh(1)+cos(23)×sinh(1)×i", cartesianCtx);
  simplifies_to("√(1-i)", "√((1+√(2))/2)-√((-1+√(2))/2)×i", cartesianCtx);
  simplifies_to("tan(23)", "tan(23)", cartesianCtx);
  simplifies_to("tan(23+i)", "tan(23+i)", cartesianCtx);
  simplifies_to("[[1,√(-1)]]", "[[1,i]]", cartesianCtx);

  // User defined variable
  simplifies_to("a", "a", cartesianCtx);
  {
    // a = 2+i
    PoincareTest::SymbolStore symbolStore;
    Poincare::ProjectionContext projCtx = {
        .m_complexFormat = ComplexFormat::Cartesian,
        .m_symbolic = SymbolicComputation::ReplaceDefinedSymbols,
        .m_context = symbolStore};
    store("2+i→a", symbolStore);
    simplifies_to("a", "2+i", projCtx);
    symbolStore.reset();
    // User defined function
    // f : x → x+1
    store("x+1+i→f(x)", symbolStore);
    simplifies_to("f(3)", "4+i", projCtx);
  }
  // Polar
  simplifies_to("-2.3ᴇ3", "2300×e^(π×i)", polarCtx);
  simplifies_to("3", "3", polarCtx);
  simplifies_to("∞", "∞", polarCtx);
  simplifies_to("1+2+i", "√(10)×e^(arctan(1/3)×i)", polarCtx);
  simplifies_to("1+2+i", "√(10)×e^((π×arctan(1/3))/180×i)", degreePolarCtx);
  simplifies_to("-(5+2×i)", "√(29)×e^((-π+arctan(2/5))×i)", polarCtx);
  simplifies_to("(5+2×i)", "√(29)×e^(arctan(2/5)×i)", polarCtx);
  simplifies_to("i+i", "2×e^(π/2×i)", polarCtx);
  simplifies_to("i+i", "2×e^(π/2×i)", polarCtx);
  simplifies_to("-2+2×i", "2×√(2)×e^((3×π)/4×i)", polarCtx);
  simplifies_to("(3+i)-(2+4×i)", "√(10)×e^(-arctan(3)×i)", polarCtx);
  simplifies_to("(2+3×i)×(4-2×i)", "(2×√(65))×e^(arctan(4/7)×i)", polarCtx);
  simplifies_to("(3+i)/2", "√(10)/2×e^(arctan(1/3)×i)", polarCtx);
  simplifies_to("(3+i)/(2+i)", "√(2)×e^(-arctan(1/7)×i)", polarCtx);
  // TODO: simplify arctan(tan(x)) = x±k×pi?
  // simplifies_to("(3+i)^(2+i)",
  // "10e^((2×arctan(3)-π)/2)×e^(((-4×arctan(3)+ln(2)+ln(5)+2π)/2)i)",
  //  polarCtx);
  // The simplification of (3+i)^(2+i) in a Polar complex form generates too
  // many nodes
  simplifies_to("(3+i)^(2+i)", "(3+i)^(2+i)", polarCtx);
  simplifies_to("(1+i)^2", "2×e^(π/2×i)", polarCtx);
  simplifies_to("2×i", "2×e^(π/2×i)", polarCtx);
  simplifies_to("3!", "6", polarCtx);
  simplifies_to("x!", "x!", polarCtx);
  simplifies_to("e", "e", polarCtx);
  simplifies_to("π", "π", polarCtx);
  simplifies_to("i", "e^(π/2×i)", polarCtx);
  simplifies_to("abs(-3)", "3", polarCtx);
  simplifies_to("abs(-3+i)", "√(10)", polarCtx);
  simplifies_to("conj(2×e^(i×π/2))", "2×e^(-π/2×i)", polarCtx);
  simplifies_to("-2×e^(i×π/2)", "2×e^(-π/2×i)", polarCtx);
  simplifies_to("[[1,√(-1)]]", "[[1,e^(π/2×i)]]", polarCtx);
  simplifies_to("arctan(2)", "arctan(2)", polarCtx);
  simplifies_to("arctan(-2)", "arctan(2)×e^(π×i)", polarCtx);
  simplifies_to("cos(42π)", "cos(42×π)", degreePolarCtx);

  // User defined variable
  simplifies_to("a", "a", polarCtx);
  {
    PoincareTest::SymbolStore symbolStore;
    Poincare::ProjectionContext projCtx = {
        .m_complexFormat = ComplexFormat::Polar,
        .m_symbolic = SymbolicComputation::ReplaceDefinedSymbols,
        .m_context = symbolStore};
    // a = 2 + i
    store("2+i→a", symbolStore);
    simplifies_to("a", "√(5)×e^(arctan(1/2)×i)", projCtx);
    symbolStore.reset();
    // User defined function
    // f: x → x+1

    store("x+1+i→f(x)", symbolStore);
    simplifies_to("f(3)", "√(17)×e^(arctan(1/4)×i)", projCtx);
    symbolStore.reset();
  }
}
