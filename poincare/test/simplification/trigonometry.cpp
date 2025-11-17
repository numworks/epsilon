#include <poincare/math_options.h>

#include "../helper.h"
#include "helper.h"

using namespace Poincare;
using namespace Poincare::Internal;

QUIZ_CASE(pcj_simplification_trigonometry) {
  // Direct trigonometry exact formulas
  simplifies_to("cos({0,π/2,π,3π/2,-4π})", "{1,0,-1,0,1}");
  simplifies_to("sin({0,π/2,π,3π/2,-4π})", "{0,1,0,-1,0}");
  // test π/12 in top-right quadrant
  simplifies_to("cos({π/12,-19π/12})", "{(√(2)+√(6))/4,(-√(2)+√(6))/4}");
  simplifies_to("sin({π/12,-19π/12})", "{(-√(2)+√(6))/4,(√(2)+√(6))/4}");
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
  simplifies_to("sin(4π/21)sin(π/7)", "-1/4+cos(π/21)/2");
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
  simplifies_to("acos(-x)", "arccos(-x)", k_cartesianCtx);
  simplifies_to("asin(-x)", "arcsin(-x)", k_cartesianCtx);
  simplifies_to("atan(-x)", "-arctan(x)", k_cartesianCtx);

  // trig(atrig)
  simplifies_to("cos({acos(x), asin(x), atan(x)})", "{x,√(-x^2+1),1/√(x^2+1)}",
                k_cartesianCtx);
  simplifies_to("cos({acos(-x), asin(-x), atan(-x)})",
                "{-x,√(-x^2+1),1/√(x^2+1)}", k_cartesianCtx);
  simplifies_to("sin({acos(x), asin(x), atan(x)})", "{√(-x^2+1),x,x/√(x^2+1)}",
                k_cartesianCtx);
  simplifies_to("sin({acos(-x), asin(-x), atan(-x)})",
                "{√(-x^2+1),-x,-x/√(x^2+1)}", k_cartesianCtx);
  simplifies_to("tan({acos(x), asin(x), atan(x)})",
                "{√(-x^2+1)/x,x/√(-x^2+1),x}", k_cartesianCtx);
  simplifies_to("tan({acos(-x), asin(-x), atan(-x)})",
                "{-√(-x^2+1)/x,-x/√(-x^2+1),-x}", k_cartesianCtx);
  simplifies_to("cos({acos(x), asin(x), atan(x)})", "{x,√(-x^2+1),1/√(x^2+1)}",
                {.m_complexFormat = ComplexFormat::Cartesian,
                 .m_angleUnit = AngleUnit::Degree});
  simplifies_to("cos(acos(3/7))", "3/7");
  simplifies_to("cos(acos(9/7))", "nonreal");
  simplifies_to("cos(acos(9/7))", "9/7", k_cartesianCtx);
  simplifies_to("sin(asin(3/7))", "3/7");
  simplifies_to("sin(asin(9/7))", "nonreal");
  simplifies_to("sin(asin(9/7))", "9/7", k_cartesianCtx);

  // atrig(trig)
  simplifies_to("acos({cos(x), sin(x), tan(x)})",
                "{arccos(cos(x)),arccos(sin(x)),arccos(tan(x))}",
                k_cartesianCtx);
  simplifies_to("asin({cos(x), sin(x), tan(x)})",
                "{arcsin(cos(x)),arcsin(sin(x)),arcsin(tan(x))}",
                k_cartesianCtx);
  simplifies_to("atan({cos(x), sin(x), tan(x)})",
                "{arctan(cos(x)),arctan(sin(x)),arctan(tan(x))}",
                k_cartesianCtx);
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
  simplifies_to("asin(2/√(3)×sin(60))", "90",
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
  simplifies_to("sin(17×π/12)^2+cos(5×π/12)^2", "1", k_cartesianCtx);
  simplifies_to("sin(17)^2+cos(6)^2", "cos(6)^2+sin(17)^2", k_cartesianCtx);
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

  // -- sin/cos -> tan
  simplifies_to("sin(x)/cos(x)", "tan(x)");
  simplifies_to("cos(x)/sin(x)", "cot(x)");
  // 1/tan = cot if tan(x) != undef
  simplifies_to("1/tan(x)", "dep(cot(x),{nonNull(cos(x))})");
  simplifies_to("sin(x)×π/cos(x)", "π×tan(x)");
  simplifies_to("sin(x)/(π×cos(x))", "tan(x)/π");
  simplifies_to("1×tan(2)×tan(5)", "tan(2)×tan(5)");
  /* TODO_PCJ: decide which angle we want to reduce to, see #6586
   * In this example we currently retun tan((20×π)/21) */
#if TODO_PCJ
  simplifies_to("tan(62π/21)", "-tan(π/21)");
  simplifies_to("cos(26π/21)/sin(25π/17)", "cos((5×π)/21)/sin((8×π)/17)");
  simplifies_to("cos(62π/21)×π×3/sin(62π/21)", "-(3×π)/tan(π/21)");
  simplifies_to("cos(62π/21)/(π×3×sin(62π/21))", "-1/(3×π×tan(π/21))");
  simplifies_to("sin(62π/21)×π×3/cos(62π/21)", "-3×π×tan(π/21)");
  simplifies_to("sin(62π/21)/(π×3cos(62π/21))", "-tan(π/21)/(3×π)");
  simplifies_to("-cos(π/62)ln(3)/(sin(π/62)π)", "-ln(3)/(π×tan(π/62))");
#endif
  simplifies_to("-2cos(π/62)ln(3)/(sin(π/62)π)", "-(2×cot(π/62)×ln(3))/π");
  // -- cos
  simplifies_to("cos(0)", "1");
  simplifies_to("cos(π)", "-1");
#if TODO_PCJ
  simplifies_to("cos(π×4/7)", "-cos((3×π)/7)");
  simplifies_to("cos(π×35/29)", "-cos((6×π)/29)");
  simplifies_to("cos(-π×35/29)", "-cos((6×π)/29)");
#endif
  simplifies_to("cos(π×340000)", "1");
  simplifies_to("cos(-π×340001)", "-1");
  simplifies_to("cos(-π×√(2))", "cos(π×√(2))");
  simplifies_to("cos(1311π/6)", "0");
  simplifies_to("cos(π/12)", "(√(2)+√(6))/4");
  simplifies_to("cos(-π/12)", "(√(2)+√(6))/4");
  simplifies_to("cos(-π17/8)", "√(2+√(2))/2");
  simplifies_to("cos(41π/6)", "-√(3)/2");
  simplifies_to("cos(π/4+1000π)", "√(2)/2");
  simplifies_to("cos(-π/3)", "1/2");
  simplifies_to("cos(41π/5)", "(1+√(5))/4");
  simplifies_to("cos(7π/10)", "-√((5-√(5))/8)");
  simplifies_to("cos(0)", "1", k_degreeCtx);
  simplifies_to("cos(180)", "-1", k_degreeCtx);
#if TODO_PCJ
  simplifies_to("cos(720/7)", "-cos(540/7)", k_degreeCtx);
  simplifies_to("cos(6300/29)", "-cos(1080/29)", k_degreeCtx);
  simplifies_to("cos(-6300/29)", "-cos(1080/29)", k_degreeCtx);
#endif
  simplifies_to("cos(61200000)", "1", k_degreeCtx);
  simplifies_to("cos(-61200180)", "-1", k_degreeCtx);
  simplifies_to("cos(-180×√(2))", "cos(180×√(2))", k_degreeCtx);
  simplifies_to("cos(39330)", "0", k_degreeCtx);
  simplifies_to("cos(15)", "(√(2)+√(6))/4", k_degreeCtx);
  simplifies_to("cos(-15)", "(√(2)+√(6))/4", k_degreeCtx);
  simplifies_to("cos(-765/2)", "√(2+√(2))/2", k_degreeCtx);
  simplifies_to("cos(7380/6)", "-√(3)/2", k_degreeCtx);
  simplifies_to("cos(180045)", "√(2)/2", k_degreeCtx);
  simplifies_to("cos(-60)", "1/2", k_degreeCtx);
  simplifies_to("cos(7380/5)", "(1+√(5))/4", k_degreeCtx);
  simplifies_to("cos(112.5)", "-√(2-√(2))/2", k_degreeCtx);
  // -- sin
  simplifies_to("sin(0)", "0");
  simplifies_to("sin(π)", "0");
#if TODO_PCJ
  simplifies_to("sin(π×35/29)", "-sin((6×π)/29)");
  simplifies_to("sin(-π×35/29)", "sin((6×π)/29)");
#endif
  simplifies_to("sin(π×340000)", "0");
  simplifies_to("sin(π×340001)", "0");
  simplifies_to("sin(-π×340001)", "0");
#if TODO_PCJ
  simplifies_to("sin(π/12)", "(√(6)-√(2))/4");
  simplifies_to("sin(-π/12)", "(-√(6)+√(2))/4");
#endif
  simplifies_to("sin(-π×√(2))", "-sin(π×√(2))");
  simplifies_to("sin(1311π/6)", "1");
  simplifies_to("sin(-π17/8)", "-√(2-√(2))/2");
  simplifies_to("sin(41π/6)", "1/2");
  simplifies_to("sin(-3π/10)", "-(1+√(5))/4");
  simplifies_to("sin(π/4+1000π)", "√(2)/2");
  simplifies_to("sin(-π/3)", "-√(3)/2");
  simplifies_to("sin(17π/5)", "-√((5+√(5))/8)");
  simplifies_to("sin(π/5)", "√((5-√(5))/8)");
  simplifies_to("sin(0)", "0", k_degreeCtx);
  simplifies_to("sin(180)", "0", k_degreeCtx);
#if TODO_PCJ
  simplifies_to("sin(6300/29)", "-sin(1080/29)", k_degreeCtx);
  simplifies_to("sin(-6300/29)", "sin(1080/29)", k_degreeCtx);
#endif
  simplifies_to("sin(61200000)", "0", k_degreeCtx);
  simplifies_to("sin(61200180)", "0", k_degreeCtx);
  simplifies_to("sin(-61200180)", "0", k_degreeCtx);
  /*  Currently simplifies to (√(2)×(-1+√(3)))/4.
   * TODO: develop and contract */
#if TODO_PCJ
  simplifies_to("sin(15)", "(√(6)-√(2))/4", k_degreeCtx);
  simplifies_to("sin(-15)", "(-√(6)+√(2))/4", k_degreeCtx);
#endif
  simplifies_to("sin(-180×√(2))", "-sin(180×√(2))", k_degreeCtx);
  simplifies_to("sin(39330)", "1", k_degreeCtx);
  simplifies_to("sin(-765/2)", "-√(2-√(2))/2", k_degreeCtx);
  simplifies_to("sin(1230)", "1/2", k_degreeCtx);
  simplifies_to("sin(180045)", "√(2)/2", k_degreeCtx);
  simplifies_to("sin(-60)", "-√(3)/2", k_degreeCtx);
  simplifies_to("sin(612)", "-√((5+√(5))/8)", k_degreeCtx);
  simplifies_to("sin(36)", "√((5-√(5))/8)", k_degreeCtx);
  // -- tan
  simplifies_to("tan(0)", "0");
  simplifies_to("tan(π)", "0");
  simplifies_to("tan(3×π/2)", "undef");
  simplifies_to("tan(π/2)", "undef");
  simplifies_to("tan(90)", "undef", k_degreeCtx);
  simplifies_to("tan(100)", "undef", k_gradianCtx);
#if TODO_PCJ
  simplifies_to("tan(π×35/29)", "tan((6×π)/29)");
  simplifies_to("tan(-π×35/29)", "-tan((6×π)/29)");
#endif
  simplifies_to("tan(π×340000)", "0");
  simplifies_to("tan(π×340001)", "0");
  simplifies_to("tan(-π×340001)", "0");
  simplifies_to("tan(π/12)", "2-√(3)");
  simplifies_to("tan(-π/12)", "-2+√(3)");
  simplifies_to("tan(-π×√(2))", "-tan(π×√(2))");
  simplifies_to("tan(1311π/6)", "undef");
  simplifies_to("tan(-π17/8)", "1-√(2)");
  simplifies_to("tan(41π/6)", "-√(3)/3");
  simplifies_to("tan(π/4+1000π)", "1");
  simplifies_to("tan(-π/3)", "-√(3)");
  /*  Currently simplifies to -((-1+√(5))×√(2-(2×√(5))/5))/4.
   * TODO: contract to -√(1-2×√(5)/5) */
#if TODO_PCJ
  simplifies_to("tan(-π/10)", "-√(1-2×√(5)/5)");
#endif
  simplifies_to("tan(0)", "0", k_degreeCtx);
  simplifies_to("tan(180)", "0", k_degreeCtx);
#if TODO_PCJ
  simplifies_to("tan(6300/29)", "tan(1080/29)", k_degreeCtx);
  simplifies_to("tan(-6300/29)", "-tan(1080/29)", k_degreeCtx);
#endif
  simplifies_to("tan(61200000)", "0", k_degreeCtx);
  simplifies_to("tan(61200180)", "0", k_degreeCtx);
  simplifies_to("tan(-61200180)", "0", k_degreeCtx);
  simplifies_to("tan(15)", "2-√(3)", k_degreeCtx);
  simplifies_to("tan(-15)", "-2+√(3)", k_degreeCtx);
  simplifies_to("tan(-180×√(2))", "-tan(180×√(2))", k_degreeCtx);
  simplifies_to("tan(39330)", "undef", k_degreeCtx);
  simplifies_to("tan(-382.5)", "1-√(2)", k_degreeCtx);
  simplifies_to("tan(1230)", "-√(3)/3", k_degreeCtx);
  simplifies_to("tan(180045)", "1", k_degreeCtx);
  simplifies_to("tan(-60)", "-√(3)", k_degreeCtx);
  simplifies_to("tan(tan(tan(tan(9))))", "tan(tan(tan(tan(9))))");
  // 1/tan = cot if tan(x) != undef
  simplifies_to("1/tan(2)", "cot(2)");
  simplifies_to("tan(3)*sin(3)/cos(3)", "tan(3)^2");
  // -- acos
  simplifies_to("acos(-1/2)", "(2×π)/3", k_cartesianCtx);
  simplifies_to("acos(-1.2)", "π-arccos(6/5)", k_cartesianCtx);
  simplifies_to("acos(cos(2/3))", "2/3", k_cartesianCtx);
  simplifies_to("acos(cos(3/2))", "3/2", k_cartesianCtx);
  simplifies_to("cos(acos(3/2))", "3/2", k_cartesianCtx);
  simplifies_to("cos(acos(2/3))", "2/3", k_cartesianCtx);

  simplifies_to("acos(cos(12))", "-12+4×π", k_cartesianCtx);
  simplifies_to("acos(cos(2*1ᴇ10))", "arccos(cos(20000000000))",
                k_cartesianCtx);
  simplifies_to("acos(cos(inf))", "undef", k_cartesianCtx);
  simplifies_to("acos(cos(9))", "9-2×π", k_cartesianCtx);
  simplifies_to("acos(cos(10^125))", "arccos(cos(10^125))", k_cartesianCtx);
  simplifies_to("acos(cos(1/0))", "undef", k_cartesianCtx);
  simplifies_to("acos(cos(-8.8))", "44/5-2×π", k_cartesianCtx);
  simplifies_to("acos(cos(π+26))", "-26+9×π", k_cartesianCtx);
  simplifies_to("acos(cos(0))", "0", k_cartesianCtx);
  simplifies_to("acos(cos(9π))", "π", k_cartesianCtx);
  simplifies_to("acos(cos(2*1ᴇ10))", "160", k_degreeCartesianCtx);
  simplifies_to("acos(cos(180+50))", "130", k_degreeCartesianCtx);
  simplifies_to(
      "arccos(cos(2345995537929342462976×π^5-36850577388590589246720×π^4+"
      "231537533966682879807360×π^3-727391989955238208647840×π^2+"
      "1142577399842170168717980×π-717897987691852588770249))",
      "arccos(cos(2345995537929342462976×π^5-36850577388590589246720×π^4+"
      "231537533966682879807360×π^3-727391989955238208647840×π^2-"
      "717897987691852588770249+1142577399842170168717980×π))",
      k_cartesianCtx);

  simplifies_to("acos(cos(4π/7))", "(4×π)/7", k_cartesianCtx);
  simplifies_to("acos(-cos(2))", "-2+π", k_cartesianCtx);
  simplifies_to("acos(-1/2)", "120", k_degreeCartesianCtx);
  simplifies_to("acos(-1.2)", "180-arccos(6/5)", k_degreeCartesianCtx);
  simplifies_to("acos(cos(2/3))", "2/3", k_degreeCartesianCtx);
  simplifies_to("acos(cos(190))", "170", k_degreeCartesianCtx);
  simplifies_to("acos(cos(75))", "75", k_degreeCartesianCtx);
  simplifies_to("cos(acos(190))", "190", k_degreeCartesianCtx);
  simplifies_to("cos(acos(75))", "75", k_degreeCartesianCtx);
  simplifies_to("acos(cos(12))", "12", k_degreeCartesianCtx);
  simplifies_to("acos(cos(720/7))", "720/7", k_degreeCartesianCtx);
  // -- asin
  simplifies_to("asin(-1/2)", "-π/6", k_cartesianCtx);
  simplifies_to("asin(-1.2)", "-arcsin(6/5)", k_cartesianCtx);
  simplifies_to("asin(sin(2/3))", "2/3", k_cartesianCtx);
  simplifies_to("sin(asin(2/3))", "2/3", k_cartesianCtx);
  simplifies_to("sin(asin(3/2))", "3/2", k_cartesianCtx);
  simplifies_to("asin(sin(3/2))", "3/2", k_cartesianCtx);
  simplifies_to("asin(sin(3.6))", "-18/5+π", k_cartesianCtx);
  simplifies_to("asin(sin(-2.23))", "223/100-π", k_cartesianCtx);
  simplifies_to("asin(sin(-18.39))", "-1839/100+6×π", k_cartesianCtx);

  simplifies_to("asin(sin(12))", "12-4×π", k_cartesianCtx);
  simplifies_to("asin(sin(2+π))", "2-π", k_cartesianCtx);
  simplifies_to("asin(sin(90+6800))", "50", k_degreeCartesianCtx);
  simplifies_to("asin(sin(60-9×9×9))", "51", k_degreeCartesianCtx);

  simplifies_to("asin(sin(-π/7))", "-π/7", k_cartesianCtx);
  simplifies_to("asin(sin(-√(2)))", "-√(2)", k_cartesianCtx);
  simplifies_to("asin(-1/2)", "-30", k_degreeCartesianCtx);
  simplifies_to("asin(-1.2)", "-arcsin(6/5)", k_degreeCartesianCtx);
  simplifies_to("asin(sin(75))", "75", k_degreeCartesianCtx);
  simplifies_to("sin(asin(75))", "75", k_degreeCartesianCtx);
  simplifies_to("sin(asin(190))", "190", k_degreeCartesianCtx);
  simplifies_to("asin(sin(32))", "32", k_degreeCartesianCtx);
  simplifies_to("asin(sin(400))", "40", k_degreeCartesianCtx);
  simplifies_to("asin(sin(-180/7))", "-180/7", k_degreeCartesianCtx);
  simplifies_to("arcsin(sin(10^10))", "arcsin(sin(10000000000))",
                k_cartesianCtx);
  // -- atan
  simplifies_to("atan(-1)", "-π/4");
  simplifies_to("atan(-1.2)", "-arctan(6/5)");
  simplifies_to("atan(tan(2/3))", "2/3");
  simplifies_to("tan(atan(2/3))", "2/3");
  // If target != User, sin/cos is not reduced to tan(x)
  projects_and_reduces_to(
      "atan(sin(2/3)/cos(2/3))", "2/3",
      {.m_reductionTarget = Poincare::ReductionTarget::SystemForAnalysis});
  simplifies_to("tan(atan(5/2))", "5/2");
  simplifies_to("atan(tan(5/2))", "5/2-π");
  simplifies_to("atan(tan(-π/7))", "-π/7");
  simplifies_to("atan(√(3))", "π/3");
  simplifies_to("atan(tan(-√(2)))", "-√(2)");
  simplifies_to("atan(-1)", "-45", k_degreeCtx);
  simplifies_to("atan(-1.2)", "-arctan(6/5)", k_degreeCtx);
  simplifies_to("atan(tan(-45))", "-45", k_degreeCtx);
  simplifies_to("tan(atan(120))", "120", k_degreeCtx);
  simplifies_to("tan(atan(2293))", "2293", k_degreeCtx);
  simplifies_to("atan(tan(2293))", "-47", k_degreeCtx);
  simplifies_to("atan(tan(1808))", "8", k_degreeCtx);
  simplifies_to("atan(tan(-180/7))", "-180/7", k_degreeCtx);
  simplifies_to("atan(√(3))", "60", k_degreeCtx);
  simplifies_to("atan(inf)", "π/2", k_cartesianCtx);
  simplifies_to("atan(-inf)", "-π/2", k_cartesianCtx);
  simplifies_to("tan(atan(inf))", "undef", k_cartesianCtx);
  // Use abs(x)+1 because atan(1/x) is only reduced if sign(x) can be reduced.
  simplifies_to("atan(1/(abs(x)+1))", "π/2-arctan(1+abs(x))");
  simplifies_to("atan(1/(abs(x)+1))", "90-arctan(1+abs(x))", k_degreeCtx);
  simplifies_to("atan(1/(abs(x)+1))", "100-arctan(1+abs(x))", k_gradianCtx);
  simplifies_to("atan(cos(x)/sin(x))", "arctan(cot(x))");
  simplifies_to("atan(cot(π/7))", "(5×π)/14");
  // TODO: contract to (3×π-8)/2
  simplifies_to("atan(cot(4))", "-4+(3×π)/2");
  simplifies_to("atan(cos(π/7)/sin(π/7))", "(5×π)/14");
  // TODO: contract to (3×π-8)/2
  simplifies_to("atan(cos(4)/sin(4))", "-4+(3×π)/2");
  simplifies_to("atan(cos(1.57079632)/sin(1.57079632))",
                "-9817477/6250000+π/2");
  // cos(asin)
  simplifies_to("cos(asin(x))", "√(-x^2+1)", k_degreeCartesianCtx);
  simplifies_to("cos(asin(-x))", "√(-x^2+1)", k_degreeCartesianCtx);
  // cos(atan)
  simplifies_to("cos(atan(x))", "1/√(x^2+1)", k_degreeCartesianCtx);
  simplifies_to("cos(atan(-x))", "1/√(x^2+1)", k_degreeCartesianCtx);
  // sin(acos)
  simplifies_to("sin(acos(x))", "√(-x^2+1)", k_degreeCartesianCtx);
  simplifies_to("sin(acos(-x))", "√(-x^2+1)", k_degreeCartesianCtx);
  // sin(atan)
  simplifies_to("sin(atan(x))", "x/√(x^2+1)", k_degreeCartesianCtx);
  simplifies_to("sin(atan(-x))", "-x/√(x^2+1)", k_degreeCartesianCtx);
  // tan(acos)
  simplifies_to("tan(acos(x))", "√(-x^2+1)/x", k_degreeCartesianCtx);
  simplifies_to("tan(acos(-x))", "-√(-x^2+1)/x", k_degreeCartesianCtx);
  // tan(asin)
  simplifies_to("tan(asin(x))", "x/√(-x^2+1)", k_degreeCartesianCtx);
  simplifies_to("tan(asin(-x))", "-x/√(-x^2+1)", k_degreeCartesianCtx);

  // Mix
  simplifies_to("sin(atan(3/4))", "3/5", k_degreeCartesianCtx);
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
  simplifies_to("sinh(i)", "sin(1)×i", k_cartesianCtx);
  simplifies_to("tanh(i)", "tan(1)×i", k_cartesianCtx);
  simplifies_to("arcosh(i)", "arcosh(i)", k_cartesianCtx);
  simplifies_to("arsinh(i)", "π/2×i", k_cartesianCtx);
  simplifies_to("artanh(i)", "π/4×i", k_cartesianCtx);
  simplifies_to("cosh(2*pi+1)", "cosh(1+2×π)", k_cartesianCtx);
  simplifies_to("cosh(-x)+sinh(x)", "e^(x)");
  simplifies_to("cosh(x)^2-sinh(-x)^2", "1");
  // TODO: Should simplify to 0
  simplifies_to("((1+tanh(x)^2)*tanh(2x)/2)-tanh(x)",
                "-tanh(x)+((tanh(x)^2+1)×tanh(2×x))/2");
  simplifies_to("arcosh(5)", "arcosh(5)", k_cartesianCtx);
  simplifies_to("arcosh(5)-ln(5+√(24))", "0", k_cartesianCtx);
  simplifies_to("arcosh(cosh(x))", "abs(x)", k_cartesianCtx);
  simplifies_to("arsinh(sinh(x))", "x", k_cartesianCtx);
  simplifies_to("artanh(tanh(x))", "x", k_cartesianCtx);
  simplifies_to("cosh(arcosh(x))", "x", k_cartesianCtx);
  simplifies_to("sinh(arsinh(x))", "x", k_cartesianCtx);
  simplifies_to("tanh(artanh(x))", "x", k_cartesianCtx);

  // Exact values
  simplifies_to("sinh(0)", "0");
  simplifies_to("cosh(0)", "1");
  simplifies_to("tanh(0)", "0");
  simplifies_to("arsinh(0)", "0");
  simplifies_to("arcosh(1)", "0");
  simplifies_to("artanh(0)", "0");

  // arcosh(cosh)
  simplifies_to("arcosh(cosh(3))", "3");
  simplifies_to("arcosh(cosh(0.5))", "1/2");
  simplifies_to("arcosh(cosh(-3))", "3");
  simplifies_to("arcosh(cosh(3))", "3", k_cartesianCtx);
  simplifies_to("arcosh(cosh(0.5))", "1/2", k_cartesianCtx);
  simplifies_to("arcosh(cosh(-3))", "3", k_cartesianCtx);

  // cosh(acosh)
  simplifies_to("cosh(arcosh(3))", "3");
  simplifies_to("cosh(arcosh(0.5))", "1/2");
  simplifies_to("cosh(arcosh(-3))", "-3");
  simplifies_to("cosh(arcosh(3))", "3", k_cartesianCtx);
  simplifies_to("cosh(arcosh(0.5))", "1/2", k_cartesianCtx);
  simplifies_to("cosh(arcosh(-3))", "-3", k_cartesianCtx);

  // sinh(asinh)
  simplifies_to("sinh(arsinh(3))", "3");
  simplifies_to("sinh(arsinh(0.5))", "1/2");
  simplifies_to("sinh(arsinh(-3))", "-3");
  simplifies_to("sinh(arsinh(3))", "3", k_cartesianCtx);
  simplifies_to("sinh(arsinh(0.5))", "1/2", k_cartesianCtx);
  simplifies_to("sinh(arsinh(-3))", "-3", k_cartesianCtx);

  // arsinh(sinh)
  simplifies_to("arsinh(sinh(3))", "3");
  simplifies_to("arsinh(sinh(0.5))", "1/2");
  simplifies_to("arsinh(sinh(-3))", "-3");
  simplifies_to("arsinh(sinh(3))", "3", k_cartesianCtx);
  simplifies_to("arsinh(sinh(0.5))", "1/2", k_cartesianCtx);
  simplifies_to("arsinh(sinh(-3))", "-3", k_cartesianCtx);

  // tanh(atanh)
  simplifies_to("tanh(artanh(3))", "3");
  simplifies_to("tanh(artanh(0.5))", "1/2");
  simplifies_to("tanh(artanh(-3))", "-3");
  simplifies_to("tanh(artanh(3))", "3", k_cartesianCtx);
  simplifies_to("tanh(artanh(0.5))", "1/2", k_cartesianCtx);
  simplifies_to("tanh(artanh(-3))", "-3", k_cartesianCtx);

  // artanh(tanh)
  simplifies_to("artanh(tanh(3))", "3");
  simplifies_to("artanh(tanh(0.5))", "1/2");
  simplifies_to("artanh(tanh(-3))", "-3");
  simplifies_to("artanh(tanh(3))", "3", k_cartesianCtx);
  simplifies_to("artanh(tanh(0.5))", "1/2", k_cartesianCtx);
  simplifies_to("artanh(tanh(-3))", "-3", k_cartesianCtx);
}

QUIZ_CASE(pcj_simplification_trigonometry_advanced) {
  simplifies_to("sec(x)", "1/cos(x)");
  simplifies_to("csc(x)", "1/sin(x)");
  simplifies_to("cot(x)", "cot(x)");
  simplifies_to("sec(arcsec(x))", "dep(x,{nonNull(x)})", k_cartesianCtx);
  simplifies_to("csc(arccsc(x))", "dep(x,{nonNull(x)})", k_cartesianCtx);
  simplifies_to("cot(arccot(1+abs(x)))", "1+abs(x)", k_cartesianCtx);
  simplifies_to("sin(x)*(cos(x)^-1)*ln(x)",
                "dep(tan(x)×ln(x),{nonNull(x),realPos(x)})");
  simplifies_to("ln(x)*tan(x)", "dep(tan(x)×ln(x),{nonNull(x),realPos(x)})");
  simplifies_to("sin(x)*(cos(y)^-1)*(cos(x)^-1)*sin(y)", "tan(x)×tan(y)");

  // Exact values
  simplifies_to("cot(0)", "undef");
  simplifies_to("cot(π/2)", "0");
  simplifies_to("cot(π°)", "cot(π^2/180)");
  simplifies_to("cot(2)", "cot(2)");
  simplifies_to("cot(90)", "0", {.m_angleUnit = AngleUnit::Degree});
  simplifies_to("csc(0)", "undef");
  simplifies_to("sec(0)", "1");
  simplifies_to("arccsc(2/√(3))", "π/3");
  simplifies_to("arcsec(2/√(3))", "π/6");
  simplifies_to("csc(π/2)", "1");
  simplifies_to("sec(π/2)", "undef");
  simplifies_to("cot(π/2)", "0");
  simplifies_to("arccsc(1)", "π/2");
  simplifies_to("arcsec(1)", "0");
  simplifies_to("arccot(0)", "90", {.m_angleUnit = AngleUnit::Degree});
  simplifies_to("arccot(0)", "π/2");
  simplifies_to("arccot(1)", "π/4");
  simplifies_to("arccot(-1)", "3π/4");
  simplifies_to("arccot(-1)", "135", {.m_angleUnit = AngleUnit::Degree});

  // arcsec(sec)
  simplifies_to("arcsec(sec(3))", "3");
  simplifies_to("arcsec(sec(0.5))", "1/2");
  simplifies_to("arcsec(sec(-3))", "3");
  simplifies_to("arcsec(sec(π/6))", "π/6");

  // sec(asec)
  simplifies_to("sec(arcsec(3))", "3", k_cartesianCtx);
  simplifies_to("sec(arcsec(0.5))", "1/2", k_cartesianCtx);
  simplifies_to("sec(arcsec(-3))", "-3", k_cartesianCtx);
  simplifies_to("sec(arcsec(9/7))", "9/7");
  simplifies_to("sec(arcsec(3/7))", "nonreal");

  // arccsc(csc)
  simplifies_to("arccsc(csc(3))", "-3+π");
  simplifies_to("arccsc(csc(0.5))", "1/2");
  simplifies_to("arccsc(csc(-3))", "3-π");
  simplifies_to("arccsc(csc(π/6))", "π/6");

  // csc(acsc)
  simplifies_to("csc(arccsc(3))", "3", k_cartesianCtx);
  simplifies_to("csc(arccsc(0.5))", "1/2", k_cartesianCtx);
  simplifies_to("csc(arccsc(-3))", "-3", k_cartesianCtx);
  simplifies_to("csc(arccsc(9/7))", "9/7");
  simplifies_to("csc(arccsc(3/7))", "nonreal");
  simplifies_to("csc(arccsc(3/7))", "3/7", k_cartesianCtx);

  // arccot(cot)
  simplifies_to("arccot(cot(3))", "3");
  simplifies_to("arccot(cot(0.5))", "1/2");
  simplifies_to("arccot(cot(-3))", "-3+π");
  simplifies_to("arccot(cot(π/6))", "π/6");
  simplifies_to("arccot(3)", "π/2-arctan(3)");
  projects_and_reduces_to(
      "arccot(3)", "π/2-arctan(3)",
      {.m_reductionTarget = Poincare::ReductionTarget::SystemForAnalysis});
  projects_and_reduces_to(
      "arccot(3)", "π/2-arctan(3)",
      {.m_reductionTarget = Poincare::ReductionTarget::SystemForApproximation});
  simplifies_to("arccot(x)", "π/2-arctan(x)");
  projects_and_reduces_to(
      "arccot(x)", "π/2-arctan(x)",
      {.m_reductionTarget = Poincare::ReductionTarget::SystemForAnalysis});
  projects_and_reduces_to(
      "arccot(x)", "π/2-arctan(x)",
      {.m_reductionTarget = Poincare::ReductionTarget::SystemForApproximation});

  // cot(acot)
  simplifies_to("cot(arccot(3))", "3");
  simplifies_to("cot(arccot(0.5))", "1/2");
  simplifies_to("cot(arccot(-3))", "-3");
}
