#include <poincare/test/helper.h>

#include "helper.h"

using namespace Poincare::Internal;
using namespace Poincare;

QUIZ_CASE(pcj_simplification_addition) {
  simplifies_to("1/x^2+3", "3+1/x^2");
  simplifies_to("1+x", "x+1");
  simplifies_to("1/2+1/3+1/4+1/5+1/6+1/7", "223/140");
  simplifies_to("1+x+4-y-2x", "-x-y+5");
  simplifies_to("2+1", "3");
  simplifies_to("1+2", "3");
  simplifies_to("1+2+3+4+5+6+7", "28");
  simplifies_to("(0+0)", "0");
  simplifies_to("1+2+3+4+5+M+6+7", "M+28");
  simplifies_to("1+M+2+P+3", "M+P+6");
  simplifies_to("-2+6", "4");
  simplifies_to("-2-6", "-8");
  simplifies_to("-M", "-M");
  simplifies_to("M-M", "0");
  simplifies_to("-5π+3π", "-2×π");
  simplifies_to("1-3+M-5+2M-4M", "-M-7");
  simplifies_to("M+P-M-P", "0");
  simplifies_to("M+P+(-1)×M+(-1)×P", "0");
  simplifies_to("2+13cos(2)-23cos(2)", "2-10×cos(2)");
  simplifies_to("1+1+ln(2)+(5+3×2)/9-4/7+1/98", "2347/882+ln(2)");
  simplifies_to("1+2+0+cos(2)", "3+cos(2)");
  simplifies_to("M-M+2cos(2)+P-P-cos(2)", "cos(2)");
  simplifies_to("x+3+π+2×x", "π+3×(x+1)");
  simplifies_to("1/(x+1)+1/(π+2)", "1/(2+π)+1/(x+1)");
  simplifies_to("1/x^2+1/(x^2×π)", "(1+1/π)/x^2");
  simplifies_to("1/x^2+1/(x^3×π)", "1/x^2+1/(π×x^3)");
  simplifies_to("4x/x^2+3π/(x^3×π)", "4/x+3/x^3");
  simplifies_to("3^(1/2)+π^2+1", "π^2+1+√(3)");
  simplifies_to("3^(1/2)+2^(-2×3^(1/2)×e^π)/2", "√(3)+1/(2×2^(2×e^(π)×√(3)))");
  // TODO_PCJ: Should be (2×π-9)/5
  simplifies_to("((4π+5)/10)-2.3", "-9/5+(2×π)/5");
  simplifies_to("4×_°+3×_'+2×_\"-(3×_°+2×_'+1×_\")", "(3661×π)/648000×_rad");
  simplifies_to("[[1,2+i][3,4][5,6]]+[[1,2+i][3,4][5,6]]",
                "[[2,2×(2+i)][6,8][10,12]]");
  simplifies_to("3+[[1,2][3,4]]", "undef");
  simplifies_to("[[1][3][5]]+[[1,2+i][3,4][5,6]]", "undef");
  // Beautification order
  simplifies_to("√(2)+4+3×π+√(5)+2×√(5)", "4+3×π+√(2)+3×√(5)");
  simplifies_to("(√(2)+1+x+y)^3", "(x+y+1+√(2))^3");
  // cos^2+sin^2
  simplifies_to("cos(x)^2+sin(x)^2", "1");
  simplifies_to("2xln(x)cos(x)^2+2xln(x)sin(x)^2", "2×xln(x)");
  simplifies_to("5cos(3x+2)^2+5sin(3x+2)^2+3", "8");
  simplifies_to("4cos(x)^2+3sin(x)^2", "4×cos(x)^2+3×sin(x)^2");
}

QUIZ_CASE(pcj_simplification_multiplication) {
  simplifies_to("undef×x", "undef");
  simplifies_to("0×x+P", "P");
  simplifies_to("0×x×0×32×cos(3)", "0");
  simplifies_to("3×M^4×P^x×P^2×(M^2+2)×2×1.2", "(36×(M^2+2)×M^4×P^2×P^x)/5");
  simplifies_to("M×(P+Q)×(X+3)", "M×(P+Q)×(X+3)");
  simplifies_to("M/P", "M/P");
  simplifies_to("(1/2)×M/P", "M/(2×P)");
  simplifies_to("1+2+3+4+5+6", "21");
  simplifies_to("1-2+3-4+5-6", "-3");
  simplifies_to("987654321123456789×998877665544332211",
                "986545842648570754445552922919330479");
  simplifies_to("2/3", "2/3");
  simplifies_to("9/17+5/4", "121/68");
  simplifies_to("1/2×3/4", "3/8");
  simplifies_to("0×2/3", "0");
  simplifies_to("1+(1/(1+1/(1+1/(1+1))))", "8/5");
  simplifies_to("1+2/(3+4/(5+6/(7+8)))", "155/101");
  simplifies_to("3/4×16/12", "1");
  simplifies_to("3/4×(8+8)/12", "1");
  simplifies_to("916791/794976477", "305597/264992159");
  simplifies_to("321654987123456789/112233445566778899",
                "3249040273974311/1133671167341201");
  simplifies_to("0.1+0.2", "3/10");
  simplifies_to("2^3", "8");
  simplifies_to("(-1)×(-1)", "1");
  simplifies_to("(-2)^2", "4");
  simplifies_to("(-3)^3", "-27");
  simplifies_to("(1/2)^-1", "2");
  simplifies_to("√(2)×√(3)", "√(6)");
  simplifies_to("2×2^π", "2×2^π");
  simplifies_to("M^3×P×M^(-3)", "dep(P,{M^0})");
  simplifies_to("M^3×M^(-3)", "dep(1,{M^0})");
  simplifies_to("2^π×(1/2)^π", "1");
  simplifies_to("-iπ^3×(-iπ^3)×(-iπ^3)", "π^9×i", k_cartesianCtx);
  simplifies_to("(x+1)×(x+2)", "(x+1)×(x+2)");
  simplifies_to("(x+1)×(x-1)", "x^2-1");
  simplifies_to("11π/(22π+11π)", "1/3");
  simplifies_to("11/(22π+11π)", "1/(3×π)");
  simplifies_to("-11/(22π+11π)", "-1/(3×π)");
  simplifies_to("M^2×P×M^(-2)×P^(-2)", "dep(1/P,{M^0})");
  simplifies_to("M^(-1)×P^(-1)", "1/(M×P)");
  simplifies_to("x+x", "2×x");
  simplifies_to("2×x+x", "3×x");
  simplifies_to("x×2+x", "3×x");
  simplifies_to("2×x+2×x", "4×x");
  simplifies_to("x×2+2×n", "2×n+2×x");
  simplifies_to("x+x+n+n", "2×n+2×x");
  simplifies_to("x-x-n+n", "0");
  simplifies_to("x+n-x-n", "0");
  simplifies_to("π×3^(1/2)×(5π)^(1/2)×(4/5)^(1/2)", "2×π×√(3×π)");
  // TODO_PCJ: Should be (√(3)×π^(5/4))/3
  simplifies_to("12^(1/4)×(π/6)×(12×π)^(1/4)", "(π×root(144×π,4))/6");
  simplifies_to(
      "[[1,2+i][3,4][5,6]]×[[1,2+i,3,4][5,6+i,7,8]]",
      "[[11+5×i,13+9×i,17+7×i,20+8×i][23,30+7×i,37,44][35,46+11×i,57,68]]",
      k_cartesianCtx);
  simplifies_to("[[1,2][3,4]]×[[1,3][5,6]]×[[2,3][4,6]]",
                "[[82,123][178,267]]");
  simplifies_to("0*[[1,0][0,1]]^500", "[[0,0][0,0]]");
  simplifies_to("x^5/x^3", "dep(x^2,{nonNull(x)})");
  simplifies_to("x^5*x^3", "x^8");
  simplifies_to("x^3/x^5", "1/x^2");
  simplifies_to("x^0", "dep(1,{x^0})");

  ProjectionContext projCtx = {
      .m_complexFormat = ComplexFormat::Cartesian,
      .m_reductionTarget = ReductionTarget::SystemForAnalysis};
  projects_and_reduces_to("π^5/π^3", "π^2", projCtx);
  projects_and_reduces_to("π^5*π^3", "π^8", projCtx);
  projects_and_reduces_to("π^3/π^5", "1/π^2", projCtx);
  projects_and_reduces_to("π^0", "1", projCtx);
  projects_and_reduces_to("π^π/π^(π-1)", "π", projCtx);
  projects_and_reduces_to("x^5/x^3", "dep(x^2,{nonNull(x)})", projCtx);
  projects_and_reduces_to("x^5×x^3", "x^8", projCtx);
  projects_and_reduces_to("x^3/x^5", "1/x^2", projCtx);
  projects_and_reduces_to("x^0", "dep(1,{x^0})", projCtx);
  projects_and_reduces_to("x^π/x^(π-1)", "dep(x,{0×ln(x)})", projCtx);
  // TODO_PCJ: The dependency should be resolved
  projects_and_reduces_to("x^π/x^(π+1)", "dep(1/x,{0×ln(x)})", projCtx);
  projects_and_reduces_to("2^x×2^(-x)", "1", projCtx);
  projects_and_reduces_to("y^x×y^(-x)", "dep(1,{0×ln(y)})", projCtx);
  projects_and_reduces_to("x/√(x)", "dep(√(x),{-ln(x)/2})", projCtx);
  projects_and_reduces_to("x^(1/2)×x^(1/2)", "x", projCtx);

  simplifies_to("x^(1/2)×x^(1/2)", "x", k_cartesianCtx);
  simplifies_to("2*3^x*3^(-x)", "2");
  simplifies_to("10-1/(3^x)", "10-1/3^x");
  simplifies_to("2×cos(π/12)×e^(5πi/12)", "1/2+(1+√(3)/2)×i", k_cartesianCtx);
  // Do not factorize exponent if the multiplication result is over DBL_MAX
  simplifies_to("((1.63871ᴇ182)^(1/256))*((1.93871ᴇ157)^(1/256))",
                "root(1.93871×10^157,256)×root(1.63871×10^182,256)");
}
