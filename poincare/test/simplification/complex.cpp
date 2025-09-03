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
