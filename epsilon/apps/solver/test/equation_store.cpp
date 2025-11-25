#include <poincare/symbol_context.h>
#include <poincare/test/helper.h>
#include <poincare/test/helpers/symbol_store.h>
#include <quiz.h>
#include <string.h>

#include "helpers.h"

QUIZ_CASE(solver_error) {
  // Denominator containing variable
  assert_solves_to_error("80=x^2/(3(x-50)+√(2x+9(x-50)^2))",
                         RequireApproximateSolution);

  setComplexFormatAndAngleUnit(Cartesian, Radian);
  assert_solves_to_error("cos(x)=0", RequireApproximateSolution);

  assert_solves_to_error("x+y+z+a+b+c+d=0", TooManyVariables);

  assert_solves_to_error("x^2+y=0", NonLinearSystem);

#if 0 /* TODO_PCJ: currently solves to NoError */
  assert_solves_to_error("x^3+x^2+1=int(1/t,t,0,1)", EquationUndefined);
  assert_solves_to_error("x×(x^2×int(1/t,t,0,1)+1)=0", EquationUndefined);
#endif

  assert_solves_to_error("x-[[2,3]]=0", EquationUndefined);
  assert_solves_to_error("x[[2,3]]=0", EquationUndefined);
  assert_solves_to_error("x-{2,3}=0", EquationUndefined);
  assert_solves_to_error("x{2,3}=0", EquationUndefined);

  assert_solves_to_error("x-random()=0", EquationUndefined);

  assert_solves_to_error("0*√(x)=0", EquationUnhandled);
  assert_solves_to_error("x+0/a=0", EquationUnhandled);

  // Restore default preferences
  setComplexFormatAndAngleUnit(Real, Radian);
}

QUIZ_CASE(solver_linear_system) {
  setComplexFormatAndAngleUnit(Cartesian, Radian);
  assert_solves_to({"x+y=0", "3x+y+z=-5", "4z-π=0", "a+b+c=0", "a=3", "c=a+2"},
                   {"a=3", "b=-8", "c=5", "x=-5/2-π/8", "y=5/2+π/8", "z=π/4"});
  assert_solves_to("2x+3=4", "x=1/2");
  assert_solves_to({"x+y=0", "3x+y+z=-5", "4z-π=0"},
                   {"x=-5/2-π/8", "y=5/2+π/8", "z=π/4"});
  assert_solves_to({"x+y=0", "3x+y=-5"}, {"x=-5/2", "y=5/2"});
  assert_solves_to_infinite_solutions("0=0");
  assert_solves_to_infinite_solutions({"x+y=0"}, {"x=-t", "y=t"});

  assert_solves_to({"x=int(e^(-t),t,0,inf)"}, {"x=1"});
  assert_solves_to({"x*int(e^(-2t),t,0,inf)=1"}, {"x=2"});
  assert_solves_to({"x=1+int(xe^(-2t),t,0,inf)"}, {"x=2"});

#if 0 /* TODO_PCJ: incorrect number of solutions */
  assert_solves_to_infinite_solutions({"x-x=0"}, {"x=t"});
  assert_solves_to_infinite_solutions(
      {"t*arctan(0000000000000000000000000000000000000000)=0"}, {"t=t1"});
  assert_solves_to_infinite_solutions({"4y+(1-√(5))x=0", "x=(1+√(5))y"},
                                      {"x=√(5)t+t", "y=t"});
  assert_solves_to_infinite_solutions({"x=x", "y=y"}, {"x=t2", "y=t1"});
#endif
  assert_solves_to_infinite_solutions({"x+y+z=0"},
                                      {"x=-t1-t2", "y=t2", "z=t1"});
  assert_solves_to_infinite_solutions({"x+y+z=0", "x+2y+3z=0"},
                                      {"x=t", "y=-2t", "z=t"});

  assert_solves_to_infinite_solutions({"x+y+z=2", "2x+y-z=3", "3x+2y=5"},
                                      {"x=2t+1", "y=-3t+1", "z=t"});

  assert_solves_to_infinite_solutions({"a=b", "c=d"},
                                      {"a=t2", "b=t2", "c=t1", "d=t1"});
  assert_solves_to_infinite_solutions({"a-b+c=0", "c-d=0"},
                                      {"a=-t1+t2", "b=t2", "c=t1", "d=t1"});

  assert_solves_to_infinite_solutions({"x=t"}, {"t=t1", "x=t1"});
  assert_solves_to_infinite_solutions(
      {"t1=t2+t3", "t5=t1-t6"},
      {"t1=t4+t7", "t2=t4+t7-t8", "t3=t8", "t5=t7", "t6=t4"});

  assert_solves_to_infinite_solutions(
      {"a+2b+c+3d=0", "c-d=2"}, {"a=-2-4×t1-2×t2", "b=t2", "c=t1+2", "d=t1"});
  assert_solves_to_no_solution("2=0");
  assert_solves_to_no_solution("e=1");
  assert_solves_to_no_solution("i=5");
  assert_solves_to_no_solution("x-x+2=0");
  assert_solves_to_no_solution("x/x-1+x=0");

  /* TODO_PCJ: currently solves to EquationUndefined */
  // assert_solves_to_no_solution("x+y=0*diff(tan(2x),x,0,x)");

  assert_solves_to("√(x)^(2)=-1", {"x=-1"});
  assert_solves_to("sin(asin(x))=2", {"x=2"});

  assert_solves_to("x-0y=0", {"x=0", "y=t"});
  assert_solves_to("x+cos(π/2)y=0", {"x=0", "y=t"});
  assert_solves_to("x-x=0", {"x=t"});
  // Technically solved in SolveLinearSystem, since the variable is reduced out
  assert_solves_to("0x^2=0", {"x=t"});
  setComplexFormatAndAngleUnit(Cartesian, Radian);
  assert_solves_to({"x+i×y=0", "x+y×exp(i×π/3)=3"},
                   {"x=3/2-3×(2+√(3))/2×i", "y=3+3/2×√(3)+3/2×i"});
  setComplexFormatAndAngleUnit(Polar, Radian);
  assert_solves_to({"x+i×y=0", "x+y×exp(i×π/3)=3"},
                   {"x=-3×i/(e^(π×i/3)-i)", "y=(3×(√(2)+√(6))×e^(iπ/12))/2"});

  // Restore default preferences
  setComplexFormatAndAngleUnit(Real, Radian);
}

QUIZ_CASE(solver_quadratic) {
  setComplexFormatAndAngleUnit(Cartesian, Radian);

  assert_solves_to("(x-3)^2=0", {"x=3", "delta=0"});
  assert_solves_to("(x-2π)(x/2-pi)=0", {"x=2π", "delta=0"});

#if 0  // TODO_PCJ: the below equations fail to simplify
  assert_solves_to("(x-π)(x-ln(2))=0",
                   {"x=ln(2)", "x=π", "delta=ln(2)^2+π^2-2×π×ln(2)"});
  assert_solves_to("(x-√(2))(x-√(3))=0",
                   {"x=√(2)", "x=√(3)", "delta=5-2×√(6)"});
#endif

  assert_solves_to("x^2+1=0", {"x=-i", "x=i", "delta=-4"});
  assert_solves_to("i/5×(x-3)^2=0", {"x=3", "delta=0"});
  assert_solves_to("2i×(x-3i)^2=0", {"x=3×i", "delta=0"});

  assert_solves_to("2×x^2-4×x+2=0", {"x=1", "delta=0"});
  assert_solves_to("2×x^2-4×x+4=3", {"x=1-√(2)/2", "x=1+√(2)/2", "delta=8"});
  assert_solves_to("3×x^2-4x+4=2",
                   {"x=2/3-√(2)/3×i", "x=2/3+√(2)/3×i", "delta=-8"});
  assert_solves_to("x^2+x+1=3×x^2+π×x-√(5)",
                   {"x=(1-π-√(9-2×π+π^2+8×√(5)))/4",
                    "x=(1-π+√(9-2×π+π^2+8×√(5)))/4", "delta=9-2×π+π^2+8×√(5)"});

  assert_solves_to("x^2+(1+2^(-52))*x+0.25+2^(-53)=0",
                   {"x=-(1+2^(-51))/2", "x=-1/2",
                    "delta=1/20282409603651670423947251286016"});

  // Restore default preferences
  setComplexFormatAndAngleUnit(Real, Radian);
}

QUIZ_CASE(solver_cubic) {
  setComplexFormatAndAngleUnit(Cartesian, Radian);

  assert_solves_to("x^3+x+1=0",
                   {"x=-0.6823278038", "x=0.3411639019-1.1615414×i",
                    "x=0.3411639019+1.1615414×i", "delta=-31"});
  assert_solves_to("x^3+x^2+1=0",
                   {"x=-1.465571232", "x=0.2327856159-0.7925519925×i",
                    "x=0.2327856159+0.7925519925×i", "delta=-31"});

#if 0  // TODO_PCJ: integer overflow raised without a TreeStackCheckpoint
  assert_solves_to("x^3+x^2=10^200", {"delta=-27×10^400+4×10^200"});
#endif

  assert_solves_to("x^3-3x-2=0", {"x=-1", "x=2", "delta=0"});
  assert_solves_to("x^3-4x^2+6x-24=0",
                   {"x=4", "x=-√(6)i", "x=√(6)i", "delta=-11616"});

  assert_solves_to("4×x^3+3×x+i=0", {"x=-i/2", "x=i", "delta=0"});
  assert_solves_to("x^3-8=0",
                   {"x=2", "x=-1-√(3)×i", "x=-1+√(3)×i", "delta=-1728"});

  assert_solves_to("x^3-8i=0",
                   {"x=-√(3)+i", "x=-2i", "x=√(3)+i", "delta=1728"});

  /* NOTE: we used to only display the approximate form for the below case, this
   * can be discussed. */
  assert_solves_to("x^3-13-i=0",
                   {"x=-root(-13-i,3)", "x=-(root(13+i,3)(1+√(3)i))/2",
                    "x=root(13+i,3)", "delta=-4536-702×i"});

  assert_solves_to("x^3-(2+i)×x^2-2×i×x-2+4×i=0",
                   {"x=-1-i", "x=1+i", "x=2+i", "delta=-96+40×i"});
  assert_solves_to("x^3+3×x^2+3×x+0.7=0",
                   {"x=-0.3305670499", "x=-1.334716475-0.5797459409i",
                    "x=-1.334716475+0.5797459409i", "delta=-2.43"});

  assert_solves_to(
      "x^3+x^2+x-39999999",
      {
          "x=341.6612041", "x=-171.3306021-296.1770828×i",
          "x=-171.3306021+296.1770828×i",
          "delta=-43199998400000016"  // should we display -4.32ᴇ16 here?
      });
  assert_solves_to(
      "x^3+x^2+x+1-80*π*200000",
      {
          "x=368.7200924", "x=-184.8600462-319.610685×i",
          "x=-184.8600462+319.610685×i",
          "delta=-16+640000000×π-6912000000000000×π^2"  // or approximate value?
      });
  assert_solves_to(
      "x^3+x^2+x-e^(20)",
      {"x=3927191891/5000000", "x=-3932191891/10000000-425311721/625000×i",
       "x=-3932191891/10000000+425311721/625000×i",
       "delta=-3-14×exp(20)-27×exp(40)"});

  assert_solves_to("(x-√(3)/2)(x^2-x+6/4)=0",
                   {"x=√(3)/2", "x=1/2-√(5)i/2", "x=1/2+√(5)i/2",
                    "delta=-465/16+45/4×exp(1/2×ln(3))"});
  assert_solves_to("(x-2i+1)(x+3i-1)(x-i+2)=0",
                   {"x=-2+1×i", "x=-1+2×i", "x=1-3×i", "delta=-1288-666×i"});

  assert_solves_to("4(2x-1)^3-12(2x-1)^2-95(2x-1)+325=0",
                   {"x=-2", "x=5/2-1/4×i", "x=5/2+1/4×i", "delta=-108160000"});

  assert_solves_to(
      {"x^3+1111111116*x^2-6"},
      {"x=-1111111116", "x=-7363160451/100000000000000",
       "x=7339318593/100000000000000", "delta=32921811134156380512921812532"},
      Poincare::EmptySymbolContext{}, 1e-2);

  // Restore default preferences
  setComplexFormatAndAngleUnit(Real, Radian);
}

QUIZ_CASE(solver_quadratic_real) {
  setComplexFormatAndAngleUnit(Real, Radian);

  assert_solves_to("x^2-3×x+2=0", {"x=1", "x=2", "delta=1"});
  assert_solves_to("3×x^2=0", {"x=0", "delta=0"});
  assert_solves_to("1/3×x^2+2/3×x-5=0", {"x=-5", "x=3", "delta=64/9"});
  assert_solves_to("(x-2/3)(x+0.2)=0", {"x=-1/5", "x=2/3", "delta=169/225"});
  assert_solves_to("x^2+1", {"delta=-4"});
  assert_solves_to("x^3+3×x^2+3×x+0.7=0", {"x=-0.3305670499", "delta=-2.43"});
  // Complex format is updated with input
  assert_solves_to("(x+i)(x-i)", {"x=-i", "x=i", "delta=-4"});
  assert_solves_to("(x-i)^2", {"x=i", "delta=0"});

#if 0
  // TODO_PCJ: fails to simplify
  assert_solves_to("√(2)(x-√(3))(x-√(5))=0",
                   {"x=√(3)", "x=√(5)", "delta=16-4×√(15)"});
  assert_solves_to("(x-7/3)(x-π)(x-log(3))=0",
                   {"x=log(3)", "x=7/3", "x=π", "delta=1.598007ᴇ1"});

  /* TODO_PCJ: the following expression raises
   * "assert(!layout->isSeparatorLayout())" on a "ThousandSeparator" layout in
   * Tokenizer::popToken(). A possible element of explanation is that a quotient
   * of two IntegerPosBig is created at some point */
  assert_solves_to("(x-4.231)^3=0", {"x=4231/1000", "delta=0"});
#endif
}

QUIZ_CASE(solver_cubic_real) {
  setComplexFormatAndAngleUnit(Real, Radian);

  assert_solves_to("x^3-3×x^2+3×x-1=0", {"x=1", "delta=0"});
  assert_solves_to("x^3+x^2-15/4×x-9/2=0", {"x=-3/2", "x=2", "delta=0"});

  assert_solves_to("x^3+8/27=0", {"x=-2/3", "delta=-64/27"});

#if 0  // TODO_PCJ: fails to simplify
  assert_solves_to("1/9×(x+√(2))^3=0", {"x=-√(2)", "delta=0"});
#endif

  assert_solves_to("(x-1)(x-2)(x-3)=0", {"x=1", "x=2", "x=3", "delta=4"});

#if 0  // TODO_PCJ: delta fails to simplify
  assert_solves_to("(x-√(3)/2)(x^2-x+6/4)=0",
                   {"x=√(3)/2", "delta=(-465+180×√(3))/16"});
#endif
}

QUIZ_CASE(solver_approximate) {
  setComplexFormatAndAngleUnit(Cartesian, Degree);
  assert_solves_numerically_to("(3x)^3/(0.1-3x)^3=10^(-8)", -10, 10,
                               {0.000071660});
  assert_solves_numerically_to("(x-1)/(2×(x-2)^2)=20.8", -10, 10,
                               {1.856511, 2.167528});
  assert_solves_numerically_to("-2/(x-4)=-x^2+2x-4", -10, 10, {4.154435});
  assert_solves_numerically_to("1/cos(x)=0", 0, 10000, {});
  assert_solves_numerically_to("4.4ᴇ-9/(0.12+x)^2=1.1ᴇ-9/x^2", -10, 10,
                               {-0.04, 0.12});
  assert_solves_numerically_to("8x^4-22x^2+15=0", -10, 10,
                               {-1.224745, -1.118034, 1.118034, 1.224745});
  assert_solves_numerically_to("cos(x)=0", -100, 100, {-90.0, 90.0});
  assert_solves_numerically_to("cos(x)=0", -900, 1000,
                               {-810.0, -630.0, -450.0, -270.0, -90.0, 90.0,
                                270.0, 450.0, 630.0, 810.0});
  assert_solves_numerically_to("e^x/1000=0", -1000, 1000, {});
  assert_solves_numerically_to("e^x=0", -1000, 1000, {});
  assert_solves_numerically_to("√(y)=0", -900, 1000, {0},
                               Poincare::EmptySymbolContext{}, "y");
  assert_solves_numerically_to("√(y+1)=0", -900, 1000, {-1},
                               Poincare::EmptySymbolContext{}, "y");
  // The ends of the interval are solutions
  assert_solves_numerically_to(
      "sin(x)=0", 0, 10000,
      {0, 180, 360, 540, 720, 900, 1080, 1260, 1440, 1620});
  assert_solves_numerically_to("(x-1)^2×(x+1)^2=0", -1, 1, {-1, 1});
  assert_solves_numerically_to("(x-1.00001)^2×(x+1.00001)^2=0", -1, 1, {});
  assert_solves_numerically_to("sin(x)=0", -180, 180, {-180, 0, 180});

#if 0  // TODO_PCJ: currently solves to NoError
  assert_solves_to_error("conj(x)*x+1=0", RequireApproximateSolution);
  assert_solves_numerically_to("conj(x)*x+1=0", -100, 100, {});
#endif

  assert_solves_to_error("(x-10)^7=0", RequireApproximateSolution);
  assert_solves_numerically_to("(x-10)^7=0", -100, 100, {10});

  assert_solves_numerically_to("abs(x-145)=0.75", 0, 300, {144.25, 145.75});
  assert_solves_numerically_to(
      "6x^5-x^4-43x^3+42x^2+x-7=0", -10, 10,
      {-2.99103, -0.3591962, 0.6322375, 0.8400476, 2.044608});

  // Filter out fake root and keep real ones
  assert_solves_numerically_to("floor(x)-0.5=0", -10, 10, {});
  assert_solves_numerically_to("7×10^9×(3×10^8-x)/(3×10^8+x)=7×10^9-320", -10,
                               10, {6.857143});
  assert_solves_numerically_to("250e^(1.6x)=10^8", -10, 10, {8.062012});
  // Gentle slope, far from x=0
  assert_solves_numerically_to("10^(-4)×abs(x-10^4)=0", 9000, 11000, {10000});
#if 0  // TODO_PCJ: the solution is not found
  // Steep slope, close to x=0
  assert_solves_numerically_to("10^4×abs(x-10^(-4))=0", -10, 10, {0.0001});
#endif

  assert_solves_numerically_to("10^4×abs(x-10^(-4))+0.001=0", -10, 10, {});

#if 0
  /* TODO: This does not work in real-mode because abs(x) is reduced to
   * sign(x)*x which does not always approximate to the same value. */
  set_complex_format(Real);
  assert_solves_numerically_to("10^4×abs(x-10^(-4))=0", -10, 10, {0.0001});
#endif
  setComplexFormatAndAngleUnit(Real, Radian);
  /* This tests triggers the escape case [std::fabs(end->x() - middle->x()) ==
   * 0] in Solver::FindMinimalIntervalContainingDiscontinuity.
   * TODO: This test finds roots that are wrong because of how the solver
   * (badly) handles precisions for large x values. */
  assert_solves_numerically_to(
      "√(cos(x))=0.1", -224802933571584, -90000000000000,
      {-2.23822e+14, -2.18499e+14, -2.12789e+14, -2.01024e+14, -1.95777e+14,
       -1.83716e+14, -1.81387e+14, -1.70970e+14, -1.66871e+14, -1.64543e+14});

  assert_solves_numerically_to("(4-x)/(((6+2x)/(x-1))-8)=-(x-1)/4", 0, 4,
                               {3.0});
  assert_solves_numerically_to(".91×ln(x - 4) + 22.796 = 26", 0, 100,
                               {37.814142093595251});
  // Restore default preferences
  setComplexFormatAndAngleUnit(Real, Radian);
}

QUIZ_CASE(solver_complex_real) {
  setComplexFormatAndAngleUnit(Real, Radian);
  // We still want complex solutions if the input has some complex value
  assert_solves_to("x+i=0", "x=-i");
  assert_solves_to_error("x+√(-1)=0", EquationNonReal);
  assert_solves_to("x^2+x+1=0", {"delta=-3"});
  assert_solves_to("x^2+x+π=0", {"delta=-4π+1"});
  assert_solves_to_error("x^2-√(-1)=0", EquationNonReal);
  assert_solves_to_error("x+√(-1)×√(-1)=0", EquationNonReal);
  assert_solves_to("x*2+2i-10=0", "x=5-i");
  assert_solves_to_error("x+re(x)=2+i", RequireApproximateSolution);

#if 0
  // TODO_PCJ: currently solves to EquationUndefined instead of EquationNonReal
  assert_solves_to_error("x-arcsin(10)=0", EquationNonReal);

  // TODO_PCJ: currently solves to RequireApproximateSolution
  assert_solves_to_no_solution("√(x)^(2)=-1");
#endif

  assert_solves_to_no_solution("sin(asin(x))=2");

  assert_solves_to("root(-8,3)*x+3=0", "x=3/2");

#if 0  // TODO_PCJ: currently solves to NoError (without any solutions)
  assert_solves_to_error("x√(cot(4π/5))=0", EquationUndefined);
  assert_solves_to_error({"x√(cot(4π/5))=0", "0=0"}, EquationUndefined);
#endif

  // With a predefined variable that should be ignored
  PoincareTest::SymbolStore symbolStore;
  PoincareTest::store("h", "3", symbolStore);
  assert_solves_to("(h-1)*(h-2)=0", {"h=1", "h=2", "delta=1"}, symbolStore);
  PoincareTest::store("h", "1", symbolStore);
  assert_solves_to("h^2=-1", {"delta=-4"},
                   symbolStore);  // No real solutions
  PoincareTest::store("h", "i+1", symbolStore);
  assert_solves_to("h^2=-1", {"delta=-4"},
                   symbolStore);  // No real solutions
  //  - We still want complex solutions if the input has some complex value
  PoincareTest::store("h", "1", symbolStore);
  assert_solves_to("(h-i)^2=0", {"h=i", "delta=0"},
                   symbolStore);  // Complex solutions
  PoincareTest::store("h", "i+1", symbolStore);
  assert_solves_to("(h-i)^2=0", {"h=i", "delta=0"},
                   symbolStore);  // Complex solutions
}

QUIZ_CASE(solver_complex_cartesian) {
  setComplexFormatAndAngleUnit(Cartesian, Radian);
  assert_solves_to("x+i=0", "x=-i");
  assert_solves_to("x+√(-1)=0", "x=-i");
  assert_solves_to(
      "x^2+x+1=0",
      {"x=-1/2-√(3)/2×i", "x=-1/2+√(3)/2×i",  // TODO_PCJ: simplify (metric)
       "delta=-3"});
  assert_solves_to("x^2-√(-1)=0",
                   {"x=-√(2)/2-√(2)/2×i", "x=√(2)/2+√(2)/2×i", "delta=4i"});
  assert_solves_to("x+√(-1)×√(-1)=0", "x=1");
  assert_solves_to("root(-8,3)*x+3=0",
                   "x=-3/4+3/4×√(3)×i");  //  TODO_PCJ: force cartesian format
  assert_solves_to("x^2+x/x-1=0", {"delta=0"});

  /* TODO_PCJ: The next equation finds x=0 as a solution (and not x=1), which is
   * mathematically incorrect. */
  // assert_solves_to("x^2*(x-1)/x=0", {"x=1", "delta=1"});

  // Restore default preferences
  setComplexFormatAndAngleUnit(Real, Radian);
}

QUIZ_CASE(solver_complex_polar) {
  setComplexFormatAndAngleUnit(Polar, Radian);
  assert_solves_to("x+i=0", "x=e^(-(π/2)i)");
  assert_solves_to("x+√(-1)=0", "x=e^(-(π/2)i)");

  assert_solves_to("x^2+x+1=0",
                   {"x=e^(-(2π/3)i)", "x=e^((2π/3)i)", "delta=3e^(πi)"});
#if 0
  /* TODO_PCJ: Improve polar format reduction (with double beautification in the
   * solver, currently solves to
   * (abs(3/8×abs(exp(2/3×Ln(-8)))×exp(arg(3/8×exp(2/3×Ln(-8)))×i))×
   * exp(arg(3/8×abs(exp(2/3×Ln(-8)))×exp(arg(3/8×exp(2/3×Ln(-8)))×i))×i)
   */
  assert_solves_to("root(-8,3)*x+3=0",
                   "x=(3×abs((-8)^(2/3)))/8×e^(arg((3×(-8)^(2/3))/8)×i)");
#endif
  assert_solves_to("x^2-√(-1)=0",
                   {"x=e^(-(3π/4)i)", "x=e^((π/4)i)", "delta=4e^((π/2)i)"});

#if 0
  /* TODO_PCJ: when the equation has form "ax^3 + d = 0", display approximate
   * values instead of exact values if the expressions are too complicated */
  assert_solves_to("2x^3-e^(2iπ/7)=0",
                   {"x=(root(e^((2×π)/7×i)/2,3)×(-1+√(3)×i))/2",
                    "x=-(root(e^((2×π)/7×i)/2,3)×(1+√(3)×i))/2",
                    "x=root(e^((2×π)/7×i)/2,3)", "delta=-108e^(((4π)/7)i)"});
  assert_solves_to(
      "x^3-e^(2iπ/7)-1=0",
      {"x=((root(1+e^(((2π)/7)i),3)(-1+√(3)i))/2)",
       "x=((root(1+e^(((2π)/7)i),3)(-1-√(3)i))/2)", "x=root(1+e^(((2π)/7)i),3)",
       "delta=-27(1+e^(((2π)/7)i))^2"});
#endif

  // Restore default preferences
  setComplexFormatAndAngleUnit(Real, Radian);
}

QUIZ_CASE(solver_symbolic_computation) {
  PoincareTest::SymbolStore symbolStore;
  setComplexFormatAndAngleUnit(Cartesian, Radian);
  /* This test case needs the user defined variable. Indeed, in the equation
   * store, m_variables is just before m_userVariables, so bad fetching in
   * m_variables might fetch into m_userVariables and create problems. */
  PoincareTest::store("x", "0", symbolStore);
  assert_solves_to_infinite_solutions(
      {"D=0", "b=0", "c=0", "x+y+z+t=0"},
      {"D=0", "b=0", "c=0", "t=-t1-t2", "y=t2", "z=t1"}, symbolStore);
  symbolStore.reset();

  // Long variable names
  assert_solves_to("2\"abcde\"+3=4", "\"abcde\"=1/2", symbolStore);
  assert_solves_to({"\"Big1\"+\"Big2\"=0", "3\"Big1\"+\"Big2\"=-5"},
                   {"\"Big1\"=-5/2", "\"Big2\"=5/2"}, symbolStore);

  /* Without the user defined variable, this test has too many variables.
   * With the user defined variable, it has no solutions. */
  PoincareTest::store("g", "0", symbolStore);
  assert_solves_to_no_solution({"a=a+1", "a+b+c+d+f+g+h=0"}, symbolStore);

  PoincareTest::store("a", "x", symbolStore);
  // a is undef
  assert_solves_to("a=0", {"a=0"}, symbolStore);

  symbolStore.reset();

  assert_solves_to_infinite_solutions({"x+a=0"}, {"a=-t", "x=t"}, symbolStore);

  PoincareTest::store("a", "-3", symbolStore);
  assert_solves_to("x+a=0", "x=3", symbolStore);

  assert_solves_to("a=0", "a=0", symbolStore);
  /* The equation has no solution since the user defined a = -3. So a is not
   * replaced with its context value, and the solution is a = 0. */

  PoincareTest::store("b", "-4", symbolStore);
  assert_solves_to_infinite_solutions({"a+b=0"}, {"a=-t", "b=t"}, symbolStore);
  /* The equation has no solution since the user defined a = -3 and b = -4.
   * So neither a nor b are replaced with their context values. Therefore the
   * solution is an infinity of solutions. */

  assert_solves_to("a+b+c=0", "c=7", symbolStore);

  assert_solves_to({"a+c=0", "a=3"}, {"a=3", "c=-3"}, symbolStore);
  /* The system has no solution since the user defined a = -3. So a is not
   * replaced with its context value, and the solution is a = 3 and c = -3. */

  PoincareTest::store("f(x)", "x+1", symbolStore);

  assert_solves_to("f(x)=0", "x=-1", symbolStore);

  assert_solves_to("f(a)=0", "a=-1", symbolStore);
  /* The equation has no solution since the user defined a = -3. So a is not
   * replaced with its context value, and the solution is a = -1. */

  PoincareTest::store("g(x)", "a+x+2", symbolStore);

  assert_solves_to("g(x)=0", "x=1", symbolStore);

  assert_solves_to("g(a)=0", "a=-1", symbolStore);
  /* The equation has no solution since the user defined a = -3. So a is not
   * replaced with its context value, and the equation becomes a + a + 2 = 0.
   * The solution is therefore a = -1. */

#if 0
  PoincareTest::store("d", "5", symbolStore);
  PoincareTest::store("c", "d", symbolStore);
  PoincareTest::store("h(x)", "c+d+3", symbolStore);
  assert_solves_to_infinite_solutions({"h(x)=0", "c=-3"},
                                      {"c=-3", "d=0", "x=t"}, symbolStore);
  // c and d context values should not be used
#endif

  assert_solves_to({"c+d=5", "c-d=1"}, {"c=3", "d=2"}, symbolStore);

  PoincareTest::store("j", "8_g", symbolStore);
  assert_solves_to("j+1=0", {"j=-1"}, symbolStore);

  symbolStore.reset();

  PoincareTest::store("a", "0", symbolStore);
  assert_solves_to("a=0", "a=0", symbolStore);
  symbolStore.reset();

  PoincareTest::store("b", "0", symbolStore);
  assert_solves_to_no_solution({"b*b=1", "a=b"}, symbolStore);
  // If predefined variable had been ignored, there would have been this error
  // assert_solves_to_error({"b*b=1","a=b"}, NonLinearSystem, symbolStore);
  symbolStore.reset();

  PoincareTest::store("x", "-1", symbolStore);
  assert_solves_to_error("x^5+x^2+x+1=0", RequireApproximateSolution,
                         symbolStore);
  PoincareTest::store("x", "1", symbolStore);
  assert_solves_to_error("x^5+x^2+x+1=0", RequireApproximateSolution,
                         symbolStore);
  symbolStore.reset();

  PoincareTest::store("t", "1", symbolStore);
  PoincareTest::store("a", "2", symbolStore);
  assert_solves_to_infinite_solutions({"ax=y"}, {"x=t1/2", "y=t1"},
                                      symbolStore);
  symbolStore.reset();

  PoincareTest::store("a", "0", symbolStore);
  assert_solves_to_error("cos(πx)+cos(a)=0", RequireApproximateSolution,
                         symbolStore);
  // Value of a was not ignored, which would have resulted in a NonLinearSystem
  symbolStore.reset();

  PoincareTest::store("c", "arcsin(10)cb=0", symbolStore);
  // TODO: EquationUndefined?
  // assert_solves_to_error("arcsin(10)cb=0", NonLinearSystem, symbolStore);
  symbolStore.reset();

  // Restore default preferences
  setComplexFormatAndAngleUnit(Real, Radian);
}
