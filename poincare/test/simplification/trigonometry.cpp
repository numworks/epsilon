#include "helper.h"

using namespace Poincare;
using namespace Poincare::Internal;

QUIZ_CASE(pcj_simplification_trigonometry) {
  // Direct trigonometry exact formulas
  simplifies_to("cos({0,π/2,π,3π/2,-4π})", "{1,0,-1,0,1}");
  simplifies_to("sin({0,π/2,π,3π/2,-4π})", "{0,1,0,-1,0}");
  // test π/12 in top-right quadrant
  simplifies_to("cos({π/12,-19π/12})", "{(√(2)+√(6))/4,(√(2)×(-1+√(3)))/4}");
  simplifies_to("sin({π/12,-19π/12})", "{(√(2)×(-1+√(3)))/4,(√(2)+√(6))/4}");
  // test π/10 in top-left quadrant
  simplifies_to("cos({66π/10,-31π/10})", "{-(-1+√(5))/4,-√((5+√(5))/8)}");
  simplifies_to("sin({66π/10,-31π/10})", "{√((5+√(5))/8),(-1+√(5))/4}");
  // test π/8 in bottom-left quadrant
  simplifies_to("cos({9π/8,59π/8})", "{-√(2+√(2))/2,-√(2-√(2))/2}");
  simplifies_to("sin({9π/8,59π/8})", "{-√(2-√(2))/2,-√(2+√(2))/2}");
  // test π/6 in bottom-right quadrant
  simplifies_to("cos({22π/6,11π/6})", "{1/2,√(3)/2}");
  simplifies_to("sin({22π/6,11π/6})", "{-√(3)/2,-1/2}");
  // test π/5 in all quadrants
  simplifies_to("cos({6π/5,-33π/5,18π/5,-π/5})",
                "{-(1+√(5))/4,-(-1+√(5))/4,(-1+√(5))/4,(1+√(5))/4}");
  simplifies_to("sin({π/5,2π/5,3π/5,-6π/5})",
                "{√((5-√(5))/8),√((5+√(5))/8),√((5+√(5))/8),√((5-√(5))/8)}");
  // test π/4 in all quadrants
  simplifies_to("cos({π/4,3π/4,-11π/4,7π/4})",
                "{√(2)/2,-√(2)/2,-√(2)/2,√(2)/2}");
  simplifies_to("sin({π/4,3π/4,-11π/4,7π/4})",
                "{√(2)/2,√(2)/2,-√(2)/2,-√(2)/2}");
  simplifies_to("cos(π)", "cos(π)", {.m_angleUnit = AngleUnit::Degree});
  simplifies_to("cos(45)", "√(2)/2", {.m_angleUnit = AngleUnit::Degree});

  // Inverse trigonometry exact formulas
  simplifies_to("acos({-1, -√(3)/2, -√(2)/2, -1/2, 0, 1/2, √(2)/2, √(3)/2, 1})",
                "{π,5π/6,3π/4,2π/3,π/2,π/3,π/4,π/6,0}");
  simplifies_to("acos(-1/√(2))", "3π/4");
  simplifies_to("acos(1/√(2))", "π/4");
  simplifies_to("acos(-(√(6)+√(2))/4)", "11π/12");
  simplifies_to("acos(-(√(6)-√(2))/4)", "7π/12");
  simplifies_to("acos((√(6)-√(2))/4)", "5π/12");
  simplifies_to("acos((√(6)+√(2))/4)", "π/12");
  simplifies_to("asin(-1/√(2))", "-π/4");
  simplifies_to("asin(1/√(2))", "π/4");
  simplifies_to("asin({-1, -√(3)/2, -√(2)/2, -1/2, 0, 1/2, √(2)/2, √(3)/2, 1})",
                "{-π/2,-π/3,-π/4,-π/6,0,π/6,π/4,π/3,π/2}");
  simplifies_to("asin({-1, -√(3)/2, -√(2)/2, -1/2, 0, 1/2, √(2)/2, √(3)/2, 1})",
                "{-90,-60,-45,-30,0,30,45,60,90}",
                {.m_angleUnit = AngleUnit::Degree});

  simplifies_to("asin(-(√(6)+√(2))/4)", "-5π/12");
  simplifies_to("asin(-(√(6)-√(2))/4)", "-π/12");
  simplifies_to("asin((√(6)-√(2))/4)", "π/12");
  simplifies_to("asin((√(6)+√(2))/4)", "5π/12");

  simplifies_to("atan(0)", "0");
  simplifies_to("atan({1,-1}*(inf))", "{π/2, -π/2}");
  simplifies_to("atan({1,-1}*(√(3)))", "{π/3, -π/3}");
  simplifies_to("atan({1,-1}*(1))", "{π/4, -π/4}");
  simplifies_to("atan({1,-1}*(√(5-2√(5))))", "{π/5, -π/5}");
  simplifies_to("atan({1,-1}*(√(5+2√(5))))", "{2π/5, -2π/5}");
  simplifies_to("atan({1,-1}*(√(3)/3))", "{π/6, -π/6}");
  simplifies_to("atan({1,-1}*(-1+√(2)))", "{π/8, -π/8}");
  simplifies_to("atan({1,-1}*(1+√(2)))", "{3π/8, -3π/8}");
  simplifies_to("atan({1,-1}*(√(1-2√(5)/5)))", "{π/10, -π/10}");
  simplifies_to("atan({1,-1}*(√(1+2√(5)/5)))", "{3π/10, -3π/10}");
  simplifies_to("atan({1,-1}*(2-√(3)))", "{π/12, -π/12}");
  simplifies_to("atan({1,-1}*(2+√(3)))", "{5π/12, -5π/12}");

  // Trig diff
  simplifies_to("2×sin(2y)×sin(y)+cos(3×y)", "cos(y)");
  simplifies_to("2×sin(2y)×cos(y)-sin(3×y)", "sin(y)");
  simplifies_to("2×cos(2y)×sin(y)+sin(y)", "sin(3×y)");
  simplifies_to("2×cos(2y)×cos(y)-cos(y)", "cos(3×y)");
  simplifies_to("sin(4π/21)sin(π/7)", "(-1/2+cos((41×π)/21))/2");
  simplifies_to("cos(13π/42)sin(π/7)", "(-1/2+sin((19×π)/42))/2");

  // Direct trigonometry
  simplifies_to("cos(-x)", "cos(x)");
  simplifies_to("-sin(-x)", "sin(x)");
  simplifies_to("tan(-x)", "-tan(x)");
  simplifies_to("cos({-inf,inf})", "{undef, undef}");
  simplifies_to("sin({-inf,inf})", "{undef, undef}");
  simplifies_to("sin((241/120)π)", "sin(π/120)");

  // Direct advanced trigonometry
  simplifies_to("1/tan(x)", "dep(cot(x),{nonNull(cos(x))})");
  simplifies_to("1/tan(3)", "cot(3)");

  // Inverse trigonometry
  simplifies_to("acos(-x)", "arccos(-x)", cartesianCtx);
  simplifies_to("asin(-x)", "arcsin(-x)", cartesianCtx);
  simplifies_to("atan(-x)", "-arctan(x)", cartesianCtx);

  // trig(atrig)
  simplifies_to("cos({acos(x), asin(x), atan(x)})",
                "{x,√(-x^2+1),cos(arctan(x))}", cartesianCtx);
  simplifies_to("cos({acos(-x), asin(-x), atan(-x)})",
                "{-x,√(-x^2+1),cos(arctan(x))}", cartesianCtx);
  simplifies_to("sin({acos(x), asin(x), atan(x)})",
                "{√(-x^2+1),x,sin(arctan(x))}", cartesianCtx);
  simplifies_to("sin({acos(-x), asin(-x), atan(-x)})",
                "{√(-x^2+1),-x,-sin(arctan(x))}", cartesianCtx);
  simplifies_to("tan({acos(x), asin(x), atan(x)})",
                "{√(-x^2+1)/x,x/√(-x^2+1),x}", cartesianCtx);
  simplifies_to("tan({acos(-x), asin(-x), atan(-x)})",
                "{-√(-x^2+1)/x,-x/√(-x^2+1),-x}", cartesianCtx);
  simplifies_to("cos({acos(x), asin(x), atan(x)})",
                "{x,√(-x^2+1),cos(arctan(x))}",
                {.m_complexFormat = ComplexFormat::Cartesian,
                 .m_angleUnit = AngleUnit::Degree});
  simplifies_to("cos(acos(3/7))", "3/7");
  simplifies_to("cos(acos(9/7))", "nonreal");
  simplifies_to("cos(acos(9/7))", "9/7", cartesianCtx);
  simplifies_to("sin(asin(3/7))", "3/7");
  simplifies_to("sin(asin(9/7))", "nonreal");
  simplifies_to("sin(asin(9/7))", "9/7", cartesianCtx);

  // atrig(trig)
  simplifies_to("acos({cos(x), sin(x), tan(x)})",
                "{arccos(cos(x)),arccos(sin(x)),arccos(tan(x))}", cartesianCtx);
  simplifies_to("asin({cos(x), sin(x), tan(x)})",
                "{arcsin(cos(x)),arcsin(sin(x)),arcsin(tan(x))}", cartesianCtx);
  simplifies_to("atan({cos(x), sin(x), tan(x)})",
                "{arctan(cos(x)),arctan(sin(x)),arctan(tan(x))}", cartesianCtx);
  simplifies_to("acos({cos(x), sin(x), tan(x)})",
                "{arccos(cos(x)),arccos(sin(x)),arccos(tan(x))}",
                {.m_complexFormat = ComplexFormat::Cartesian,
                 .m_angleUnit = AngleUnit::Degree});
  simplifies_to("acos(cos({3π/7, -11π/15,34π/15, 40π/13}))",
                "{(3×π)/7,(11×π)/15,(4×π)/15,(12×π)/13}");
  simplifies_to("asin(sin({3π/7, -11π/15,34π/15, 40π/13}))",
                "{(3×π)/7,-(4×π)/15,(4×π)/15,-π/13}");
  simplifies_to("atan(tan({3π/7, -11π/15,34π/15, 40π/13}))",
                "{(3×π)/7,(4×π)/15,(4×π)/15,π/13}");
  simplifies_to("acos({cos(683), sin(683)})", "{117,183}",
                {.m_angleUnit = AngleUnit::Gradian});
  simplifies_to("acos(sin({π*23/7, -23*π/7}))/π", "{11/14,3/14}");
  simplifies_to("asin(cos({π*23/7, -23*π/7}))/π", "{-3/14,-3/14}");
  simplifies_to("atan({tan(-3π/13), -sin(3π/13)/cos(3π/13)})",
                "{-3π/13,-3π/13}");
  simplifies_to("atan({sin(55π/13)/cos(3π/13),sin(55π/13)/cos(-101π/13)})",
                "{3π/13,3π/13}");
  simplifies_to("atan(sin({3,10,3,16,3,23}π/13)/cos({36,55,42,55,75,55}π/13))",
                "{-3π/13,3π/13,-3π/13,-3π/13,3π/13,-3π/13}");
  simplifies_to("atan(asin(cos(4)))", "-atan(-4+3π/2)");
  // TODO: fix
  // simplifies_to("atan(tan(π/10))", "π/10");
  simplifies_to("atan(tan(9))", "9", {.m_angleUnit = AngleUnit::Degree});
  simplifies_to("acos(cos(9))", "9", {.m_angleUnit = AngleUnit::Degree});
  simplifies_to("asin(sin(9))", "9", {.m_angleUnit = AngleUnit::Degree});
  simplifies_to("acos(atan(tan(cos(9))))", "9",
                {.m_angleUnit = AngleUnit::Degree});
  simplifies_to("asin(acos(atan(tan(cos(sin(9))))))", "9",
                {.m_angleUnit = AngleUnit::Degree});

  // Angle format with hyperbolic trigonometry
  simplifies_to("cos(2)+cosh(2)+cos(2)", "2×cos(2)+cosh(2)",
                {.m_angleUnit = AngleUnit::Degree});

#if 0
  // TODO: asin(x) = π/2 - acos(x) advanced reduction safe from infinite loops.
  simplifies_to("asin(x)-π/2", "-arccos(x)");
  simplifies_to("90-acos(x)", "arcsin(x)", {.m_angleUnit = AngleUnit::Degree});
#endif
  // TODO: Improve output with better advanced reduction.
  simplifies_to("(y*π+z/180)*asin(x)", "(π×y+z/180)×arcsin(x)",
                {.m_complexFormat = ComplexFormat::Cartesian,
                 .m_angleUnit = AngleUnit::Degree});
  simplifies_to("arg(cos(π/6)+i*sin(π/6))", "π/6");
  simplifies_to("sin(17×π/12)^2+cos(5×π/12)^2", "1", cartesianCtx);
  simplifies_to("sin(17)^2+cos(6)^2", "cos(6)^2+sin(17)^2", cartesianCtx);
  // Only works in cartesian, because Power VS PowerReal. See Projection::Expand
  simplifies_to("cos(atan(x))-√(-(x/√(x^(2)+1))^(2)+1)",
                "dep(0,{0×√(-x^2/(x^2+1)+1)})", cartesianCtx);
  // Strategy
  simplifies_to("sin(90)", "1",
                {.m_angleUnit = AngleUnit::Degree,
                 .m_strategy = Strategy::ApproximateToFloat});

  // Beautification and dependencies
  simplifies_to("re(2*acos(cos(x)))",
                "dep(2×re(arccos(cos(x))),{0×im(arccos(cos(x))),0×im(π×arccos("
                "cos(x))),piecewise(0,abs(cos((180×π×x)/(180×π)))≤1,nonreal)})",
                {.m_angleUnit = AngleUnit::Degree});
  simplifies_to("tan(20)^2*tan(2)", "tan(2)*tan(20)^2");

  // atan(1/x)
  simplifies_to("arctan(1/(1+x^2))", "π/2-arctan(x^2+1)");
  simplifies_to("arctan(-1/(1+abs(x)))", "-π/2+arctan(1+abs(x))");
}

QUIZ_CASE(pcj_simplification_trigonometry_hyperbolic) {
  simplifies_to("cosh(-x)", "cosh(x)");
  simplifies_to("sinh(-x)", "-sinh(x)");
  simplifies_to("tanh(-x)", "-tanh(x)");
  simplifies_to("arcosh(-x)", "arcosh(-x)");
  // TODO: Should simplify to -arsinh(x)
  simplifies_to("arsinh(-x)", "arsinh(-x)");
  simplifies_to("artanh(-x)", "artanh(-x)");
  simplifies_to("cosh(i)", "cos(1)");
  simplifies_to("sinh(i)", "sin(1)×i", cartesianCtx);
  simplifies_to("tanh(i)", "tan(1)×i", cartesianCtx);
  simplifies_to("arcosh(i)", "arcosh(i)", cartesianCtx);
  simplifies_to("arsinh(i)", "π/2×i", cartesianCtx);
  simplifies_to("artanh(i)", "π/4×i", cartesianCtx);
  simplifies_to("cosh(2*pi+1)", "cosh(1+2×π)", cartesianCtx);
  simplifies_to("cosh(-x)+sinh(x)", "e^(x)");
  simplifies_to("cosh(x)^2-sinh(-x)^2", "1");
  // TODO: Should simplify to 0
  simplifies_to("((1+tanh(x)^2)*tanh(2x)/2)-tanh(x)",
                "-tanh(x)-(-sinh(x)^2/(2×cosh(x)^2)-1/2)×tanh(2×x)");
  simplifies_to("arcosh(5)", "arcosh(5)", cartesianCtx);
  simplifies_to("arcosh(5)-ln(5+√(24))", "0", cartesianCtx);
  simplifies_to("arcosh(cosh(x))", "abs(x)", cartesianCtx);
  simplifies_to("arsinh(sinh(x))", "x", cartesianCtx);
  simplifies_to("artanh(tanh(x))", "x", cartesianCtx);
  simplifies_to("cosh(arcosh(x))", "x", cartesianCtx);
  simplifies_to("sinh(arsinh(x))", "x", cartesianCtx);
  simplifies_to("tanh(artanh(x))", "x", cartesianCtx);
}

QUIZ_CASE(pcj_simplification_trigonometry_advanced) {
  simplifies_to("cot(π/2)", "0");
  simplifies_to("cot(90)", "0", {.m_angleUnit = AngleUnit::Degree});
  simplifies_to("arccot(0)", "90", {.m_angleUnit = AngleUnit::Degree});
  simplifies_to("arccot(0)", "π/2");
  simplifies_to("sec(x)", "1/cos(x)");
  simplifies_to("csc(x)", "1/sin(x)");
  simplifies_to("cot(x)", "cot(x)");
  simplifies_to("arcsec(sec(π/6))", "π/6");
  simplifies_to("arccsc(csc(π/6))", "π/6");
  simplifies_to("arccot(cot(π/6))", "π/6");
  simplifies_to("arccot(-1)", "3π/4");
  simplifies_to("arccot(-1)", "135", {.m_angleUnit = AngleUnit::Degree});

  simplifies_to("csc(arccsc(9/7))", "9/7");
  simplifies_to("csc(arccsc(3/7))", "nonreal");
  simplifies_to("csc(arccsc(3/7))", "3/7", cartesianCtx);
  simplifies_to("sec(arcsec(9/7))", "9/7");
  simplifies_to("sec(arcsec(3/7))", "nonreal");

  simplifies_to("sec(arcsec(x))", "dep(x,{nonNull(x)})", cartesianCtx);
  simplifies_to("csc(arccsc(x))", "dep(x,{nonNull(x)})", cartesianCtx);
  // FIXME : Should be "dep(1+abs(x),{tan(arctan(1+abs(x)))})"
  simplifies_to("cot(arccot(1+abs(x)))", "tan(arctan(1+abs(x)))", cartesianCtx);

  simplifies_to("sin(x)*(cos(x)^-1)*ln(x)",
                "dep(tan(x)×ln(x),{nonNull(x),realPos(x)})");
  simplifies_to("ln(x)*tan(x)", "dep(tan(x)×ln(x),{nonNull(x),realPos(x)})");
  simplifies_to("sin(x)*(cos(y)^-1)*(cos(x)^-1)*sin(y)", "tan(x)×tan(y)");
}
