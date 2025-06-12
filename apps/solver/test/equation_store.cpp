#include <apps/shared/global_context.h>
#include <poincare/test/helper.h>
#include <quiz.h>
#include <string.h>

#include "helpers.h"

QUIZ_CASE(solver_error) {
  Shared::GlobalContext globalContext;
  setComplexFormatAndAngleUnit(Cartesian, Radian);
  assert_solves_to_error("cos(x)=0", RequireApproximateSolution,
                         &globalContext);

  assert_solves_to_error("x+y+z+a+b+c+d=0", TooManyVariables, &globalContext);

  assert_solves_to_error("x^2+y=0", NonLinearSystem, &globalContext);

#if 0 /* TODO_PCJ: currently solves to NoError */
  assert_solves_to_error("x^3+x^2+1=int(1/t,t,0,1)", EquationUndefined, &globalContext);
  assert_solves_to_error("x×(x^2×int(1/t,t,0,1)+1)=0", EquationUndefined, &globalContext);
#endif

  assert_solves_to_error("x-[[2,3]]=0", EquationUndefined, &globalContext);
  assert_solves_to_error("x[[2,3]]=0", EquationUndefined, &globalContext);
  assert_solves_to_error("x-{2,3}=0", EquationUndefined, &globalContext);
  assert_solves_to_error("x{2,3}=0", EquationUndefined, &globalContext);

  assert_solves_to_error("x-random()=0", EquationUndefined, &globalContext);

  assert_solves_to_error("0*√(x)=0", EquationUnhandled, &globalContext);
  assert_solves_to_error("x+0/a=0", EquationUnhandled, &globalContext);

  // Restore default preferences
  setComplexFormatAndAngleUnit(Real, Radian);
}

QUIZ_CASE(solver_linear_system) {
  Shared::GlobalContext globalContext;
  setComplexFormatAndAngleUnit(Cartesian, Radian);
  assert_solves_to({"x+y=0", "3x+y+z=-5", "4z-π=0", "a+b+c=0", "a=3", "c=a+2"},
                   {"a=3", "b=-8", "c=5", "x=-5/2-π/8", "y=5/2+π/8", "z=π/4"},
                   &globalContext);
  assert_solves_to("2x+3=4", "x=1/2", &globalContext);
  assert_solves_to({"x+y=0", "3x+y+z=-5", "4z-π=0"},
                   {"x=-5/2-π/8", "y=5/2+π/8", "z=π/4"}, &globalContext);
  assert_solves_to({"x+y=0", "3x+y=-5"}, {"x=-5/2", "y=5/2"}, &globalContext);
  assert_solves_to_infinite_solutions("0=0", &globalContext);
  assert_solves_to_infinite_solutions({"x+y=0"}, {"x=-t", "y=t"},
                                      &globalContext);

  assert_solves_to({"x=int(e^(-t),t,0,inf)"}, {"x=1"}, &globalContext);
  assert_solves_to({"x*int(e^(-2t),t,0,inf)=1"}, {"x=2"}, &globalContext);
  assert_solves_to({"x=1+int(xe^(-2t),t,0,inf)"}, {"x=2"}, &globalContext);

#if 0 /* TODO_PCJ: incorrect number of solutions */
  assert_solves_to_infinite_solutions({"x-x=0"}, {"x=t"}, &globalContext);
  assert_solves_to_infinite_solutions(
      {"t*arctan(0000000000000000000000000000000000000000)=0"}, {"t=t1"}, &globalContext);
  assert_solves_to_infinite_solutions({"4y+(1-√(5))x=0", "x=(1+√(5))y"},
                                      {"x=√(5)t+t", "y=t"}, &globalContext);
  assert_solves_to_infinite_solutions({"x=x", "y=y"}, {"x=t2", "y=t1"}, &globalContext);
#endif

  assert_solves_to_infinite_solutions(
      {"x+y+z=0"}, {"x=-(t1+t2)", "y=t2", "z=t1"}, &globalContext);
  assert_solves_to_infinite_solutions({"x+y+z=0", "x+2y+3z=0"},
                                      {"x=t", "y=-2t", "z=t"}, &globalContext);

  assert_solves_to_infinite_solutions({"x+y+z=2", "2x+y-z=3", "3x+2y=5"},
                                      {"x=2t+1", "y=-3t+1", "z=t"},
                                      &globalContext);

  assert_solves_to_infinite_solutions(
      {"a=b", "c=d"}, {"a=t2", "b=t2", "c=t1", "d=t1"}, &globalContext);
  assert_solves_to_infinite_solutions({"a-b+c=0", "c-d=0"},
                                      {"a=-t1+t2", "b=t2", "c=t1", "d=t1"},
                                      &globalContext);

  assert_solves_to_infinite_solutions({"x=t"}, {"t=t1", "x=t1"},
                                      &globalContext);
  assert_solves_to_infinite_solutions(
      {"t1=t2+t3", "t5=t1-t6"},
      {"t1=t4+t7", "t2=t4+t7-t8", "t3=t8", "t5=t7", "t6=t4"}, &globalContext);

  assert_solves_to_infinite_solutions(
      {"a+2b+c+3d=0", "c-d=2"}, {"a=-4t1-2t2-2", "b=t2", "c=t1+2", "d=t1"},
      &globalContext);
  assert_solves_to_no_solution("2=0", &globalContext);
  assert_solves_to_no_solution("e=1", &globalContext);
  assert_solves_to_no_solution("i=5", &globalContext);
  assert_solves_to_no_solution("x-x+2=0", &globalContext);
  assert_solves_to_no_solution("x/x-1+x=0", &globalContext);

  /* TODO_PCJ: currently solves to EquationUndefined */
  // assert_solves_to_no_solution("x+y=0*diff(tan(2x),x,0,x)", &globalContext);

  assert_solves_to("√(x)^(2)=-1", {"x=-1"}, &globalContext);
  assert_solves_to("sin(asin(x))=2", {"x=2"}, &globalContext);

  assert_solves_to("x-0y=0", {"x=0", "y=t"}, &globalContext);
  assert_solves_to("x+cos(π/2)y=0", {"x=0", "y=t"}, &globalContext);
  assert_solves_to("x-x=0", {"x=t"}, &globalContext);
  // Technically solved in SolveLinearSystem, since the variable is reduced out
  assert_solves_to("0x^2=0", {"x=t"}, &globalContext);
  setComplexFormatAndAngleUnit(Cartesian, Radian);
  assert_solves_to({"x+i×y=0", "x+y×exp(i×π/3)=3"},
                   {"x=3/2-3/2×(2+√(3))×i", "y=3/2×(2+√(3))+3/2×i"},
                   &globalContext);
  setComplexFormatAndAngleUnit(Polar, Radian);
  assert_solves_to(
      {"x+i×y=0", "x+y×exp(i×π/3)=3"},
      {"x=3×(√(2)/2+√(6)/2)×exp(-i×π×5/12)", "y=3/2×(√(2)+√(6))×exp(i×π/12)"},
      &globalContext);

  // Restore default preferences
  setComplexFormatAndAngleUnit(Real, Radian);
}

QUIZ_CASE(solver_quadratic) {
  Shared::GlobalContext globalContext;
  setComplexFormatAndAngleUnit(Cartesian, Radian);

  assert_solves_to("(x-3)^2=0", {"x=3", "delta=0"}, &globalContext);
  assert_solves_to("(x-2π)(x/2-pi)=0", {"x=2π", "delta=0"}, &globalContext);

#if 0  // TODO_PCJ: the below equations fail to simplify
  assert_solves_to("(x-π)(x-ln(2))=0",
                   {"x=ln(2)", "x=π", "delta=ln(2)^2+π^2-2×π×ln(2)"}, &globalContext);
  assert_solves_to("(x-√(2))(x-√(3))=0",
                   {"x=√(2)", "x=√(3)", "delta=5-2×√(6)"}, &globalContext);
#endif

  assert_solves_to("x^2+1=0", {"x=-i", "x=i", "delta=-4"}, &globalContext);
  assert_solves_to("i/5×(x-3)^2=0", {"x=3", "delta=0"}, &globalContext);
  assert_solves_to("2i×(x-3i)^2=0", {"x=3×i", "delta=0"}, &globalContext);

  assert_solves_to("2×x^2-4×x+2=0", {"x=1", "delta=0"}, &globalContext);
  assert_solves_to("2×x^2-4×x+4=3", {"x=1-√(2)/2", "x=1+√(2)/2", "delta=8"},
                   &globalContext);
  assert_solves_to("3×x^2-4x+4=2",
                   {"x=2/3-√(2)/3×i", "x=2/3+√(2)/3×i", "delta=-8"},
                   &globalContext);
  assert_solves_to("x^2+x+1=3×x^2+π×x-√(5)",
                   {"x=-(-1+π+√(9-2×π+π^2+8×√(5)))/4",
                    "x=(1-π+√(9-2×π+π^2+8×√(5)))/4", "delta=9-2×π+π^2+8×√(5)"},
                   &globalContext);

  assert_solves_to("x^2+(1+2^(-52))*x+0.25+2^(-53)=0",
                   {"x=-(1+2^(-51))/2", "x=-1/2",
                    "delta=1/20282409603651670423947251286016"},
                   &globalContext);

  // Restore default preferences
  setComplexFormatAndAngleUnit(Real, Radian);
}

QUIZ_CASE(solver_cubic) {
  Shared::GlobalContext globalContext;
  setComplexFormatAndAngleUnit(Cartesian, Radian);

  assert_solves_to("x^3+x+1=0",
                   {"x=-0.6823278038", "x=0.3411639019-1.1615414×i",
                    "x=0.3411639019+1.1615414×i", "delta=-31"},
                   &globalContext);
  assert_solves_to("x^3+x^2+1=0",
                   {"x=-1.465571232", "x=0.2327856159-0.7925519925×i",
                    "x=0.2327856159+0.7925519925×i", "delta=-31"},
                   &globalContext);

#if 0  // TODO_PCJ: integer overflow raised without a TreeStackCheckpoint
  assert_solves_to("x^3+x^2=10^200", {"delta=-27×10^400+4×10^200"}, &globalContext);
#endif

  assert_solves_to("x^3-3x-2=0", {"x=-1", "x=2", "delta=0"}, &globalContext);
  assert_solves_to("x^3-4x^2+6x-24=0",
                   {"x=4", "x=-√(6)i", "x=√(6)i", "delta=-11616"},
                   &globalContext);

  assert_solves_to("4×x^3+3×x+i=0", {"x=-i/2", "x=i", "delta=0"},
                   &globalContext);
  assert_solves_to("x^3-8=0",
                   {"x=2", "x=-1-√(3)×i", "x=-1+√(3)×i", "delta=-1728"},
                   &globalContext);

  assert_solves_to("x^3-8i=0", {"x=-√(3)+i", "x=-2i", "x=√(3)+i", "delta=1728"},
                   &globalContext);

  /* NOTE: we used to only display the approximate form for the below case, this
   * can be discussed. */
  assert_solves_to("x^3-13-i=0",
                   {"x=-root(-13-i,3)", "x=-(root(13+i,3)(1+√(3)i))/2",
                    "x=root(13+i,3)", "delta=-4536-702×i"},
                   &globalContext);

  assert_solves_to("x^3-(2+i)×x^2-2×i×x-2+4×i=0",
                   {"x=-1-i", "x=1+i", "x=2+i", "delta=-96+40×i"},
                   &globalContext);
  assert_solves_to("x^3+3×x^2+3×x+0.7=0",
                   {"x=-0.3305670499", "x=-1.334716475-0.5797459409i",
                    "x=-1.334716475+0.5797459409i", "delta=-2.43"},
                   &globalContext);

  assert_solves_to(
      "x^3+x^2+x-39999999",
      {
          "x=341.6612041", "x=-171.3306021-296.1770828×i",
          "x=-171.3306021+296.1770828×i",
          "delta=-43199998400000016"  // should we display -4.32ᴇ16 here?
      },
      &globalContext);
  assert_solves_to(
      "x^3+x^2+x+1-80*π*200000",
      {
          "x=368.7200924", "x=-184.8600462-319.610685×i",
          "x=-184.8600462+319.610685×i",
          "delta=-16+640000000×π-6912000000000000×π^2"  // or approximate value?
      },
      &globalContext);
  assert_solves_to(
      "x^3+x^2+x-e^(20)",
      {"x=3927191891/5000000", "x=-3932191891/10000000-425311721/625000×i",
       "x=-3932191891/10000000+425311721/625000×i",
       "delta=-3-14×exp(20)-27×exp(40)"},
      &globalContext);

  assert_solves_to("(x-√(3)/2)(x^2-x+6/4)=0",
                   {"x=√(3)/2", "x=1/2-√(5)i/2", "x=1/2+√(5)i/2",
                    "delta=-465/16+45/4×exp(1/2×ln(3))"},
                   &globalContext);
  assert_solves_to("(x-2i+1)(x+3i-1)(x-i+2)=0",
                   {"x=-2+1×i", "x=-1+2×i", "x=1-3×i", "delta=-1288-666×i"},
                   &globalContext);

  assert_solves_to("4(2x-1)^3-12(2x-1)^2-95(2x-1)+325=0",
                   {"x=-2", "x=5/2-1/4×i", "x=5/2+1/4×i", "delta=-108160000"},
                   &globalContext);

  assert_solves_to(
      {"x^3+1111111116*x^2-6"},
      {"x=-1111111116", "x=-7363160451/100000000000000",
       "x=7339318593/100000000000000", "delta=32921811134156380512921812532"},
      &globalContext, 1e-2);

  // Restore default preferences
  setComplexFormatAndAngleUnit(Real, Radian);
}

QUIZ_CASE(solver_quadratic_real) {
  Shared::GlobalContext globalContext;
  setComplexFormatAndAngleUnit(Real, Radian);

  assert_solves_to("x^2-3×x+2=0", {"x=1", "x=2", "delta=1"}, &globalContext);
  assert_solves_to("3×x^2=0", {"x=0", "delta=0"}, &globalContext);
  assert_solves_to("1/3×x^2+2/3×x-5=0", {"x=-5", "x=3", "delta=64/9"},
                   &globalContext);
  assert_solves_to("(x-2/3)(x+0.2)=0", {"x=-1/5", "x=2/3", "delta=169/225"},
                   &globalContext);
  assert_solves_to("x^2+1", {"delta=-4"}, &globalContext);
  assert_solves_to("x^3+3×x^2+3×x+0.7=0", {"x=-0.3305670499", "delta=-2.43"},
                   &globalContext);
  // Complex format is updated with input
  assert_solves_to("(x+i)(x-i)", {"x=-i", "x=i", "delta=-4"}, &globalContext);
  assert_solves_to("(x-i)^2", {"x=i", "delta=0"}, &globalContext);

#if 0
  // TODO_PCJ: fails to simplify
  assert_solves_to("√(2)(x-√(3))(x-√(5))=0",
                   {"x=√(3)", "x=√(5)", "delta=16-4×√(15)"}, &globalContext);
  assert_solves_to("(x-7/3)(x-π)(x-log(3))=0",
                   {"x=log(3)", "x=7/3", "x=π", "delta=1.598007ᴇ1"}, &globalContext);

  /* TODO_PCJ: the following expression raises
   * "assert(!layout->isSeparatorLayout())" on a "ThousandSeparator" layout in
   * Tokenizer::popToken(). A possible element of explanation is that a quotient
   * of two IntegerPosBig is created at some point */
  assert_solves_to("(x-4.231)^3=0", {"x=4231/1000", "delta=0"}, &globalContext);
#endif
}

QUIZ_CASE(solver_cubic_real) {
  Shared::GlobalContext globalContext;
  setComplexFormatAndAngleUnit(Real, Radian);

  assert_solves_to("x^3-3×x^2+3×x-1=0", {"x=1", "delta=0"}, &globalContext);
  assert_solves_to("x^3+x^2-15/4×x-9/2=0", {"x=-3/2", "x=2", "delta=0"},
                   &globalContext);

#if 0  // TODO_PCJ: fails to simplify
  assert_solves_to("1/9×(x+√(2))^3=0", {"x=-√(2)", "delta=0"}, &globalContext);
#endif

  assert_solves_to("(x-1)(x-2)(x-3)=0", {"x=1", "x=2", "x=3", "delta=4"},
                   &globalContext);

#if 0  // TODO_PCJ: delta fails to simplify
  assert_solves_to("(x-√(3)/2)(x^2-x+6/4)=0",
                   {"x=√(3)/2", "delta=(-465+180×√(3))/16"}, &globalContext);
#endif
}

QUIZ_CASE(solver_approximate) {
  Shared::GlobalContext globalContext;
  setComplexFormatAndAngleUnit(Cartesian, Degree);
  assert_solves_numerically_to("(3x)^3/(0.1-3x)^3=10^(-8)", -10, 10,
                               {0.000071660}, &globalContext);
  assert_solves_numerically_to("(x-1)/(2×(x-2)^2)=20.8", -10, 10,
                               {1.856511, 2.167528}, &globalContext);
  assert_solves_numerically_to("-2/(x-4)=-x^2+2x-4", -10, 10, {4.154435},
                               &globalContext);
  assert_solves_numerically_to("1/cos(x)=0", 0, 10000, {}, &globalContext);
  assert_solves_numerically_to("4.4ᴇ-9/(0.12+x)^2=1.1ᴇ-9/x^2", -10, 10,
                               {-0.04, 0.12}, &globalContext);
  assert_solves_numerically_to("8x^4-22x^2+15=0", -10, 10,
                               {-1.224745, -1.118034, 1.118034, 1.224745},
                               &globalContext);
  assert_solves_numerically_to("cos(x)=0", -100, 100, {-90.0, 90.0},
                               &globalContext);
  assert_solves_numerically_to(
      "cos(x)=0", -900, 1000,
      {-810.0, -630.0, -450.0, -270.0, -90.0, 90.0, 270.0, 450.0, 630.0, 810.0},
      &globalContext);
  assert_solves_numerically_to("e^x/1000=0", -1000, 1000, {}, &globalContext);
  assert_solves_numerically_to("e^x=0", -1000, 1000, {}, &globalContext);
  assert_solves_numerically_to("√(y)=0", -900, 1000, {0}, &globalContext, "y");
  assert_solves_numerically_to("√(y+1)=0", -900, 1000, {-1}, &globalContext,
                               "y");
  // The ends of the interval are solutions
  assert_solves_numerically_to(
      "sin(x)=0", 0, 10000,
      {0, 180, 360, 540, 720, 900, 1080, 1260, 1440, 1620}, &globalContext);
  assert_solves_numerically_to("(x-1)^2×(x+1)^2=0", -1, 1, {-1, 1},
                               &globalContext);
  assert_solves_numerically_to("(x-1.00001)^2×(x+1.00001)^2=0", -1, 1, {},
                               &globalContext);
  assert_solves_numerically_to("sin(x)=0", -180, 180, {-180, 0, 180},
                               &globalContext);

#if 0  // TODO_PCJ: currently solves to NoError
  assert_solves_to_error("conj(x)*x+1=0", RequireApproximateSolution, &globalContext);
  assert_solves_numerically_to("conj(x)*x+1=0", -100, 100, {}, &globalContext);
#endif

  assert_solves_to_error("(x-10)^7=0", RequireApproximateSolution,
                         &globalContext);
  assert_solves_numerically_to("(x-10)^7=0", -100, 100, {10}, &globalContext);

  assert_solves_numerically_to("abs(x-145)=0.75", 0, 300, {144.25, 145.75},
                               &globalContext);
  assert_solves_numerically_to(
      "6x^5-x^4-43x^3+42x^2+x-7=0", -10, 10,
      {-2.99103, -0.3591962, 0.6322375, 0.8400476, 2.044608}, &globalContext);

  // Filter out fake root and keep real ones
  assert_solves_numerically_to("floor(x)-0.5=0", -10, 10, {}, &globalContext);
  assert_solves_numerically_to("7×10^9×(3×10^8-x)/(3×10^8+x)=7×10^9-320", -10,
                               10, {6.857143}, &globalContext);
  assert_solves_numerically_to("250e^(1.6x)=10^8", -10, 10, {8.062012},
                               &globalContext);
  // Gentle slope, far from x=0
  assert_solves_numerically_to("10^(-4)×abs(x-10^4)=0", 9000, 11000, {10000},
                               &globalContext);
#if 0  // TODO_PCJ: the solution is not found
  // Steep slope, close to x=0
  assert_solves_numerically_to("10^4×abs(x-10^(-4))=0", -10, 10, {0.0001}, &globalContext);
#endif

  assert_solves_numerically_to("10^4×abs(x-10^(-4))+0.001=0", -10, 10, {},
                               &globalContext);

#if 0
  /* TODO: This does not work in real-mode because abs(x) is reduced to
   * sign(x)*x which does not always approximate to the same value. */
  set_complex_format(Real, &globalContext);
  assert_solves_numerically_to("10^4×abs(x-10^(-4))=0", -10, 10, {0.0001}, &globalContext);
#endif
  setComplexFormatAndAngleUnit(Real, Radian);
  /* This tests triggers the escape case [std::fabs(end->x() - middle->x()) ==
   * 0] in Solver::FindMinimalIntervalContainingDiscontinuity */
  assert_solves_numerically_to(
      "√(cos(x))=0.1", -224802933571584, -90000000000000,
      {-2.18499e+14, -2.12789e+14, -1.66872e+14, -1.64543e+14, -1.38559e+14,
       -1.14107e+14, -1.13126e+14},
      &globalContext);

  assert_solves_numerically_to("(4-x)/(((6+2x)/(x-1))-8)=-(x-1)/4", 0, 4, {3.0},
                               &globalContext);
  assert_solves_numerically_to(".91×ln(x - 4) + 22.796 = 26", 0, 100,
                               {37.814142093595251}, &globalContext);
  // Restore default preferences
  setComplexFormatAndAngleUnit(Real, Radian);
}

void set(const char* variable, const char* expression,
         Shared::GlobalContext* globalContext) {
  char buffer[50];
  int expressionLen = strlen(expression);
  strlcpy(buffer, expression, expressionLen + 1);
  const char* storeSymbol = "→";
  int storeSymbolLen = strlen(storeSymbol);
  strlcpy(buffer + expressionLen, storeSymbol, storeSymbolLen + 1);
  strlcpy(buffer + expressionLen + storeSymbolLen, variable,
          strlen(variable) + 1);
  store(buffer, globalContext);
}

QUIZ_CASE(solver_complex_real) {
  Shared::GlobalContext globalContext;
  setComplexFormatAndAngleUnit(Real, Radian);
  // We still want complex solutions if the input has some complex value
  assert_solves_to("x+i=0", "x=-i", &globalContext);
  assert_solves_to_error("x+√(-1)=0", EquationNonReal, &globalContext);
  assert_solves_to("x^2+x+1=0", {"delta=-3"}, &globalContext);
  assert_solves_to("x^2+x+π=0", {"delta=-4π+1"}, &globalContext);
  assert_solves_to_error("x^2-√(-1)=0", EquationNonReal, &globalContext);
  assert_solves_to_error("x+√(-1)×√(-1)=0", EquationNonReal, &globalContext);
  assert_solves_to("x*2+2i-10=0", "x=5-i", &globalContext);
  assert_solves_to_error("x+re(x)=2+i", RequireApproximateSolution,
                         &globalContext);

#if 0
  // TODO_PCJ: currently solves to EquationUndefined instead of EquationNonReal
  assert_solves_to_error("x-arcsin(10)=0", EquationNonReal, &globalContext);

  // TODO_PCJ: currently solves to RequireApproximateSolution
  assert_solves_to_no_solution("√(x)^(2)=-1", &globalContext);
#endif

  assert_solves_to_no_solution("sin(asin(x))=2", &globalContext);

  assert_solves_to("root(-8,3)*x+3=0", "x=3/2", &globalContext);

#if 0  // TODO_PCJ: currently solves to NoError (without any solutions)
  assert_solves_to_error("x√(cot(4π/5))=0", EquationUndefined, &globalContext);
  assert_solves_to_error({"x√(cot(4π/5))=0", "0=0"}, EquationUndefined, &globalContext);
#endif

  // With a predefined variable that should be ignored
  set("h", "3", &globalContext);
  assert_solves_to("(h-1)*(h-2)=0", {"h=1", "h=2", "delta=1"}, &globalContext);
  set("h", "1", &globalContext);
  assert_solves_to("h^2=-1", {"delta=-4"},
                   &globalContext);  // No real solutions
  set("h", "i+1", &globalContext);
  assert_solves_to("h^2=-1", {"delta=-4"},
                   &globalContext);  // No real solutions
  //  - We still want complex solutions if the input has some complex value
  set("h", "1", &globalContext);
  assert_solves_to("(h-i)^2=0", {"h=i", "delta=0"},
                   &globalContext);  // Complex solutions
  set("h", "i+1", &globalContext);
  assert_solves_to("(h-i)^2=0", {"h=i", "delta=0"},
                   &globalContext);  // Complex solutions
}

QUIZ_CASE(solver_complex_cartesian) {
  Shared::GlobalContext globalContext;
  setComplexFormatAndAngleUnit(Cartesian, Radian);
  assert_solves_to("x+i=0", "x=-i", &globalContext);
  assert_solves_to("x+√(-1)=0", "x=-i", &globalContext);
  assert_solves_to(
      "x^2+x+1=0",
      {"x=-1/2-√(3)/2×i", "x=-1/2+√(3)/2×i",  // TODO_PCJ: simplify (metric)
       "delta=-3"},
      &globalContext);
  assert_solves_to("x^2-√(-1)=0",
                   {"x=-√(2)/2-√(2)/2×i", "x=√(2)/2+√(2)/2×i", "delta=4i"},
                   &globalContext);
  assert_solves_to("x+√(-1)×√(-1)=0", "x=1", &globalContext);
  assert_solves_to("root(-8,3)*x+3=0", "x=-3/4+3/4×√(3)×i",
                   &globalContext);  //  TODO_PCJ: force cartesian format
  assert_solves_to("x^2+x/x-1=0", {"delta=0"}, &globalContext);

  /* TODO_PCJ: The next equation finds x=0 as a solution (and not x=1), which is
   * mathematically incorrect. */
  // assert_solves_to("x^2*(x-1)/x=0", {"x=1", "delta=1"}, &globalContext);

  // Restore default preferences
  setComplexFormatAndAngleUnit(Real, Radian);
}

QUIZ_CASE(solver_complex_polar) {
  Shared::GlobalContext globalContext;
  setComplexFormatAndAngleUnit(Polar, Radian);
  assert_solves_to("x+i=0", "x=e^(-(π/2)i)", &globalContext);
  assert_solves_to("x+√(-1)=0", "x=e^(-(π/2)i)", &globalContext);

  assert_solves_to("x^2+x+1=0",
                   {"x=e^(-(2π/3)i)", "x=e^((2π/3)i)", "delta=3e^(πi)"},
                   &globalContext);
#if 0
  /* TODO_PCJ: Improve polar format reduction (with double beautification in the
   * solver, currently solves to
   * (abs(3/8×abs(exp(2/3×Ln(-8)))×exp(arg(3/8×exp(2/3×Ln(-8)))×i))×
   * exp(arg(3/8×abs(exp(2/3×Ln(-8)))×exp(arg(3/8×exp(2/3×Ln(-8)))×i))×i)
   */
  assert_solves_to("root(-8,3)*x+3=0",
                   "x=(3×abs((-8)^(2/3)))/8×e^(arg((3×(-8)^(2/3))/8)×i)", &globalContext);
#endif
  assert_solves_to("x^2-√(-1)=0",
                   {"x=e^(-(3π/4)i)", "x=e^((π/4)i)", "delta=4e^((π/2)i)"},
                   &globalContext);

#if 0
  /* TODO_PCJ: when the equation has form "ax^3 + d = 0", display approximate
   * values instead of exact values if the expressions are too complicated */
  assert_solves_to("2x^3-e^(2iπ/7)=0",
                   {"x=(root(e^((2×π)/7×i)/2,3)×(-1+√(3)×i))/2",
                    "x=-(root(e^((2×π)/7×i)/2,3)×(1+√(3)×i))/2",
                    "x=root(e^((2×π)/7×i)/2,3)", "delta=-108e^(((4π)/7)i)"}, &globalContext);
  assert_solves_to(
      "x^3-e^(2iπ/7)-1=0",
      {"x=((root(1+e^(((2π)/7)i),3)(-1+√(3)i))/2)",
       "x=((root(1+e^(((2π)/7)i),3)(-1-√(3)i))/2)", "x=root(1+e^(((2π)/7)i),3)",
       "delta=-27(1+e^(((2π)/7)i))^2"}, &globalContext);
#endif

  // Restore default preferences
  setComplexFormatAndAngleUnit(Real, Radian);
}

QUIZ_CASE(solver_symbolic_computation) {
  Shared::GlobalContext globalContext;
  setComplexFormatAndAngleUnit(Cartesian, Radian);
  /* This test case needs the user defined variable. Indeed, in the equation
   * store, m_variables is just before m_userVariables, so bad fetching in
   * m_variables might fetch into m_userVariables and create problems. */
  set("x", "0", &globalContext);
  assert_solves_to_infinite_solutions(
      {"D=0", "b=0", "c=0", "x+y+z+t=0"},
      {"D=0", "b=0", "c=0", "t=-(t1+t2)", "y=t2", "z=t1"}, &globalContext);
  Ion::Storage::FileSystem::sharedFileSystem->destroyAllRecords();

  // Long variable names
  assert_solves_to("2\"abcde\"+3=4", "\"abcde\"=1/2", &globalContext);
  assert_solves_to({"\"Big1\"+\"Big2\"=0", "3\"Big1\"+\"Big2\"=-5"},
                   {"\"Big1\"=-5/2", "\"Big2\"=5/2"}, &globalContext);

  /* Without the user defined variable, this test has too many variables.
   * With the user defined variable, it has no solutions. */
  set("g", "0", &globalContext);
  assert_solves_to_no_solution({"a=a+1", "a+b+c+d+f+g+h=0"}, &globalContext);
  Ion::Storage::FileSystem::sharedFileSystem->destroyAllRecords();

  set("a", "x", &globalContext);
  // a is undef
  assert_solves_to("a=0", {"a=0"}, &globalContext);

  Ion::Storage::FileSystem::sharedFileSystem->destroyAllRecords();

  assert_solves_to_infinite_solutions({"x+a=0"}, {"a=-t", "x=t"},
                                      &globalContext);

  set("a", "-3", &globalContext);
  assert_solves_to("x+a=0", "x=3", &globalContext);

  assert_solves_to("a=0", "a=0", &globalContext);
  /* The equation has no solution since the user defined a = -3. So a is not
   * replaced with its context value, and the solution is a = 0. */

  set("b", "-4", &globalContext);
  assert_solves_to_infinite_solutions({"a+b=0"}, {"a=-t", "b=t"},
                                      &globalContext);
  /* The equation has no solution since the user defined a = -3 and b = -4.
   * So neither a nor b are replaced with their context values. Therefore the
   * solution is an infinity of solutions. */

  assert_solves_to("a+b+c=0", "c=7", &globalContext);

  assert_solves_to({"a+c=0", "a=3"}, {"a=3", "c=-3"}, &globalContext);
  /* The system has no solution since the user defined a = -3. So a is not
   * replaced with its context value, and the solution is a = 3 and c = -3. */

  set("f(x)", "x+1", &globalContext);

  assert_solves_to("f(x)=0", "x=-1", &globalContext);

  assert_solves_to("f(a)=0", "a=-1", &globalContext);
  /* The equation has no solution since the user defined a = -3. So a is not
   * replaced with its context value, and the solution is a = -1. */

  set("g(x)", "a+x+2", &globalContext);

  assert_solves_to("g(x)=0", "x=1", &globalContext);

  assert_solves_to("g(a)=0", "a=-1", &globalContext);
  /* The equation has no solution since the user defined a = -3. So a is not
   * replaced with its context value, and the equation becomes a + a + 2 = 0.
   * The solution is therefore a = -1. */

#if 0
  set("d", "5", &globalContext);
  set("c", "d", &globalContext);
  set("h(x)", "c+d+3", &globalContext);
  assert_solves_to_infinite_solutions({"h(x)=0", "c=-3"},
                                      {"c=-3", "d=0", "x=t"}, &globalContext);
  // c and d context values should not be used
#endif

  assert_solves_to({"c+d=5", "c-d=1"}, {"c=3", "d=2"}, &globalContext);

  set("j", "8_g", &globalContext);
  assert_solves_to("j+1=0", {"j=-1"}, &globalContext);

  Ion::Storage::FileSystem::sharedFileSystem->destroyAllRecords();

  set("a", "0", &globalContext);
  assert_solves_to("a=0", "a=0", &globalContext);
  Ion::Storage::FileSystem::sharedFileSystem->destroyAllRecords();

  set("b", "0", &globalContext);
  assert_solves_to_no_solution({"b*b=1", "a=b"}, &globalContext);
  // If predefined variable had been ignored, there would have been this error
  // assert_solves_to_error({"b*b=1","a=b"}, NonLinearSystem, &globalContext);
  Ion::Storage::FileSystem::sharedFileSystem->destroyAllRecords();

  set("x", "-1", &globalContext);
  assert_solves_to_error("x^5+x^2+x+1=0", RequireApproximateSolution,
                         &globalContext);
  set("x", "1", &globalContext);
  assert_solves_to_error("x^5+x^2+x+1=0", RequireApproximateSolution,
                         &globalContext);
  Ion::Storage::FileSystem::sharedFileSystem->destroyAllRecords();

  set("t", "1", &globalContext);
  set("a", "2", &globalContext);
  assert_solves_to_infinite_solutions({"ax=y"}, {"x=t1/2", "y=t1"},
                                      &globalContext);
  Ion::Storage::FileSystem::sharedFileSystem->destroyAllRecords();

  set("a", "0", &globalContext);
  assert_solves_to_error("cos(πx)+cos(a)=0", RequireApproximateSolution,
                         &globalContext);
  // Value of a was not ignored, which would have resulted in a NonLinearSystem
  Ion::Storage::FileSystem::sharedFileSystem->destroyAllRecords();

  set("c", "arcsin(10)cb=0", &globalContext);
  assert_solves_to_error("arcsin(10)cb=0", NonLinearSystem, &globalContext);
  Ion::Storage::FileSystem::sharedFileSystem->destroyAllRecords();

  // Restore default preferences
  setComplexFormatAndAngleUnit(Real, Radian);
}
